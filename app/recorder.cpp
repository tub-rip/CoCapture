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

    std::vector<camera::Base *> cameras;

    int n_cameras = app_parameter.camera_types.size();
    GLuint textures[n_cameras];
    int      widths[n_cameras];
    int     heights[n_cameras];
    cv::Mat    mats[n_cameras];

    // Set all but last prophesee camera to slave mode
    int last_prophesee_i = -1;
    for (int i = 0; i < n_cameras; ++i) {
        if (app_parameter.camera_types[i] == "prophesee") {
            last_prophesee_i = i;
        }
    }

    for (int i = 0; i < n_cameras; ++i) {
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

        widths[i] = cam->get_width();
        heights[i] = cam->get_height();

        mats[i] = cv::Mat(heights[i], widths[i],
                          CV_8UC3, cv::Vec3b(0,0,0));
        cam->set_display(mats[i]);

        g.setup_texture_cam(textures + i,
                            widths[i], heights[i]);
    }

    // Main loop
    bool done = false;
    while(!done) {
        g.handle_event(&done);

        g.start_frame();

        for(int i = 0; i < n_cameras; i++) {
            cameras[i]->update_display_frame();

            if(!mats[i].empty()) {
                cv::flip(mats[i], mats[i], 0);

                g.update_texture_cam(textures[i], (void*) mats[i].data,
                                     widths[i], heights[i]);
            }

            g.display_cams(textures, cameras);
        }

        g.render();
    }

    // Cleanup
    for(auto cam : cameras) {
        delete cam;
    }

    return 0;
}