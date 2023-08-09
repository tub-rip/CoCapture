#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_PROPHESEE_CAMERA_H
#define RIP_COCAPTURE_GUI_PROPHESEE_CAMERA_H

#include <fstream>

#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <metavision/hal/facilities/i_ll_biases.h>
#include <metavision/hal/facilities/i_hw_identification.h>
#include <metavision/hal/facilities/i_camera_synchronization.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/hal/device/device_discovery.h>

#include "trigger_event_saver.h"
#include "event_signal_processor.h"

namespace rcg::cams::prophesee {

    const char* const MODE_STANDALONE = "Standalone";
    const char* const MODE_MASTER = "Master";
    const char* const MODE_SLAVE = "Slave";

    const char* const BIAS_DIFF_OFF = "bias_diff_off";
    const char* const BIAS_DIFF_ON = "bias_diff_on";
    const char* const BIAS_FO = "bias_fo";
    const char* const BIAS_HPF = "bias_hpf";
    const char* const BIAS_REFR = "bias_refr";

    class PropheseeCamera {
    public:
        PropheseeCamera(const char* serial_number, int display_accumulation_time_us = 10000,
                        cv::Scalar background_color = {255, 255, 255},
                        cv::Scalar on_color = {0, 0, 255},
                        cv::Scalar off_color = {255, 0, 0});

    public:
        std::string GetSerialNumber();
        std::string GetGeneration();
        std::string GetPluginName();
        int GetCDFrameWidth();
        int GetCDFrameHeight();
        int GetBiasValue(const char* bias_name);
        bool SetBiasValue(const char* bias_name, int bias_value);
        std::map<std::string, std::pair<int, int>> GetBiasRanges();
        std::string GetMode();
        bool SetMode(const char* mode);
        EventSignalProcessor* GetEventSignalProcessor();

    public:
        bool IsStarted() {
            return is_started_;
        }

        bool IsRecording() {
            return is_recording_;
        }

    public:
        bool Start(int fps = 30);
        bool Stop();
        void OutputCDFrame(cv::Mat& cd_frame);
        bool EnableTriggerIn();
        bool DisableTriggerIn();
        bool StartRecording(const char* output_dir);
        bool StopRecording();
        void AnalyzeRecording(const char* output_dir);

    public:
        static std::vector<std::string> ListConnectedCameras();

    private:
        Metavision::Camera camera_;
        std::unique_ptr<Metavision::CDFrameGenerator> cd_frame_generator_;
        std::unique_ptr<EventSignalProcessor> event_signal_processor_;
        cv::Mat cd_frame_;
        std::mutex cd_frame_mutex_;
        TriggerEventSaver trigger_event_saver_;
        bool is_started_;
        bool is_recording_;
        long start_recording_ts_;
        long stop_recording_ts_;
        Metavision::I_LL_Biases* biases_;
        Metavision::I_HW_Identification* hw_identification_;
        Metavision::I_CameraSynchronization* camera_synchronization_;
        Metavision::I_TriggerIn* trigger_in_;
    };

} // rcg::cams::prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_CAMERA_H
#endif //ENABLE_METAVISION_SDK