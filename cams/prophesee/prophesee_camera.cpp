#ifdef ENABLE_METAVISION_SDK

#include "prophesee_camera.h"

namespace rcg::cams::prophesee {

    PropheseeCamera::PropheseeCamera(const char* serial_number, int display_accumulation_time_us,
                                     cv::Scalar background_color, cv::Scalar on_color, cv::Scalar off_color) :

    camera_                 (Metavision::Camera::from_serial(serial_number)),
    cd_frame_generator_     (std::make_unique<Metavision::CDFrameGenerator>
                            (camera_.geometry().width(), camera_.geometry().height())),
    is_started_             (false),
    is_recording_           (false),
    biases_                 (camera_.get_device().get_facility<Metavision::I_LL_Biases>()),
    hw_identification_      (camera_.get_device().get_facility<Metavision::I_HW_Identification>()),
    camera_synchronization_ (camera_.get_device().get_facility<Metavision::I_CameraSynchronization>()),
    trigger_in_             (camera_.get_device().get_facility<Metavision::I_TriggerIn>())

    {
        this->EnableTriggerIn();
        cd_frame_generator_->set_display_accumulation_time_us(display_accumulation_time_us);
        cd_frame_generator_->set_colors(background_color, on_color, off_color, true);
        camera_.cd().add_callback([this] (const Metavision::EventCD* begin, const Metavision::EventCD* end) {
            cd_frame_generator_->add_events(begin, end);
        });

        if(std::stof(GetGeneration()) >= 4.1) {
            event_signal_processor_ = std::make_unique<EventSignalProcessor>(&camera_);
        }
    }

    std::string PropheseeCamera::GetSerialNumber() {
        return hw_identification_->get_serial();
    }

    std::string PropheseeCamera::GetGeneration() {
        return std::to_string(hw_identification_->get_sensor_info().major_version_) + "." +
               std::to_string(hw_identification_->get_sensor_info().minor_version_);
    }

    std::string PropheseeCamera::GetPluginName() {
        return hw_identification_->get_header().get_plugin_name();
    }

    int PropheseeCamera::GetCDFrameWidth() {
        return camera_.geometry().width();
    }

    int PropheseeCamera::GetCDFrameHeight() {
        return camera_.geometry().height();
    }

    int PropheseeCamera::GetBiasValue(const char* bias_name) {
        return biases_->get(bias_name);
    }

    bool PropheseeCamera::SetBiasValue(const char* bias_name, int bias_value) {
        return biases_->set(bias_name, bias_value);
    }

    std::map<std::string, std::pair<int, int>> PropheseeCamera::GetBiasRanges() {
        Metavision::LL_Bias_Info bias_diff_off_info;
        biases_->get_bias_info("bias_diff_off", bias_diff_off_info);

        Metavision::LL_Bias_Info bias_diff_on_info;
        biases_->get_bias_info("bias_diff_on", bias_diff_on_info);

        Metavision::LL_Bias_Info bias_fo_info;
        biases_->get_bias_info("bias_fo", bias_fo_info);

        Metavision::LL_Bias_Info bias_hpf_info;
        biases_->get_bias_info("bias_hpf", bias_hpf_info);

        Metavision::LL_Bias_Info bias_refr_info;
        biases_->get_bias_info("bias_refr", bias_refr_info);

        return {{"bias_diff_off", bias_diff_off_info.get_bias_recommended_range()},
                {"bias_diff_on", bias_diff_on_info.get_bias_recommended_range()},
                {"bias_fo", bias_fo_info.get_bias_recommended_range()},
                {"bias_hpf", bias_hpf_info.get_bias_recommended_range()},
                {"bias_refr", bias_refr_info.get_bias_recommended_range()}};
    }

    std::string PropheseeCamera::GetMode() {
        Metavision::I_CameraSynchronization::SyncMode mode = camera_synchronization_->get_mode();
        if(mode == Metavision::I_CameraSynchronization::SyncMode::STANDALONE) { return std::string {MODE_STANDALONE}; }
        else if(mode == Metavision::I_CameraSynchronization::SyncMode::MASTER) { return std::string {MODE_MASTER}; }
        else if(mode == Metavision::I_CameraSynchronization::SyncMode::SLAVE) { return std::string {MODE_SLAVE}; }
        else { return {}; }
    }

    bool PropheseeCamera::SetMode(const char* mode) {
        std::string mode_str = std::string {mode};
        if(mode_str == "Standalone") { return camera_synchronization_->set_mode_standalone(); }
        else if(mode_str == "Master") { return camera_synchronization_->set_mode_master(); }
        else if(mode_str == "Slave") { return camera_synchronization_->set_mode_slave(); }
        else { return false; }
    }

    EventSignalProcessor* PropheseeCamera::GetEventSignalProcessor() {
        return event_signal_processor_.get();
    }

    bool PropheseeCamera::Start(int fps) {
        bool cd_frame_generator_start {
            cd_frame_generator_->start(fps, [this] (const Metavision::timestamp& ts, const cv::Mat& frame) {
                cd_frame_mutex_.lock();
                cd_frame_ = frame;
                cd_frame_mutex_.unlock();
            })
        };
        bool camera_start {camera_.start()};

        if(cd_frame_generator_start && camera_start) {
            is_started_ = true;
        }

        return cd_frame_generator_start && camera_start;
    }

    bool PropheseeCamera::Stop() {
        bool camera_stop {camera_.stop()};
        bool cd_frame_generator_stop {cd_frame_generator_->stop()};

        if(camera_stop && cd_frame_generator_stop) {
            is_started_ = false;
        }

        return camera_stop && cd_frame_generator_stop;
    }

    void PropheseeCamera::OutputCDFrame(cv::Mat &cd_frame) {
        cd_frame_mutex_.lock();
        cd_frame = cd_frame_;
        cd_frame_mutex_.unlock();
    }

    bool PropheseeCamera::EnableTriggerIn() {
        return trigger_in_->enable(Metavision::I_TriggerIn::Channel::Main);
    }

    bool PropheseeCamera::DisableTriggerIn() {
        return trigger_in_->disable(Metavision::I_TriggerIn::Channel::Main);
    }

    bool PropheseeCamera::StartRecording(const char* output_dir) {
        if(is_recording_) {
            return false;
        }

        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }
        camera_.start_recording(output_dir_str + "/output.raw");
        is_recording_ = true;

        return true;
    }

    bool PropheseeCamera::StopRecording() {
        if(!is_recording_) {
            return false;
        }

        camera_.stop_recording();
        is_recording_ = false;
        return true;
    }

    void PropheseeCamera::AnalyzeRecording(const char* output_dir) {
        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

        std::fstream recording_info;
        recording_info.open(output_dir_str + "/recording_info.txt", std::ios::out);

        // Count trigger events
        std::fstream trigger_events_file;
        trigger_events_file.open(output_dir_str + "/trigger_events.txt", std::ios::in);

        int trigger_events_count {0};
        std::string trigger_event;
        while(std::getline(trigger_events_file, trigger_event)) {
            ++trigger_events_count;
        }

        recording_info << "Trigger events: " << std::to_string(trigger_events_count);
    }

    std::vector<std::string> PropheseeCamera::ListConnectedCameras() {
        std::vector<std::string> serial_numbers;
        for(const auto& camera_description : Metavision::DeviceDiscovery::list_available_sources()) {
            serial_numbers.push_back(camera_description.serial_);
        }
        return serial_numbers;
    }

} // rcg::cams::prophesee

#endif //ENABLE_METAVISION_SDK