#ifdef ENABLE_PYLON_SDK
#ifndef RIP_COCAPTURE_GUI_BASLER_WRAPPER_H
#define RIP_COCAPTURE_GUI_BASLER_WRAPPER_H

#include "camera_wrapper.h"
#include "basler_gui.h"
#include "basler_interface.h"

namespace rcg::wrappers::basler {

    class BaslerWrapper : public ICameraWrapper {
    public:
        BaslerWrapper(const char* serial_number, int id);
        ~BaslerWrapper();

    public:
        gui::ICameraGui& GetGui() override;
        cam_interfaces::ICamera& GetInterface() override;

    private:
        std::unique_ptr<cams::basler::BaslerCamera> basler_camera_;
        gui::basler::BaslerGui basler_gui_;
        cam_interfaces::basler::BaslerInterface basler_interface_;
    };

} // rcg::wrappers::basler

#endif //RIP_COCAPTURE_GUI_BASLER_WRAPPER_H
#endif //ENABLE_PYLON_SDK