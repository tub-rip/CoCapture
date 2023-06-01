#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H
#define RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H

#include <thread>
#include <atomic>

#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>

namespace rcg::cams::realsense {

    enum FrameType {
        FRAME_COLOR,
        FRAME_DEPTH,
        FRAME_INFRARED
    };

    class RealSenseCamera {
    public:
        RealSenseCamera(const char* serial_number,
                        std::pair<int, int> color_frame_dimensions = std::pair<int,int> {1280, 720},
                        std::pair<int, int> depth_frame_dimensions = std::pair<int,int> {640, 480},
                        std::pair<int, int> infrared_frame_dimensions = std::pair<int,int> {640, 480},
                        int fps = 30);

        ~RealSenseCamera();

    public:
        void AcquireFrames();
        void OutputFrame(FrameType frame_type, cv::Mat& frame);
        std::pair<int, int> GetFrameDimensions(FrameType frame_type);
        bool StartRecording(const char* output_dir, bool save_color = true, bool save_depth = true, bool save_infrared = true);
        bool StopRecording();

    public:
        static void MetaDataToCSV(rs2::frame& frame, const char* filename);

    private:
        static rs2::context context_;
        rs2::config config_;
        rs2::pipeline pipeline_;

        int fps_;

        cv::Mat color_frame_;
        int color_frame_width_;
        int color_frame_height_;
        bool save_color_;

        cv::Mat depth_frame_;
        cv::Mat depth_frame_viz_;
        rs2::colorizer colorizer_;
        int depth_frame_width_;
        int depth_frame_height_;
        bool save_depth_;

        cv::Mat infrared_frame_;
        int infrared_frame_width_;
        int infrared_frame_height_;
        bool save_infrared_;

        std::mutex frame_mutex_;
        std::thread frame_acquisition_thread_;

        std::mutex recording_mutex_;
        bool is_recording_;
        int frame_sequence_;
        std::string output_dir_;

        std::atomic<bool> is_stopped_;
    };

} // rcg::cams::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H
#endif //ENABLE_REALSENSE_SDK