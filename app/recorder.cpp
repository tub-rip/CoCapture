#include <opencv2/core.hpp>
#include "../lib/basler_event_handler.h"
#include "../lib/utils.h"

#include "../lib/Application/Gui.h"
#include "../lib/Application/CameraController.h"
#include "../lib/Application/Components/Viewpanel.h"
#include "../lib/Application/Components/Sidepanel.h"
#include "../lib/Application/Components/Recorder.h"

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

    // Components
    std::vector<Gui::Component*> components;

    // Viewpanel
    Gui::Viewpanel viewpanel = Gui::Viewpanel(0.75f, controller.getCamRefs());
    components.push_back(&viewpanel);

    // Sidepanel
    Gui::Sidepanel sidepanel = Gui::Sidepanel(controller.getCamRefs());
    components.push_back(&sidepanel);

    // Recorder
    Gui::Recorder recorder = Gui::Recorder(controller.getCamRefs());
    components.push_back(&recorder);

    bool done = false;
    while(!done) {
        gui.handleEvent(&done);

        gui.startFrame();

        controller.updateCameras();
        for(Gui::Component* c : components) {
            c->show();
        }

        gui.endFrameAndFinalizeRender();
    }

    // Cleanup
    controller.cleanupCameras();
    gui.cleanupGui();

    return 0;
}