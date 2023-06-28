#ifdef ENABLE_METAVISION_SDK

#include "prophesee_camera.h"

namespace rcg::cams::prophesee {

    PropheseeCamera::PropheseeCamera(const char* serial_number, int display_accumulation_time_us,
                                     cv::Scalar background_color, cv::Scalar on_color, cv::Scalar off_color) :

    camera_             (Metavision::Camera::from_serial(serial_number)),
    cd_frame_generator_ (std::make_unique<Metavision::CDFrameGenerator>
                        (camera_.geometry().width(), camera_.geometry().height())),
    is_started_         (false),
    is_recording_       (false),
    biases_             (camera_.get_device().get_facility<Metavision::I_LL_Biases>()),
    hw_identification_  (camera_.get_device().get_facility<Metavision::I_HW_Identification>()),
    device_control_     (camera_.get_device().get_facility<Metavision::I_DeviceControl>()),
    trigger_in_         (camera_.get_device().get_facility<Metavision::I_TriggerIn>())

    {
        cd_frame_generator_->set_display_accumulation_time_us(display_accumulation_time_us);
        cd_frame_generator_->set_colors(background_color, on_color, off_color, true);
        camera_.cd().add_callback([this] (const Metavision::EventCD* begin, const Metavision::EventCD* end) {
            cd_frame_generator_->add_events(begin, end);
        });
    }

    std::string PropheseeCamera::GetSerialNumber() {
        return hw_identification_->get_serial();
    }

    std::string PropheseeCamera::GetGeneration() {
        return hw_identification_->get_sensor_info().as_string();
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

    std::string PropheseeCamera::GetMode() {
        Metavision::I_DeviceControl::SyncMode mode = device_control_->get_mode();
        if(mode == Metavision::I_DeviceControl::SyncMode::STANDALONE) { return std::string {MODE_STANDALONE}; }
        else if(mode == Metavision::I_DeviceControl::SyncMode::MASTER) { return std::string {MODE_MASTER}; }
        else if(mode == Metavision::I_DeviceControl::SyncMode::SLAVE) { return std::string {MODE_SLAVE}; }
        else { return {}; }
    }

    bool PropheseeCamera::SetMode(const char* mode) {
        std::string mode_str = std::string {mode};
        if(mode_str == "Standalone") { return device_control_->set_mode_standalone(); }
        else if(mode_str == "Master") { return device_control_->set_mode_master(); }
        else if(mode_str == "Slave") { return device_control_->set_mode_slave(); }
        else { return false; }
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
        if(GetPluginName().find("evk3") != std::string::npos ||
           GetPluginName().find("evk4") != std::string::npos) {
            return trigger_in_->enable(0);
        } else {
            return false;
        }
    }

    bool PropheseeCamera::DisableTriggerIn() {
        if(GetPluginName().find("evk3") != std::string::npos ||
           GetPluginName().find("evk4") != std::string::npos) {
            return trigger_in_->disable(0);
        } else {
            return false;
        }
    }

    bool PropheseeCamera::StartRecording(const char* output_dir) {
        if(is_recording_) {
            return false;
        }

        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

        trigger_event_saver_.OpenFile((output_dir_str + "/trigger_events.txt").c_str());
        size_t callback_id = camera_.ext_trigger().add_callback([this] (const Metavision::EventExtTrigger* begin,
                                                                            const Metavision::EventExtTrigger* end) {
            trigger_event_saver_.AddEvents(begin, end);
        });
        trigger_event_saver_.SetCallBackId(callback_id);
        camera_.start_recording(output_dir_str + "/output");
        is_recording_ = true;

        return true;
    }

    bool PropheseeCamera::StopRecording() {
        if(!is_recording_) {
            return false;
        }

        camera_.stop_recording();
        is_recording_ = false;
        size_t callback_id = trigger_event_saver_.GetCallBackId();
        camera_.ext_trigger().remove_callback(callback_id);
        trigger_event_saver_.CloseFile();

        return true;
    }

    std::vector<std::string> PropheseeCamera::ListSerialNumbers() {
        std::vector<std::string> serial_numbers;

        for(Metavision::CameraDescription& desc : Metavision::DeviceDiscovery::list_available_sources()) {
            serial_numbers.push_back(desc.serial_);
        }

        return serial_numbers;
    }

    std::map<std::string, std::pair<int, int>> PropheseeCamera::ReadBiasRanges(const char* bias_file, const char* camera_generation) {
        std::fstream bias_ranges_file;
        bias_ranges_file.open(bias_file, std::ios::in);

        std::map<std::string, std::pair<int, int>> bias_ranges;
        bool found_ranges = false;
        std::string entry;

        while(std::getline(bias_ranges_file, entry)) {
            if(entry.find(camera_generation) != std::string::npos) {
                found_ranges = true;
                continue;
            }

            if(!found_ranges) {
                continue;
            }

            if(found_ranges && entry.empty()) {
                break;
            }

            std::stringstream ss(entry);
            std::string element;
            std::vector<std::string> entry_vector;
            while(std::getline(ss, element, ' ')) {
                entry_vector.push_back(element);
            }
            std::string bias_name = entry_vector.front();
            entry_vector.erase(entry_vector.begin());
            std::pair<int, int> bias_range = {std::stoi(entry_vector.front()),
                                              std::stoi(entry_vector.back())};
            bias_ranges.insert({bias_name, bias_range});
        }

        bias_ranges_file.close();

        return bias_ranges;
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

} // rcg::cams::prophesee

#endif //ENABLE_METAVISION_SDK