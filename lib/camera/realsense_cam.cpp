#include "realsense_cam.h"

namespace camera {

    void RealSenseCam::setup_camera() {
        // Fetch a single frame to initialize frame width and height
        rs2::frameset data = pipe_.wait_for_frames();
        rs2::frame depth = data.get_depth_frame();
        width_ = depth.as<rs2::video_frame>().get_width();
        height_ = depth.as<rs2::video_frame>().get_height();
    }

    void RealSenseCam::get_display_frame(cv::Mat &display) {
        rs2::frameset data = pipe_.wait_for_frames();
        rs2::frame depth = data.get_depth_frame().apply_filter(color_map_);

        display = cv::Mat(cv::Size(width_, height_),
                          CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);
    }

    void RealSenseCam::start_recording_to_path(std::string path) {}
    void RealSenseCam::stop_recording() {}

}
