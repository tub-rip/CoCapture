#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_INTERFACE_H
#define RIP_COCAPTURE_GUI_FLIR_INTERFACE_H

#include "cam_interface.h"


namespace rcg::cam_interfaces::flir {

    class FlirInterface : public ICamera {
    public:
        //FlirInterface(cams::flir::FlirCamera* flir_camera, int id);
        ~FlirInterface();

    public:
        const char* GetIdentifier() override;

        bool Start() override;
        bool Stop() override;

        bool StartRecording(const char* output_dir) override;
        bool StopRecording() override;

        bool IsStarted() override;
        bool IsRecording() override;

    private:
        //cams::flir::FlirCamera* flir_camera_;
        std::string identifier_;
        std::string output_dir_;
    };

} // rcg::cam_interfaces::flir

#endif //RIP_COCAPTURE_GUI_FLIR_INTERFACE_H
#endif //ENABLE_SPINNAKER_SDK
