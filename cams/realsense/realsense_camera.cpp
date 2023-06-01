#ifdef ENABLE_REALSENSE_SDK

#include "realsense_camera.h"

namespace rcg::cams::realsense {

    RealSenseCamera::RealSenseCamera(const char* serial_number,
                                     std::pair<int, int> color_frame_dimensions,
                                     std::pair<int, int> depth_frame_dimensions,
                                     std::pair<int, int> infrared_frame_dimensions,
                                     int fps) :

    fps_                   (fps),

    color_frame_width_     (color_frame_dimensions.first),
    color_frame_height_    (color_frame_dimensions.second),
    save_color_            (false),

    depth_frame_width_     (depth_frame_dimensions.first),
    depth_frame_height_    (depth_frame_dimensions.second),
    save_depth_            (false),

    infrared_frame_width_  (infrared_frame_dimensions.first),
    infrared_frame_height_ (infrared_frame_dimensions.second),
    save_infrared_         (false),

    is_recording_          (false),
    frame_sequence_        (0),

    is_stopped_            (false)

    {
        config_.enable_device(serial_number);
        config_.enable_stream(RS2_STREAM_COLOR, color_frame_width_, color_frame_height_, RS2_FORMAT_RGB8, fps_);
        config_.enable_stream(RS2_STREAM_DEPTH, depth_frame_width_, depth_frame_height_, RS2_FORMAT_Z16, fps_);
        config_.enable_stream(RS2_STREAM_INFRARED, infrared_frame_width_, infrared_frame_height_, RS2_FORMAT_Y8, fps_);

        pipeline_.start(config_);

        frame_acquisition_thread_ = std::thread([&] () {
            while(!is_stopped_.load(std::memory_order_acquire)) {
                AcquireFrames();
            }
        });
    }

    RealSenseCamera::~RealSenseCamera() {
        is_stopped_.store(true, std::memory_order_release);
        frame_acquisition_thread_.join();
    }

    void RealSenseCamera::AcquireFrames() {
        rs2::frameset data = pipeline_.wait_for_frames();

        rs2::frame color_frame = data.get_color_frame();
        rs2::frame depth_frame = data.get_depth_frame();
        rs2::frame infrared_frame = data.get_infrared_frame();

        frame_mutex_.lock();

        color_frame_ = cv::Mat {cv::Size {color_frame_width_, color_frame_height_}, CV_8UC3, (void*) color_frame.get_data(), cv::Mat::AUTO_STEP};
        depth_frame_ = cv::Mat {cv::Size {depth_frame_width_, depth_frame_height_}, CV_16UC1, (void*) depth_frame.get_data(), cv::Mat::AUTO_STEP};
        depth_frame_viz_ = cv::Mat {cv::Size {depth_frame_width_, depth_frame_height_}, CV_8UC3, (void*) depth_frame.apply_filter(colorizer_).get_data(), cv::Mat::AUTO_STEP};
        infrared_frame_ = cv::Mat {cv::Size {infrared_frame_width_, infrared_frame_height_}, CV_8UC1, (void*) infrared_frame.get_data(), cv::Mat::AUTO_STEP};

        recording_mutex_.lock();
        if(is_recording_) {
            std::string frame_postfix = "_" + std::to_string(frame_sequence_) + ".png";
            std::string csv_postfix = "_" + std::to_string(frame_sequence_) + ".csv";

            if(save_color_) {
                cv::imwrite(output_dir_ + "/frame_color" + frame_postfix, color_frame_);
                MetaDataToCSV(color_frame, (output_dir_ + "/frame_color" + csv_postfix).c_str());
            }

            if(save_depth_) {
                cv::imwrite(output_dir_ + "/frame_depth" + frame_postfix, depth_frame_);
                MetaDataToCSV(depth_frame, (output_dir_ + "/frame_depth" + csv_postfix).c_str());
            }

            if(save_infrared_) {
                cv::imwrite(output_dir_ + "/frame_ir" + frame_postfix, infrared_frame_);
                MetaDataToCSV(infrared_frame, (output_dir_ + "/frame_ir" + csv_postfix).c_str());
            }

            ++frame_sequence_;
        }
        recording_mutex_.unlock();

        frame_mutex_.unlock();
    }

    void RealSenseCamera::OutputFrame(FrameType frame_type, cv::Mat &frame) {
        frame_mutex_.lock();

        switch(frame_type) {
            case FRAME_COLOR:
                frame = color_frame_;
                break;

            case FRAME_DEPTH:
                frame = depth_frame_viz_;
                break;

            case FRAME_INFRARED:
                if(!infrared_frame_.empty()) {
                    cv::cvtColor(infrared_frame_, frame, cv::COLOR_GRAY2RGB);
                }
        }

        frame_mutex_.unlock();
    }

    std::pair<int, int> RealSenseCamera::GetFrameDimensions(FrameType frame_type) {
        switch(frame_type) {
            case FRAME_COLOR:
                return {color_frame_width_, color_frame_height_};

            case FRAME_DEPTH:
                return {depth_frame_width_, depth_frame_height_};

            case FRAME_INFRARED:
                return {infrared_frame_width_, infrared_frame_height_};
        }

        return {};
    }

    bool RealSenseCamera::StartRecording(const char* output_dir, bool save_color, bool save_depth, bool save_infrared) {
        recording_mutex_.lock();
        if(is_recording_) {
            recording_mutex_.unlock();
            return false;
        }

        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

        frame_sequence_ = 0;

        save_color_ = save_color;
        save_depth_ = save_depth;
        save_infrared_ = save_infrared;

        is_recording_ = true;
        recording_mutex_.unlock();
        return true;
    }

    bool RealSenseCamera::StopRecording() {
        recording_mutex_.lock();
        if(!is_recording_) {
            recording_mutex_.unlock();
            return false;
        }

        is_recording_ = false;
        recording_mutex_.unlock();
        return true;
    }

    void RealSenseCamera::MetaDataToCSV(rs2::frame& frame, const char* csv_file) {
        std::fstream metadata_csv;
        metadata_csv.open(csv_file, std::ios::out);

        metadata_csv << "Stream," << rs2_stream_to_string(frame.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

        for(int i = 0; i < RS2_FRAME_METADATA_COUNT; ++i) {
            if(frame.supports_frame_metadata((rs2_frame_metadata_value) i)) {
                metadata_csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value) i) << ","
                             << frame.get_frame_metadata((rs2_frame_metadata_value) i) << "\n";
            }
        }
    }

} // rcg::cams::realsense

#endif //ENABLE_REALSENSE_SDK