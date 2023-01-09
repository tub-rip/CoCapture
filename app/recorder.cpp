#include <iostream>

#include <opencv2/core.hpp>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "../lib/Gui/gui.h"

#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"
#include "../lib/camera/prophesee_cam.h"
#include "../lib/camera/basler_camera.h"

#include "../lib/Gui/CameraController.h"

int main(int argc, const char* argv[]) {
    Parameters app_parameter;
    utils::parse_comman_line(argc, argv, app_parameter);

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
    Gui::CameraController controller = Gui::CameraController();
    controller.setupController(app_parameter);

    GLuint* textures = new GLuint[controller.getNumCams()];

    controller.setupCameras();

    std::vector<camera::Base*> cameras;
    for(auto cam : controller.getCams()) {
        cameras.push_back(cam.getCam());
    }

    for(int i = 0; i < controller.getNumCams(); i++) {
        auto cam = controller.getCams()[i].getCam();
        g.setup_texture_cam(textures + i, cam->get_width(), cam->get_height());
    }

    bool done = false;
    while(!done) {
        g.handle_event(&done);

        g.start_frame();

        for(int i = 0; i < controller.getNumCams(); i++) {
            controller.updateCameras();

            auto cam = controller.getCams()[i];

            if(!cam.getDisplay().empty()) {
                g.update_texture_cam(textures[i], (void*) cam.getDisplay().data,
                                     cam.getWidth(), cam.getHeight());
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