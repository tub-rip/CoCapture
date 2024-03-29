#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_CAMERA_H
#define RIP_COCAPTURE_GUI_FLIR_CAMERA_H

#include <Spinnaker.h>
#include <opencv2/opencv.hpp>

#include "image_event_handler.h"

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
        int GetMaxExposureTime();
        bool SetExposureTime(int exposure_time_us);
        bool GetTriggerMode();
        bool SetTriggerMode(bool trigger_mode);

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
        void AnalyzeRecording(const char* output_dir);

    public:
        static std::vector<std::string> ListConnectedCameras();

    private:
        Spinnaker::SystemPtr system_;
        Spinnaker::CameraPtr camera_;

        bool is_started_;
        bool is_recording_;

        int height_;
        int width_;

        ImageEventHandlerBase* image_handler_;
        std::mutex init_mutex_;
    };

} // rcg::cams::flir

#endif //RIP_COCAPTURE_GUI_FLIR_CAMERA_H
#endif //ENABLE_SPINNAKER_SDK