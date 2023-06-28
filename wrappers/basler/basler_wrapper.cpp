#ifdef ENABLE_PYLON_SDK

#include "basler_wrapper.h"

namespace rcg::wrappers::basler {

    BaslerWrapper::BaslerWrapper(const char* serial_number, int id) :
        basler_camera_(std::make_unique<cams::basler::BaslerCamera>(serial_number)),
        basler_gui_(basler_camera_.get(), id),
        basler_interface_(basler_camera_.get(), id) {}

    BaslerWrapper::~BaslerWrapper() {}

    gui::ICameraGui& BaslerWrapper::GetGui() {
        return basler_gui_;
    }

    cam_interfaces::ICamera& BaslerWrapper::GetInterface() {
        return basler_interface_;
    }

} // rcg::wrappers::basler

#endif //ENABLE_PYLON_SDK