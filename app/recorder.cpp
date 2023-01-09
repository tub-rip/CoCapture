#include <iostream>

#include <opencv2/core.hpp>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "../lib/Gui/gui.h"
#include "../lib/Gui/Gui.h"

#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"
#include "../lib/camera/prophesee_cam.h"
#include "../lib/camera/basler_camera.h"

#include "../lib/Gui/CameraController.h"

int main(int argc, const char* argv[]) {
    Parameters app_parameter;
    utils::parse_comman_line(argc, argv, app_parameter);

    // Setup GUI
    Gui::Gui gui = Gui::Gui();
    gui.setupGui();

    // Setup cameras
    Gui::CameraController controller = Gui::CameraController();
    controller.setupController(app_parameter);
    controller.setupCameras();

    GLuint* textures = new GLuint[controller.getNumCams()];
    for(int i = 0; i < controller.getNumCams(); i++) {
        auto cam = controller.getCams()[i];
        gui.setup_texture_cam(textures + i, cam.getWidth(), cam.getHeight());
    }

    std::vector<camera::Base*> cameras;
    for(auto cam : controller.getCams()) {
        cameras.push_back(cam.getActual());
    }

    bool done = false;
    while(!done) {
        gui.handleEvent(&done);

        gui.startFrame();

        for(int i = 0; i < controller.getNumCams(); i++) {
            controller.updateCameras();

            auto cam = controller.getCams()[i];

            if(!cam.getDisplay().empty()) {
                gui.update_texture_cam(textures[i], (void*) cam.getDisplay().data,
                                     cam.getWidth(), cam.getHeight());
            }

            gui.display_cams(textures, cameras);
        }

        gui.endFrameAndFinalizeRender();
    }

    // Cleanup
    controller.cleanupCameras();
    gui.cleanupGui();

    delete[] textures;

    return 0;
}