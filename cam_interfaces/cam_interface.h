#ifndef RIP_COCAPTURE_GUI_CAM_INTERFACE_H
#define RIP_COCAPTURE_GUI_CAM_INTERFACE_H

namespace rcg::cam_interfaces {

    class ICamera {
    public:
        ICamera() {}
        ~ICamera() {}

    public:
        virtual const char* GetIdentifier() { return {}; }

        virtual bool Start() { return {}; }
        virtual bool Stop() { return {}; }

        virtual bool StartRecording(const char* output_dir) { return {}; }
        virtual bool StopRecording() { return {}; }

        virtual bool IsStarted() { return {}; }
        virtual bool IsRecording() { return {}; }
    };

} // rcg::cam_interfaces

#endif //RIP_COCAPTURE_GUI_CAM_INTERFACE_H