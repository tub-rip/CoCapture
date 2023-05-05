#ifdef ENABLE_METAVISION_SDK

#include "prophesee_camera.h"

namespace Cocapture {

    PropheseeCamera::PropheseeCamera(std::string serial_number) :

    camera_             (Metavision::Camera::from_serial(serial_number)),
    cd_frame_generator_ (std::make_unique<Metavision::CDFrameGenerator>
                        (camera_.geometry().width(), camera_.geometry().height())),
    hw_identification_  (camera_.get_device().get_facility<Metavision::I_HW_Identification>()),
    device_control_     (camera_.get_device().get_facility<Metavision::I_DeviceControl>()),
    trigger_in_         (camera_.get_device().get_facility<Metavision::I_TriggerIn>()),
    biases_             (camera_.get_device().get_facility<Metavision::I_LL_Biases>())

    {
        cd_frame_generator_->set_display_accumulation_time_us(10000);
        cd_frame_generator_->set_colors({255, 255, 255},
                                        {0, 0, 255},
                                        {255, 0, 0},
                                        true);

        camera_.cd().add_callback([this] (const Metavision::EventCD* begin, const Metavision::EventCD* end) {
            cd_frame_generator_->add_events(begin, end);
        });

        cd_frame_generator_->start(30, [this] (const Metavision::timestamp& ts, const cv::Mat& frame) {
            cd_frame_mutex_.lock();
            cd_frame_ = frame;
            cd_frame_mutex_.unlock();
        });

        ReadBiasRanges("../core/prophesee/bias_ranges.txt", GetInfo(PROPHESEE_INFO_GENERATION));

        if(GetInfo(PROPHESEE_INFO_PLUGIN).find("evk3") != std::string::npos ||
           GetInfo(PROPHESEE_INFO_PLUGIN).find("evk4") != std::string::npos) {
            while(!trigger_in_->enable(0));
        }
    }

    void PropheseeCamera::Start() { camera_.start(); }

    std::string PropheseeCamera::GetInfo(int info) {
        switch(info) {
            case PROPHESEE_INFO_SERIAL:
                return hw_identification_->get_serial();

            case PROPHESEE_INFO_GENERATION:
                return hw_identification_->get_sensor_info().as_string();

            case PROPHESEE_INFO_INTEGRATOR:
                return hw_identification_->get_integrator();

            case PROPHESEE_INFO_PLUGIN:
                return hw_identification_->get_header().get_plugin_name();

            case PROPHESEE_INFO_RAW_FORMATS: {
                std::stringstream ss;
                std::vector<std::string> raw_formats = hw_identification_->get_available_raw_format();
                for (auto it = raw_formats.begin(); it != raw_formats.end(); ++it) {
                    ss << *it;
                    if (it != raw_formats.end() - 1) {
                        ss << ", ";
                    }
                }
                return ss.str();
            }

            case PROPHESEE_SYNC_MODE:
                return ToString(GetSyncMode());
        }

        return {};
    }

    double PropheseeCamera::GetProp(int property) {
        switch(property) {
            case PROPHESEE_GEOMETRY_WIDTH:
                return camera_.geometry().width();

            case PROPHESEE_GEOMETRY_HEIGHT:
                return camera_.geometry().height();

            case PROPHESEE_BIAS_DIFF_OFF:
                return biases_->get("bias_diff_off");

            case PROPHESEE_BIAS_DIFF_OFF_MIN:
                return bias_ranges_.bias_diff_off_range.first;

            case PROPHESEE_BIAS_DIFF_OFF_MAX:
                return bias_ranges_.bias_diff_off_range.second;

            case PROPHESEE_BIAS_DIFF_ON:
                return biases_->get("bias_diff_on");

            case PROPHESEE_BIAS_DIFF_ON_MIN:
                return bias_ranges_.bias_diff_on_range.first;

            case PROPHESEE_BIAS_DIFF_ON_MAX:
                return bias_ranges_.bias_diff_on_range.second;

            case PROPHESEE_BIAS_FO:
                return biases_->get("bias_fo");

            case PROPHESEE_BIAS_FO_MIN:
                return bias_ranges_.bias_fo_range.first;

            case PROPHESEE_BIAS_FO_MAX:
                return bias_ranges_.bias_fo_range.second;

            case PROPHESEE_BIAS_HPF:
                return biases_->get("bias_hpf");

            case PROPHESEE_BIAS_HPF_MIN:
                return bias_ranges_.bias_hpf_range.first;

            case PROPHESEE_BIAS_HPF_MAX:
                return bias_ranges_.bias_hpf_range.second;

            case PROPHESEE_BIAS_REFR:
                return biases_->get("bias_refr");

            case PROPHESEE_BIAS_REFR_MIN:
                return bias_ranges_.bias_refr_range.first;

            case PROPHESEE_BIAS_REFR_MAX:
                return bias_ranges_.bias_refr_range.second;
        }

        return {};
    }

    void PropheseeCamera::SetProp(int property, double value) {
        switch(property) {
            case PROPHESEE_SYNC_MODE:
                SetSyncMode(value);
                break;

            case PROPHESEE_BIAS_DIFF_OFF:
                biases_->set("bias_diff_off", value);
                break;

            case PROPHESEE_BIAS_DIFF_ON:
                biases_->set("bias_diff_on", value);
                break;

            case PROPHESEE_BIAS_FO:
                biases_->set("bias_fo", value);
                break;

            case PROPHESEE_BIAS_HPF:
                biases_->set("bias_hpf", value);
                break;

            case PROPHESEE_BIAS_REFR:
                biases_->set("bias_refr", value);
        }
    }

    void PropheseeCamera::OutputFrame(int, cv::Mat& cd_frame) {
        cd_frame_mutex_.lock();
        cd_frame = cd_frame_;
        cd_frame_mutex_.unlock();
    }

    void PropheseeCamera::StartRecording(std::string output_directory_path) {
        if(output_directory_path.back() == '/') { output_directory_path.pop_back(); }

        trigger_event_saver_.Open(output_directory_path + "/trigger_events.txt");
        camera_.start_recording(output_directory_path + "/output");
        size_t callback_id = camera_.ext_trigger().add_callback([this] (const Metavision::EventExtTrigger* begin,
                                                                        const Metavision::EventExtTrigger* end) {
            trigger_event_saver_.AddEvents(begin, end);
        });

        trigger_event_saver_.SaveCallbackId(callback_id);
    }

    void PropheseeCamera::StopRecording() {
        camera_.ext_trigger().remove_callback(trigger_event_saver_.Close());
        camera_.stop_recording();
    }

    void PropheseeCamera::Clean() { camera_.stop(); }

    std::string PropheseeCamera::ToString(int value) {
        switch(value) {
            case PROPHESEE_MODE_STANDALONE:
                return "Standalone";
            case PROPHESEE_MODE_MASTER:
                return "Master";
            case PROPHESEE_MODE_SLAVE:
                return "Slave";
        }

        return {};
    }

    int PropheseeCamera::ToEnum(std::string value) {
        if(value == "Standalone") { return PROPHESEE_MODE_STANDALONE; }

        else if(value == "Master") { return PROPHESEE_MODE_MASTER; }

        else if(value == "Slave") { return PROPHESEE_MODE_SLAVE; }

        return {};
    }

    void PropheseeCamera::ReadBiasRanges(std::string file_name, std::string camera_generation) {
        std::fstream bias_ranges_file;
        bias_ranges_file.open(file_name, std::ios::in);

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

        bias_ranges_.bias_diff_off_range = { bias_ranges["bias_diff_off"].first, bias_ranges["bias_diff_off"].second };
        bias_ranges_.bias_diff_on_range = { bias_ranges["bias_diff_on"].first, bias_ranges["bias_diff_on"].second };
        bias_ranges_.bias_fo_range = { bias_ranges["bias_fo"].first, bias_ranges["bias_fo"].second };
        bias_ranges_.bias_hpf_range = { bias_ranges["bias_hpf"].first, bias_ranges["bias_hpf"].second };
        bias_ranges_.bias_refr_range = { bias_ranges["bias_refr"].first, bias_ranges["bias_refr"].second };
    }

    int PropheseeCamera::GetSyncMode() {
        if(device_control_->get_mode() == Metavision::I_DeviceControl::SyncMode::MASTER) {
            return PROPHESEE_MODE_MASTER;
        } else if(device_control_->get_mode() == Metavision::I_DeviceControl::SyncMode::SLAVE) {
            return PROPHESEE_MODE_SLAVE;
        } else if(device_control_->get_mode() == Metavision::I_DeviceControl::SyncMode::STANDALONE) {
            return PROPHESEE_MODE_STANDALONE;
        }

        return {};
    }

    void PropheseeCamera::SetSyncMode(int synchronization_mode) {
        switch(synchronization_mode) {
            case PROPHESEE_MODE_STANDALONE:
                while (!device_control_->set_mode_standalone());
                break;

            case PROPHESEE_MODE_MASTER:
                while (!device_control_->set_mode_master());
                break;

            case PROPHESEE_MODE_SLAVE:
                while (!device_control_->set_mode_slave());
        }
    }

} // Cocapture

#endif //ENABLE_METAVISION_SDK