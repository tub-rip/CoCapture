#ifdef ENABLE_METAVISION_SDK

#include "prophesee_wrapper.h"

namespace rcg::wrappers::prophesee {

    PropheseeWrapper::PropheseeWrapper(const char* serial_number, int id) :
        prophesee_camera_(std::make_unique<cams::prophesee::PropheseeCamera>(serial_number)),
        prophesee_gui_(prophesee_camera_.get(), id),
        prophesee_interface_(prophesee_camera_.get(), id) {}

    PropheseeWrapper::~PropheseeWrapper() {}

    gui::ICameraGui& PropheseeWrapper::GetGui() {
        return prophesee_gui_;
    }

    cam_interfaces::ICamera& PropheseeWrapper::GetInterface() {
        return prophesee_interface_;
    }

} // rcg::wrappers::prophesee

#endif //ENABLE_METAVISION_SDK