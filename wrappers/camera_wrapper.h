#ifndef RIP_COCAPTURE_GUI_CAMERA_WRAPPER_H
#define RIP_COCAPTURE_GUI_CAMERA_WRAPPER_H

#include "camera_gui.h"
#include "cam_interface.h"

namespace rcg::wrappers {

    class ICameraWrapper {
    public:
        virtual gui::ICameraGui& GetGui() = 0;
        virtual cam_interfaces::ICamera& GetInterface() = 0;
    };

} // rcg::wrappers

#endif //RIP_COCAPTURE_GUI_CAMERA_WRAPPER_H