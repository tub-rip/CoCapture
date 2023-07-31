#ifdef ENABLE_REALSENSE_SDK

#include "realsense_wrapper.h"

namespace rcg::wrappers::realsense {

    RealSenseWrapper::RealSenseWrapper(const char* serial_number, int id) :
        realsense_camera_   (GetRealSenseCamera(serial_number)),
        realsense_gui_      (realsense_camera_.get(), id),
        realsense_interface_(realsense_camera_.get(), id) {}

    RealSenseWrapper::~RealSenseWrapper() {}

    std::unique_ptr<cams::realsense::RealSenseCamera> RealSenseWrapper::GetRealSenseCamera(const char* serial_number) {
        rs2::device_list devices = cams::realsense::RealSenseCamera::GetContext().query_devices();
        for(rs2::device device : devices) {
            if(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) == std::string(serial_number)) {
                switch(cams::realsense::RealSenseCamera::GetProductLine(device)) {
                    case cams::realsense::PROD_LINE_L5:
                        return std::make_unique<cams::realsense::L5>(serial_number);
                }
            }
        }

        return {};
    }

    gui::ICameraGui& RealSenseWrapper::GetGui() {
        return realsense_gui_;
    }

    cam_interfaces::ICamera& RealSenseWrapper::GetInterface() {
        return realsense_interface_;
    }

} // rcg::wrappers::realsense

#endif //ENABLE_REALSENSE_SDK