#include <iostream>

#include <pangolin/var/var.h>
#include <pangolin/gl/gl.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/display/widgets.h>
#include <pangolin/display/default_font.h>
#include <pangolin/var/varextra.h>
#include <opencv2/core.hpp>
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

    // Set all but last prophesee camera to slave mode
    int last_prophesee_i = -1;
    for (int i = 0; i < app_parameter.camera_types.size(); ++i) {
        if (app_parameter.camera_types[i] == "prophesee") {
            last_prophesee_i = i;
        }
    }

    for (int i = 0; i < app_parameter.camera_types.size(); ++i) {
        const auto type = app_parameter.camera_types[i];

        if (type == "prophesee") {
            std::string mode = "slave";
            if (i == last_prophesee_i || !app_parameter.record) { mode = "master"; }
            cameras.push_back(new camera::PropheseeCam(PropheseeParams(app_parameter,
                                                                       mode, i)));
        } else if (type == "basler") {
            cameras.push_back(new camera::BaslerCamera(BaslerParams(app_parameter)));
        }
        auto cam = cameras.back();
        cam->setup_camera();

        if (cam->get_width() > display_width) {
            display_width = cam->get_width();
        }
        display_height += cam->get_height();
    }

    // Define GUI
    pangolin::CreateWindowAndBind("Main", 640, 480);

    const int UI_WIDTH = 20 * (int) pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> b_start_recording("ui.Start-Recording", false, false);
    pangolin::Var<bool> b_stop_recording("ui.Stop-Recording", false, false);
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

    // Image
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
        if (pangolin::Pushed(b_start_recording)) {

            if (!is_recording) {
                utils::start_recording(cameras);
            } else {
                std::cout << "Recording is running already" << std::endl;
            }
        }
        if (pangolin::Pushed(b_stop_recording)) {
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
