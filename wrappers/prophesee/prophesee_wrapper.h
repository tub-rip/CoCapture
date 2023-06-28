#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_PROPHESEE_WRAPPER_H
#define RIP_COCAPTURE_GUI_PROPHESEE_WRAPPER_H

#include "camera_wrapper.h"
#include "prophesee_gui.h"
#include "prophesee_interface.h"

namespace rcg::wrappers::prophesee {

    class PropheseeWrapper : public ICameraWrapper {
    public:
        PropheseeWrapper(const char* serial_number, int id);
        ~PropheseeWrapper();

    public:
        gui::ICameraGui& GetGui() override;
        cam_interfaces::ICamera& GetInterface() override;

    private:
        std::unique_ptr<cams::prophesee::PropheseeCamera> prophesee_camera_;
        gui::prophesee::PropheseeGui prophesee_gui_;
        cam_interfaces::prophesee::PropheseeInterface prophesee_interface_;
    };

} // rcg::wrappers::prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_WRAPPER_H
#endif //ENABLE_METAVISION_SDK