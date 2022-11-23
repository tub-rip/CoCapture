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

#include "../lib/event_visualizer.h"
#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"
#include "../lib/camera/prophesee_cam.h"
#include "../lib/camera/basler_camera.h"


int main(int argc, const char *argv[]) {
    Pylon::PylonInitialize();
    Parameters app_parameter;
    utils::parse_comman_line(argc, argv, app_parameter);

    int display_height = 0;
    int display_width = -1;
    std::vector<camera::Base *> cameras;

    for (auto type: app_parameter.camera_types) {
        if (type == "prophesee") {
            auto params = utils::make_prophesee_params(app_parameter);
            cameras.push_back(new camera::PropheseeCam(params));
        } else if (type == "basler") {
            auto params = utils::make_basler_params(app_parameter);
            cameras.push_back(new camera::BaslerCamera(params));
        }
        auto cam = cameras.back();
        cam->setup_camera();

        if (cam->get_width() > display_width) {
            display_width = cam->get_width();
        }
        display_height += cam->get_height();
    }

    // Setup Prophesee Camera
    // PropheseeParams prophesee_params;
    // prophesee_params.set_rois = false; // TODO: propagate from main params
    // camera::PropheseeCam prophesee_cam(prophesee_params);
    // prophesee_cam.setup_camera();

    // Setup Basler Camera
    //
    // BaslerParams basler_params;
    // basler_params.do_warp = false; // TODO: propagate from main params
    // camera::BaslerCamera basler_cam(basler_params);
    // basler_cam.setup_camera();

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
    // auto bias_values = prophesee_camera.biases().get_facility()->get_all_biases();
    // pangolin::Var<int> bias_fo("ui.bias_fo", bias_values["bias_fo"], 45, 110);
    // pangolin::Var<int> bias_diff_off("ui.bias_diff_off", bias_values["bias_diff_off"], 25, 65);
    // pangolin::Var<int> bias_diff_on("ui.bias_diff_on", bias_values["bias_diff_on"], 95, 140);
    // pangolin::Var<int> bias_hpf("ui.bias_hpf", bias_values["bias_hpf"], 0, 120);
    // pangolin::Var<int> bias_refr("ui.bias_refr", bias_values["bias_refr"], 20, 100);
    // std::unordered_map<std::string, pangolin::Var<int> *> bias_setting{
    //         {"bias_fo",       &bias_fo},
    //         {"bias_diff_off", &bias_diff_off},
    //         {"bias_diff_on",  &bias_diff_on},
    //         {"bias_hpf",      &bias_hpf},
    //         {"bias_refr",     &bias_refr}
    // };

    //int current_snr = 0;
    //pangolin::Var<int> snr("ui.snr_proxy", current_snr);

    // TODO: at least for basler, set proper height and width in derived camera class


    // Image
    //int basler_display_height = app_parameter.do_warp ? prophesee_cam.get_height() :
    //                            basler_cam.get_height();
    //int display_height = app_parameter.overlay ?
    //                     prophesee_cam.get_height() : prophesee_cam.get_height() + basler_display_height;
    //int display_width = prophesee_cam.get_width();

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

    int offset_height = 0;
    for (auto cam: cameras) {
        int h = cam->get_height();
        int w = cam->get_width();
        cam->set_display(cv::Mat(display, cv::Rect(0, offset_height, w, h)));
        cam->set_offset(offset_height);
        offset_height += h;
    }

    while (!pangolin::ShouldQuit()) {
        // Get images from the two cameras
        // if (app_parameter.overlay) {
        //     basler_cam.get_display_frame(basler_display);
        //     prophesee_cam.get_display_frame(prophesee_display, basler_display);
        // } else {
        //     basler_cam.get_display_frame(basler_display);
        //     prophesee_cam.get_display_frame(prophesee_display);
        // }
        //cv::flip(basler_display, basler_display, 0);

        for (auto cam: cameras) {
            //auto cam_display = cam->get_display();
            //cam->get_display_frame(cam_display);
            cam->update_display_frame();
        }

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

        // if (exposure_time != current_exposure_time) {
        //     basler_camera.ExposureTime.SetValue(exposure_time);
        //     current_exposure_time = exposure_time;
        // }

        // for (const auto &it: bias_setting) {
        //     auto name = it.first;
        //     auto input_value = *(it.second);
        //     if (bias_values[name] != input_value) {
        //         prophesee_camera.biases().get_facility()->set(name, input_value);
        //     }
        // }

        // Necessary
        pangolin::FinishFrame();
    }

    Pylon::PylonTerminate();

    for (auto cam: cameras) {
        delete cam;
    }

    return 0;
}
