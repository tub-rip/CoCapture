#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_INTERFACE_H
#define RIP_COCAPTURE_GUI_REALSENSE_INTERFACE_H

#include "cam_interface.h"
#include "realsense_camera.h"
#include "l5.h"

namespace rcg::cam_interfaces::realsense {

    class RealSenseInterface : public ICamera {
    public:
        RealSenseInterface(cams::realsense::RealSenseCamera* realsense_camera, int id);
        ~RealSenseInterface();

    public:
        const char* GetIdentifier() override;

        bool Start() override;
        bool Stop() override;

        bool StartRecording(const char* output_dir) override;
        bool StopRecording() override;

        bool IsStarted() override;
        bool IsRecording() override;

    private:
        cams::realsense::RealSenseCamera* realsense_camera_;
        std::string identifier_;
        std::string output_dir_;
    };

} // rcg::cam_interfaces::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_INTERFACE_H
#endif //ENABLE_REALSENSE_SDK