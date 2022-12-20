#include "../lib/CCGui.h"

int main(int argc, const char* argv[]) {
    CCGui::CCGui ccg = CCGui::CCGui(argc, argv);
    ccg.setupCameras();

    // Main loop
    bool done = false;
    while(!done) {
        ccg.handleEvent(&done);

        // Starts a new imgui frame
        ccg.startFrame();

        // - - - - - - APPLICATON LOGIC - - - - - - //

        ccg.updateCameras();
        ccg.displayCameras();

        // - - - - - - APPLICATON LOGIC - - - - - - //

        // Ends imgui frame and finalizes rendering
        ccg.render();
    }

    return 0;
}