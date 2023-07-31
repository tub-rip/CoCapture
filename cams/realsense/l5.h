#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_L5_H
#define RIP_COCAPTURE_GUI_L5_H

#include <thread>

#include "realsense_camera.h"

namespace rcg::cams::realsense {

    class L5 : public RealSenseCamera {
    public:
        L5(const char* serial_number);
        ~L5();

    private:
        std::vector<rs2::stream_profile> GetPrefStreamProfiles(rs2::sensor& sensor);

    public:
        void OutputIRFrame(cv::Mat& ir_frame);

    public:
        rs2::device& GetDevice() override;
        void Stream() override;
        bool IsStarted() override;
        bool Start() override;
        bool Stop() override;
        void OutputFrame(cv::Mat& frame) override;
        std::pair<int, int> GetOutputFrameDimensions() override;
        bool IsRecording() override;
        bool StartRecording(const char* output_dir) override;
        bool StopRecording() override;

    private:
        rs2::device device_;
        rs2::sensor depth_sensor_;
        rs2::sensor color_sensor_;
        rs2::syncer syncer_;
        rs2::colorizer colorizer_;

        std::thread sensors_thread_;

        cv::Mat ir_frame_;
        std::mutex ir_frame_mutex_;

        bool is_started_;
        std::mutex is_started_mutex_;
        bool is_recording_;
        std::mutex is_recording_mutex_;

        std::string output_dir_;
        int frame_sequence_;
    };

} // rcg::cams::realsense

#endif //RIP_COCAPTURE_GUI_L5_H
#endif //ENABLE_REALSENSE_SDK