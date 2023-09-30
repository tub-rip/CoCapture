#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_CAMERA_H
#define RIP_COCAPTURE_GUI_FLIR_CAMERA_H

//#include <pylon/PylonIncludes.h>
//#include <pylon/FlirUniversalInstantCamera.h>
#include <opencv2/opencv.hpp>

namespace rcg::cams::flir {

    class FlirCamera {
    public:
        FlirCamera(const char* serial_number);
        ~FlirCamera();

    public:
        std::string GetSerialNumber();
        std::string GetModelName();
        std::string GetVendorName();
        int GetImageFrameWidth();
        int GetImageFrameHeight();
        int GetExposureTime();
        bool SetExposureTime(int exposure_time_us);
        bool GetTriggerMode();
        bool SetTriggerMode(bool trigger_mode);
        bool GetReverseX();
        bool SetReverseX(bool reverse_x);

    public:
        bool IsStarted() {
            return is_started_;
        }

        bool IsRecording() {
            return is_recording_;
        }

    public:
        bool Start();
        bool Stop();
        void OutputFrame(cv::Mat& image_frame);
        bool StartRecording(const char* output_dir);
        bool StopRecording();

    public:
        static void AnalyzeRecording(const char* output_dir);
        static std::vector<std::string> ListConnectedCameras();

    private:
        //Pylon::CFlirUniversalInstantCamera camera_;
        //ImageEventHandler* image_event_handler_;

        bool is_started_;
        bool is_recording_;
    };

} // rcg::cams::flir

#endif //RIP_COCAPTURE_GUI_FLIR_CAMERA_H
#endif //ENABLE_SPINNAKER_SDK