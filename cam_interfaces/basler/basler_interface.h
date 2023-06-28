#ifdef ENABLE_PYLON_SDK
#ifndef RIP_COCAPTURE_GUI_BASLER_INTERFACE_H
#define RIP_COCAPTURE_GUI_BASLER_INTERFACE_H

#include "cam_interface.h"
#include "basler_camera.h"

namespace rcg::cam_interfaces::basler {

    class BaslerInterface : public ICamera {
    public:
        BaslerInterface(cams::basler::BaslerCamera* basler_camera, int id);
        ~BaslerInterface();

    public:
        const char* GetIdentifier() override;

        bool Start() override;
        bool Stop() override;

        bool StartRecording(const char* output_dir) override;
        bool StopRecording() override;

        bool IsStarted() override;
        bool IsRecording() override;

    private:
        cams::basler::BaslerCamera* basler_camera_;
        std::string identifier_;
        std::string output_dir_;
    };

} // rcg::cam_interfaces::basler

#endif //RIP_COCAPTURE_GUI_BASLER_INTERFACE_H
#endif //ENABLE_PYLON_SDK