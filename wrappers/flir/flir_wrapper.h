#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_BASLER_WRAPPER_H
#define RIP_COCAPTURE_GUI_BASLER_WRAPPER_H

#include "camera_wrapper.h"
#include "flir_gui.h"
#include "flir_interface.h"

namespace rcg::wrappers::flir {

    class FlirWrapper : public ICameraWrapper {
    public:
        FlirWrapper(const char* serial_number, int id);
        ~FlirWrapper();

    public:
        gui::ICameraGui& GetGui() override;
        cam_interfaces::ICamera& GetInterface() override;

    private:
        std::unique_ptr<cams::flir::FlirCamera> flir_camera_;
        gui::flir::FlirGui flir_gui_;
        cam_interfaces::flir::FlirInterface flir_interface_;
    };

} // rcg::wrappers::flir

#endif //RIP_COCAPTURE_GUI_BASLER_WRAPPER_H
#endif //ENABLE_SPINNAKER_SDK