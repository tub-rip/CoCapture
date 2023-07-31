#ifdef ENABLE_REALSENSE_SDK

#include "l5.h"

namespace rcg::cams::realsense {

    L5::L5(const char *serial_number) :

    is_started_    (false),
    is_recording_  (false),
    frame_sequence_(0),
    syncer_        (2)

    {
        rs2::device_list devices = GetContext().query_devices();

        for(rs2::device device : devices) {
            if(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) == std::string(serial_number)) {
                device_ = device;
                break;
            }
        }

        for(rs2::sensor sensor : device_.query_sensors()) {
            std::string sensor_name = sensor.get_info(RS2_CAMERA_INFO_NAME);

            if (sensor_name.find("Depth") != std::string::npos) {
                depth_sensor_ = sensor;
            } else if (sensor_name.find("RGB") != std::string::npos) {
                color_sensor_ = sensor;
            }
        }

        depth_sensor_.open(GetPrefStreamProfiles(depth_sensor_));
        color_sensor_.open(GetPrefStreamProfiles(color_sensor_));
    }

    L5::~L5() {
        Stop();
        depth_sensor_.close();
        color_sensor_.close();
    }

    std::vector<rs2::stream_profile> L5::GetPrefStreamProfiles(rs2::sensor &sensor) {
        std::vector<rs2::stream_profile> stream_profiles;
        std::string sensor_name = sensor.get_info(RS2_CAMERA_INFO_NAME);

        std::vector<rs2::stream_profile> available_stream_profiles = sensor.get_stream_profiles();
        for(rs2::stream_profile stream_profile : available_stream_profiles) {
            if(!stream_profile.is<rs2::video_stream_profile>()) { continue; }

            rs2::video_stream_profile video_stream_profile = stream_profile.as<rs2::video_stream_profile>();

            if(sensor_name.find("Depth") != std::string::npos) {
                // Infrared stream
                if(video_stream_profile.format()  == RS2_FORMAT_Y8 &&
                   video_stream_profile.width()   == 640 &&
                   video_stream_profile.height()  == 480 &&
                   video_stream_profile.fps()     == 30) {
                   stream_profiles.push_back(stream_profile);
                }
            } else if(sensor_name.find("RGB") != std::string::npos) {
                // Color stream
                if((video_stream_profile.format() == RS2_FORMAT_BGR8 &&
                    video_stream_profile.width()  == 1280 &&
                    video_stream_profile.height() == 720 &&
                    video_stream_profile.fps()    == 30)) {
                    stream_profiles.push_back(stream_profile);
                }
            }
        }

        return stream_profiles;
    }

    void L5::OutputIRFrame(cv::Mat& ir_frame) {
        ir_frame_mutex_.lock();
        ir_frame = ir_frame_;
        ir_frame_mutex_.unlock();
    }

    rs2::device& L5::GetDevice() {
        return device_;
    }

    void L5::Stream() {
        while(true) {
            is_started_mutex_.lock();
            if(!is_started_) {
                is_started_mutex_.unlock();
                break;
            }
            is_started_mutex_.unlock();

            // Unite matching frames to a composite frame
            rs2::frameset fset = syncer_.wait_for_frames();

            // Infrared frame
            rs2::frame infrared_frame = fset.first_or_default(RS2_STREAM_INFRARED, RS2_FORMAT_Y8);
            cv::Mat infrared_mat;
            if(infrared_frame) {
                infrared_mat = cv::Mat {cv::Size(infrared_frame.as<rs2::video_frame>().get_width(), infrared_frame.as<rs2::video_frame>().get_height()),
                                        CV_8UC1,
                                        (void*) infrared_frame.get_data(),
                                        cv::Mat::AUTO_STEP};
            }

            // Color frame
            rs2::frame color_frame = fset.first_or_default(RS2_STREAM_COLOR, RS2_FORMAT_BGR8);
            cv::Mat color_mat;
            if(color_frame) {
                color_mat = cv::Mat {cv::Size(color_frame.as<rs2::video_frame>().get_width(), color_frame.as<rs2::video_frame>().get_height()),
                                     CV_8UC3,
                                     (void*) color_frame.get_data(),
                                     cv::Mat::AUTO_STEP};
            }

            // Save frames
            is_recording_mutex_.lock();
            if(is_recording_) {
                if(infrared_frame) {
                    std::string file_name = output_dir_ + "/" + "infrared" + "-" + std::to_string(frame_sequence_);
                    cv::imwrite(file_name + ".png", infrared_mat);
                    MetadataToCSV(infrared_frame, file_name + ".csv");
                }

                if(color_frame) {
                    std::string file_name = output_dir_ + "/" + "color" + "-" + std::to_string(frame_sequence_);
                    cv::imwrite(file_name + ".png", color_mat);
                    MetadataToCSV(color_frame, file_name + ".csv");
                }

                ++frame_sequence_;
            }
            is_recording_mutex_.unlock();

            // Output infrared frame for visualization
            if(infrared_frame) {
                ir_frame_mutex_.lock();
                infrared_mat.copyTo(ir_frame_);
                ir_frame_mutex_.unlock();
            }
        }
    }

    bool L5::IsStarted() {
        is_started_mutex_.lock();
        bool is_started {is_started_};
        is_started_mutex_.unlock();
        return is_started;
    }

    bool L5::Start() {
        is_started_mutex_.lock();
        if(is_started_) {
            is_started_mutex_.unlock();
            return false;
        }

        is_started_ = true;
        is_started_mutex_.unlock();

        // Start streaming
        depth_sensor_.start(syncer_);
        color_sensor_.start(syncer_);
        sensors_thread_ = std::thread(&L5::Stream, this);

        return true;
    }

    bool L5::Stop() {
        is_started_mutex_.lock();
        if(!is_started_) {
            is_started_mutex_.unlock();
            return false;
        }

        is_started_ = false;
        is_started_mutex_.unlock();

        // Stop streaming
        sensors_thread_.join();
        depth_sensor_.stop();
        color_sensor_.stop();

        return true;
    }

    void L5::OutputFrame(cv::Mat& frame) {
        OutputIRFrame(frame);

        if(!frame.empty()) {
            cv::equalizeHist(frame, frame);
            cv::applyColorMap(frame, frame, cv::COLORMAP_JET);
        }
    }

    std::pair<int, int> L5::GetOutputFrameDimensions() {
        return {640, 480};
    }

    bool L5::IsRecording() {
        is_recording_mutex_.lock();
        bool is_recording {is_recording_};
        is_recording_mutex_.unlock();
        return is_recording;
    }

    bool L5::StartRecording(const char* output_dir) {
        is_recording_mutex_.lock();
        if(is_recording_) {
            is_recording_mutex_.unlock();
            return false;
        }

        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }
        frame_sequence_ = 0;

        is_recording_ = true;
        is_recording_mutex_.unlock();

        return true;
    }

    bool L5::StopRecording() {
        is_recording_mutex_.lock();
        if(!is_recording_) {
            is_recording_mutex_.unlock();
            return false;
        }

        is_recording_ = false;
        is_recording_mutex_.unlock();

        return true;
    }

} // rcg::cams:.realsense

#endif //ENABLE_REALSENSE_SDK