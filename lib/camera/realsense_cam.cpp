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
//        rs2::frame depth = data.get_depth_frame().apply_filter(color_map_);
        rs2::frame depth = data.get_depth_frame();
        rs2::frame color = data.get_color_frame();
        rs2::frame infra = data.get_infrared_frame();

        display = cv::Mat(cv::Size(width_, height_),
                          CV_8UC3, (void*)(depth.apply_filter(color_map_).get_data()), cv::Mat::AUTO_STEP);

        if(record_) {
            std::string frame_postfix = "_" + std::to_string(seq_) + ".png";
            std::string csv_postfix = "_" + std::to_string(seq_) + "_metadata.csv";

            cv::Mat display_depth = cv::Mat(cv::Size(width_, height_),
                                            CV_16UC1, (void*)depth.get_data(), cv::Mat::AUTO_STEP);
            cv::imwrite(path_ + frame_postfix, display_depth);
            metadata_to_csv(depth, path_+csv_postfix);
            if (save_color){
                cv::Mat display_color = cv::Mat(cv::Size(color.as<rs2::video_frame>().get_width(), color.as<rs2::video_frame>().get_height()),
                                                CV_8UC3, (void*)color.get_data(), cv::Mat::AUTO_STEP);
                cv::imwrite(path_ + "_rgb" + frame_postfix, display_color);
                metadata_to_csv(color, path_ + "_rgb" + csv_postfix);
              }
            if (save_ir) {
                cv::Mat display_infra = cv::Mat(cv::Size(infra.as<rs2::video_frame>().get_width(), infra.as<rs2::video_frame>().get_height()),
                                                CV_8UC1, (void*)infra.get_data(), cv::Mat::AUTO_STEP);
                cv::imwrite(path_ + "_infra" + frame_postfix, display_infra);
                metadata_to_csv(infra, path_ + "_infra" + csv_postfix);
              }
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
