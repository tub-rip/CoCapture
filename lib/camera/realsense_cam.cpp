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

        if(record_) {
            std::string frame_postfix = "_" + std::to_string(seq_) + ".png";
            std::string csv_postfix = "_" + std::to_string(seq_) + "_metadata.csv";

            cv::imwrite(path_ + frame_postfix, display);
            metadata_to_csv(depth, path_+csv_postfix);

            seq_++;
        }
    }

    void RealSenseCam::start_recording_to_path(std::string path) {
        record_ = true;
        seq_ = 0;
        path_ = path;
    }

    void RealSenseCam::stop_recording() {
        record_ = false;
        seq_ = 0;
        path_ = "";
    }

    // Example snippet from the RealSense API
    void RealSenseCam::metadata_to_csv(const rs2::frame &frm, const std::string &filename) {
        std::ofstream csv;

        csv.open(filename);

        //    std::cout << "Writing metadata to " << filename << endl;
        csv << "Stream," << rs2_stream_to_string(frm.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

        // Record all the available metadata attributes
        for (size_t i = 0; i < RS2_FRAME_METADATA_COUNT; i++)
        {
            if (frm.supports_frame_metadata((rs2_frame_metadata_value)i))
            {
                csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value)i) << ","
                    << frm.get_frame_metadata((rs2_frame_metadata_value)i) << "\n";
            }
        }

        csv.close();
    }

}
