#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
#define RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H

#include <boost/filesystem.hpp>

#include "cam_interface.h"
#include "prophesee_camera.h"

namespace rcg::cam_interfaces::prophesee {

    class PropheseeInterface : public ICamera {
    public:
        PropheseeInterface(cams::prophesee::PropheseeCamera* prophesee_camera, int id);
        ~PropheseeInterface();

    public:
        const char* GetIdentifier() override;

        bool Start() override;
        bool Stop() override;

        bool StartRecording(const char* output_dir) override;
        bool StopRecording() override;

        bool IsStarted() override;
        bool IsRecording() override;

    private:
        cams::prophesee::PropheseeCamera* prophesee_camera_;
        std::string identifier_;
        std::string output_dir_;
    };

} // rcg::cam_interfaces::prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
#endif //ENABLE_METAVISION_SDK