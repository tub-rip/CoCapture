#include <iostream>

#include <pangolin/var/var.h>
#include <pangolin/gl/gl.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/display/widgets.h>
#include <pangolin/display/default_font.h>
#include <pangolin/var/varextra.h>
#include <opencv2/core.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>

#include "../lib/event_visualizer.h"
#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"


int main(int argc, const char *argv[]) {
    Parameters app_parameter;
    parse_comman_line(argc, argv, app_parameter);

    // Setup Prophesee Camera
    auto prophesee_camera = Metavision::Camera::from_first_available();
    std::cout << "Opening Prophesee Camera: " << prophesee_camera.get_camera_configuration().serial_number << std::endl;

    auto &geometry = prophesee_camera.geometry();
    int prophesee_width = geometry.width();
    int prophesee_height = geometry.height();
    app_parameter.target_width = prophesee_width;
    app_parameter.target_height = prophesee_height;

    EventVisualizer event_visualizer;
    event_visualizer.setup_display(prophesee_width, prophesee_height, app_parameter.show_snr);

    prophesee_camera.add_runtime_error_callback([](const Metavision::CameraException &e) {
        MV_LOG_ERROR() << e.what();
        throw;
    });

    prophesee_camera.cd().add_callback(
            [&event_visualizer](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                event_visualizer.process_events(begin, end);
            });

    prophesee_camera.cd().add_callback(
            [&event_visualizer](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                event_visualizer.estimate_snr(begin, end);
            });

    set_roi(prophesee_camera, app_parameter);
    prophesee_camera.start();

    // Setup Basler Camera
    Pylon::PylonInitialize();
    Pylon::CBaslerUniversalInstantCamera basler_camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    std::cout << "Opening Basler Camera: " << basler_camera.GetDeviceInfo().GetModelName() << std::endl;

    auto basler_event_handler = new basler::BaslerEventHandler(app_parameter.do_warp, app_parameter);

    basler_camera.RegisterImageEventHandler(basler_event_handler,
                                            Pylon::RegistrationMode_ReplaceAll,
                                            Pylon::Cleanup_Delete);

    basler_camera.Open();
    basler_camera.ReverseX.SetValue(true);
    basler_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne,
                                Pylon::GrabLoop_ProvidedByInstantCamera);

    // Define GUI
    pangolin::CreateWindowAndBind("Main", 640, 480);

    const int UI_WIDTH = 20 * (int) pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> start_recording("ui.Start-Recording", false, false);
    pangolin::Var<bool> stop_recording("ui.Stop-Recording", false, false);
    bool is_recording = false;
    int current_exposure_time = 5000;
    pangolin::Var<int> exposure_time("ui.exposure_time", current_exposure_time, 0, 15000);

    // Not the nicest solution but didn't find a way to directly initialize pangolin var in a std map
    auto bias_values = prophesee_camera.biases().get_facility()->get_all_biases();
    pangolin::Var<int> bias_fo("ui.bias_fo", bias_values["bias_fo"], 45, 110);
    pangolin::Var<int> bias_diff_off("ui.bias_diff_off", bias_values["bias_diff_off"], 25, 65);
    pangolin::Var<int> bias_diff_on("ui.bias_diff_on", bias_values["bias_diff_on"], 95, 140);
    pangolin::Var<int> bias_hpf("ui.bias_hpf", bias_values["bias_hpf"], 0, 120);
    pangolin::Var<int> bias_refr("ui.bias_refr", bias_values["bias_refr"], 20, 100);
    std::unordered_map<std::string, pangolin::Var<int> *> bias_setting{
            {"bias_fo",       &bias_fo},
            {"bias_diff_off", &bias_diff_off},
            {"bias_diff_on",  &bias_diff_on},
            {"bias_hpf",      &bias_hpf},
            {"bias_refr",     &bias_refr}
    };

    int current_snr = 0;
    pangolin::Var<int> snr("ui.snr_proxy", current_snr);

    // Image
    int basler_display_height = app_parameter.do_warp ? prophesee_height :
                                basler_camera.Height.GetValue();
    int display_height = app_parameter.overlay ?
                         prophesee_height : prophesee_height + basler_display_height;
    int display_width = prophesee_width;

    double aspect = (double) display_width / (double) display_height;
    pangolin::View &d_image = pangolin::Display("image")
            .SetAspect(aspect);

    pangolin::Display("multi")
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0)
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(d_image);

    pangolin::GlTexture imageTexture(display_width, display_height,
                                     GL_RGB, false, 0,
                                     GL_RGB, GL_UNSIGNED_BYTE);

    // Initialize container for depicting the image. This will contain the prophesee and the basler frames
    cv::Mat display(display_height, display_width,
                    CV_8UC3, cv::Vec3b(0, 0, 0));
    cv::Mat prophesee_display(display,
                              cv::Rect(0, 0,
                                       display_width,
                                       prophesee_height));
    cv::Mat basler_display;
    if (app_parameter.overlay) {
        basler_display = cv::Mat(basler_display_height, display_width,
                                 CV_8UC3, cv::Vec3b(0, 0, 0));
    } else {
        basler_display = cv::Mat(display,
                                 cv::Rect(0, prophesee_height,
                                          display_width,
                                          basler_display_height));
    }


    while (!pangolin::ShouldQuit()) {
        // Get images from the two cameras
        if (app_parameter.overlay) {
            basler_event_handler->get_display_frame(basler_display);
            event_visualizer.get_display_frame(prophesee_display, basler_display);
        } else {
            basler_event_handler->get_display_frame(basler_display);
            event_visualizer.get_display_frame(prophesee_display);
        }
        cv::flip(basler_display, basler_display, 0);

        // Image
        if (!display.empty()) {
            imageTexture.Upload(display.data, GL_RGB, GL_UNSIGNED_BYTE);
            d_image.Activate();
            glColor3f(1.0, 1.0, 1.0);
            imageTexture.RenderToViewport();
        }

        // GUI
        if (pangolin::Pushed(start_recording)) {

            if (!is_recording) {
                std::string out_file_path("events.raw");
                is_recording = true;
                std::cout << "Recording not implemented yet!" << std::endl;
            } else {
                std::cout << "Recording is running already" << std::endl;
            }
        }
        if (pangolin::Pushed(stop_recording)) {
            if (is_recording) {
                //i_events_stream->stop_log_raw_data();
                is_recording = false;
                std::cout << "Stopped recording";
            } else {
                std::cout << "Cannot stop recording, because it's not running" << std::endl;
            }
        }

        if (exposure_time != current_exposure_time) {
            basler_camera.ExposureTime.SetValue(exposure_time);
            current_exposure_time = exposure_time;
        }

        for (const auto &it: bias_setting) {
            auto name = it.first;
            auto input_value = *(it.second);
            if (bias_values[name] != input_value) {
                prophesee_camera.biases().get_facility()->set(name, input_value);
            }
        }

        // Necessary
        pangolin::FinishFrame();
    }

    Pylon::PylonTerminate();
    return 0;
}
