#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_WRAPPER_H
#define RIP_COCAPTURE_GUI_REALSENSE_WRAPPER_H

#include "camera_wrapper.h"
#include "realsense_gui.h"
#include "realsense_interface.h"

namespace rcg::wrappers::realsense {

    class RealSenseWrapper : public ICameraWrapper {
    public:
        RealSenseWrapper(const char* serial_number, int id);
        ~RealSenseWrapper();

    private:
        static std::unique_ptr<cams::realsense::RealSenseCamera> GetRealSenseCamera(const char* serial_number);

    public:
        gui::ICameraGui& GetGui() override;
        cam_interfaces::ICamera& GetInterface() override;

    private:
        std::unique_ptr<cams::realsense::RealSenseCamera> realsense_camera_;
        gui::realsense::RealSenseGui realsense_gui_;
        cam_interfaces::realsense::RealSenseInterface realsense_interface_;
    };

} // rcg::wrappers::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_WRAPPER_H
#endif //ENABLE_REALSENSE_SDK