#include <iostream>

#include <opencv2/core.hpp>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "../lib/gui.h"

#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"
#include "../lib/camera/prophesee_cam.h"
#include "../lib/camera/basler_camera.h"

int main(int argc, const char* argv[]) {
    // Setup GUI
    gui::gui g = gui::gui();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    gui::window_settings ws = {
            "Recorder",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            display_mode.w, display_mode.h,
            window_flags
    };

    gui::color_settings cs = {
            ImVec4(0x22/255.0f, 0x63/255.0f, 0x87/255.0f, 1.0f)
    };

    g.create_window(ws, cs);

    // Setup cameras
    Pylon::PylonInitialize();

    Parameters app_parameter;
    utils::parse_comman_line(argc, argv, app_parameter);
    int n_cams = app_parameter.camera_types.size();

    std::vector<camera::Base *> cameras;
    std::vector<cv::Mat> mats;
    GLuint* textures = new GLuint[n_cams];

    // Set all but last prophesee camera to slave mode
    int last_prophesee_i = -1;
    for (int i = 0; i < n_cams; ++i) {
        if (app_parameter.camera_types[i] == "prophesee") {
            last_prophesee_i = i;
        }
    }

    // Setup cameras
    for (int i = 0; i < n_cams; ++i) {
        const auto type = app_parameter.camera_types[i];
        if (type == "prophesee") {
            std::string mode = "slave";
            if (i == last_prophesee_i || !app_parameter.record) { mode = "master"; }
            cameras.push_back(new camera::PropheseeCam(PropheseeParams(app_parameter, mode, i)));
        } else if (type == "basler") {
            cameras.push_back(new camera::BaslerCamera(BaslerParams(app_parameter)));
        }

        auto cam = cameras.back();
        cam->setup_camera();

        mats.push_back(cv::Mat(cam->get_height(), cam->get_width(),
                               CV_8UC3, cv::Vec3b(0,0,0)));

        auto mat = mats.back();
        cam->set_display(mat);

        g.setup_texture_cam(textures + i, cam->get_width(), cam->get_height());
    }

    // Main loop
    bool done = false;
    while(!done) {
        g.handle_event(&done);

        g.start_frame();

        for(int i = 0; i < n_cams; i++) {
            cameras[i]->update_display_frame();

            if(!mats[i].empty()) {
                g.update_texture_cam(textures[i], (void*) mats[i].data,
                                     cameras[i]->get_width(), cameras[i]->get_height());
            }

            g.display_cams(textures, cameras);
        }

        g.render();
    }

    // Cleanup
    for(auto cam : cameras) {
        delete cam;
    }

    delete[] textures;

    return 0;
}