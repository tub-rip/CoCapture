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
#include <boost/program_options.hpp>

#include "../lib/event_visualizer.h"
#include "../lib/basler_event_handler.h"

namespace po = boost::program_options;


int main() {
    //


    // Setup Basler Camera
    Pylon::PylonInitialize();
    Pylon::CBaslerUniversalInstantCamera basler_camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    std::cout << "Opening Basler Camera: " << basler_camera.GetDeviceInfo().GetModelName() << std::endl;

    auto basler_event_handler = new basler::BaslerEventHandler();

    basler_camera.RegisterImageEventHandler(basler_event_handler,
                                            Pylon::RegistrationMode_ReplaceAll,
                                            Pylon::Cleanup_Delete);

    basler_camera.Open();
    basler_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne,
                                Pylon::GrabLoop_ProvidedByInstantCamera);

    // Setup Prophesee Camera

    auto prophesee_camera = Metavision::Camera::from_first_available();
    std::cout << "Opening Prophesee Camera: " << prophesee_camera.get_camera_configuration().serial_number << std::endl;

    auto &geometry = prophesee_camera.geometry();
    int prophesee_width = geometry.width();
    int prophesee_height = geometry.height();

    EventVisualizer event_visualizer;
    event_visualizer.setup_display(prophesee_width, prophesee_height);

    prophesee_camera.add_runtime_error_callback([](const Metavision::CameraException &e) {
        MV_LOG_ERROR() << e.what();
        throw;
    });

    prophesee_camera.cd().add_callback(
            [&event_visualizer](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                event_visualizer.process_events(begin, end);
            });

    prophesee_camera.start();

    // Define GUI

    pangolin::CreateWindowAndBind("Main", 640, 480);

    const int UI_WIDTH = 20 * (int) pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> start_recording("ui.Start-Recording", false, false);
    pangolin::Var<bool> stop_recording("ui.Stop-Recording", false, false);
    bool is_recording = false;
    /*
    pangolin::Var<double> a_double("ui.A_Double",3,0,5);
    pangolin::Var<int> an_int("ui.An_Int",2,0,5);
    pangolin::Var<double> a_double_log("ui.Log_scale",3,1,1E4, true);
    pangolin::Var<bool> a_checkbox("ui.A_Checkbox",false,true);
    pangolin::Var<int> an_int_no_input("ui.An_Int_No_Input",2);
    pangolin::Var<std::string> recording_status("ui.recording_status", "Ready");
     */

    // Image
    double aspect = (double) basler_camera.Width.GetValue() /
                    (double) (prophesee_height + basler_camera.Height.GetValue());
    pangolin::View &d_image = pangolin::Display("image")
            .SetAspect(aspect);

    pangolin::Display("multi")
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0)
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(d_image);

    pangolin::GlTexture imageTexture(prophesee_width,
                                     prophesee_height + basler_camera.Height.GetValue(),
                                     GL_RGB, false, 0, GL_RGB, GL_UNSIGNED_BYTE);

    // Initialize container for depicting the image. This will contain the prophesee and the basler frames
    int rows = basler_camera.Height.GetValue() + prophesee_height;
    int cols = basler_camera.Width.GetValue();
    cv::Mat display(rows, cols, CV_8UC3, cv::Vec3b(0, 0, 0));
    cv::Mat prophesee_display(display,
                              cv::Rect(0, 0,
                                       prophesee_width,
                                       prophesee_height));
    cv::Mat basler_display(display,
                           cv::Rect(0, prophesee_height,
                                    basler_camera.Width.GetValue(),
                                    basler_camera.Height.GetValue()));

    while (!pangolin::ShouldQuit()) {

        // Get images from the two cameras
        basler_event_handler->get_display_frame(basler_display);
        cv::flip(basler_display, basler_display, 0);
        event_visualizer.get_display_frame(prophesee_display);
        cv::flip(prophesee_display, prophesee_display, 0);

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
                //i_events_stream->log_raw_data(out_file_path);
                is_recording = true;
                //std::cout << "Start recording to:\n" << out_file_path << std::endl;
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

        // Necessary
        pangolin::FinishFrame();
    }

    Pylon::PylonTerminate();
    return 0;
}
