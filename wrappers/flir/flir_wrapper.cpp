#ifdef ENABLE_SPINNAKER_SDK

#include "flir_wrapper.h"

namespace rcg::wrappers::flir {

    FlirWrapper::FlirWrapper(const char* serial_number, int id) :
        flir_camera_(std::make_unique<cams::flir::FlirCamera>(serial_number)),
        flir_gui_(flir_camera_.get(), id),
        flir_interface_(flir_camera_.get(), id) {}

    FlirWrapper::~FlirWrapper() {}

    gui::ICameraGui& FlirWrapper::GetGui() {
        return flir_gui_;
    }

    cam_interfaces::ICamera& FlirWrapper::GetInterface() {
        return flir_interface_;
    }

} // rcg::wrappers::flir

#endif //ENABLE_SPINNAKER_SDK