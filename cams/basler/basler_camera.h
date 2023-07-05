#ifdef ENABLE_PYLON_SDK
#ifndef RIP_COCAPTURE_GUI_BASLER_CAMERA_H
#define RIP_COCAPTURE_GUI_BASLER_CAMERA_H

#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "image_event_handler.h"

namespace rcg::cams::basler {

    class BaslerCamera {
    public:
        BaslerCamera(const char* serial_number);
        ~BaslerCamera();

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
        Pylon::CBaslerUniversalInstantCamera camera_;
        ImageEventHandler* image_event_handler_;

        bool is_started_;
        bool is_recording_;
    };

} // rcg::cams::basler

#endif //RIP_COCAPTURE_GUI_BASLER_CAMERA_H
#endif //ENABLE_PYLON_SDK