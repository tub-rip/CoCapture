#ifdef ENABLE_METAVISION_SDK

#include "prophesee_launcher.h"

namespace Cocapture {

    PropheseeModel::PropheseeModel(int id, int type, std::unique_ptr<ICamera> &camera) : IModel(id, type) {
        std::string title = "prophesee" + std::to_string(id);
        strncpy(title_, title.c_str(), title.size() + 1);

        strncpy(serial_number_, camera->GetInfo(PROPHESEE_INFO_SERIAL).c_str(), camera->GetInfo(PROPHESEE_INFO_SERIAL).size() + 1);
        strncpy(integrator_name_, camera->GetInfo(PROPHESEE_INFO_INTEGRATOR).c_str(), camera->GetInfo(PROPHESEE_INFO_INTEGRATOR).size() + 1);
        strncpy(plugin_name_, camera->GetInfo(PROPHESEE_INFO_PLUGIN).c_str(), camera->GetInfo(PROPHESEE_INFO_PLUGIN).size() + 1);
        strncpy(generation_, camera->GetInfo(PROPHESEE_INFO_GENERATION).c_str(), camera->GetInfo(PROPHESEE_INFO_GENERATION).size() + 1);
        strncpy(raw_formats_, camera->GetInfo(PROPHESEE_INFO_RAW_FORMATS).c_str(), camera->GetInfo(PROPHESEE_INFO_RAW_FORMATS).size() + 1);

        strncpy(synchronization_mode_, camera->GetInfo(PROPHESEE_SYNC_MODE).c_str(), camera->GetInfo(PROPHESEE_SYNC_MODE).size() + 1);

        std::memset(output_directory_path_, 0, sizeof(output_directory_path_));

        bias_diff_off_ = camera->GetProp(PROPHESEE_BIAS_DIFF_OFF);

        bias_diff_on_ = camera->GetProp(PROPHESEE_BIAS_DIFF_ON);

        bias_fo_ = camera->GetProp(PROPHESEE_BIAS_FO);

        bias_hpf_ = camera->GetProp(PROPHESEE_BIAS_HPF);

        bias_refr_ = camera->GetProp(PROPHESEE_BIAS_REFR);
    }

    void* PropheseeModel::GetRef(int specifier) {
        switch(specifier) {
            case PROPHESEE_GUI_TITLE:
                return title_;

            case PROPHESEE_INFO_SERIAL:
                return serial_number_;

            case PROPHESEE_INFO_INTEGRATOR:
                return integrator_name_;

            case PROPHESEE_INFO_PLUGIN:
                return plugin_name_;

            case PROPHESEE_INFO_GENERATION:
                return generation_;

            case PROPHESEE_INFO_RAW_FORMATS:
                return raw_formats_;

            case PROPHESEE_SYNC_MODE:
                return synchronization_mode_;

            case PROPHESEE_BIAS_DIFF_OFF:
                return &bias_diff_off_;

            case PROPHESEE_BIAS_DIFF_ON:
                return &bias_diff_on_;

            case PROPHESEE_BIAS_FO:
                return &bias_fo_;

            case PROPHESEE_BIAS_HPF:
                return &bias_hpf_;

            case PROPHESEE_BIAS_REFR:
                return &bias_refr_;

            case PROPHESEE_GUI_TEXTURE:
                return texture_;

            case PROPHESEE_GUI_PATH:
                return output_directory_path_;

            case PROPHESEE_GUI_TRIGGERS:
                return &ext_trigger_event_count_;
        }

        return {};
    }

    void PropheseeModel::SetRef(int specifier, void* ref) {
        switch(specifier) {
            case PROPHESEE_GUI_TEXTURE:
                texture_ = ref;
                break;

            case PROPHESEE_GUI_TRIGGERS:
                ext_trigger_event_count_ = *((int*) ref);
        }
    }

    PropheseeLauncher::PropheseeLauncher() {
        Type = CAM_TYPE_PROPHESEE;
        Reset();
    }

    std::vector<std::string> PropheseeLauncher::GetCameraIdentifiers() {
        std::vector<std::string> prophesee_camera_identifiers;
        for(std::pair<std::string, PropheseeCamera> prophesee_camera : prophesee_cameras_) {
            prophesee_camera_identifiers.push_back(prophesee_camera.first);
        }
        return prophesee_camera_identifiers;
    }

    std::string PropheseeLauncher::GetCameraInformation(std::string identifier, int info) {
        switch(info) {
            case PROPHESEE_INFO_SERIAL:
                return prophesee_cameras_[identifier].serial_number;

            case PROPHESEE_INFO_INTEGRATOR:
                return prophesee_cameras_[identifier].integrator_name;

            case PROPHESEE_INFO_PLUGIN:
                return prophesee_cameras_[identifier].plugin_name;
        }

        return {};
    }

    std::vector<std::string> PropheseeLauncher::GetCameraLaunchOptions(std::string identifier, int setting) {
        switch(setting) {
            case PROPHESEE_SYNC_MODE:
                return prophesee_cameras_[identifier].synchronization_mode_options;
        }

        return {};
    }

    void PropheseeLauncher::SetCameraLaunchOption(std::string identifier, int setting, std::string option) {
        switch(setting) {
            case PROPHESEE_SYNC_MODE:
                prophesee_cameras_[identifier].synchronization_mode = option;
        }
    }

    std::unique_ptr<ICamera> PropheseeLauncher::GetCamera(std::string identifier) {
        std::unique_ptr<Cocapture::PropheseeCamera> prophesee_camera = std::make_unique<Cocapture::PropheseeCamera>(identifier);

        std::string synchronization_mode = prophesee_cameras_[identifier].synchronization_mode;
        if(!synchronization_mode.empty()) { prophesee_camera->SetProp(PROPHESEE_SYNC_MODE, Cocapture::PropheseeCamera::ToEnum(synchronization_mode)); }

        return prophesee_camera;
    }

    std::unique_ptr<IModel> PropheseeLauncher::GetModel(std::unique_ptr<ICamera>& camera, int id) {
        return std::make_unique<PropheseeModel>(id, CAM_TYPE_PROPHESEE, camera);
    }

    void PropheseeLauncher::Reset() {
        prophesee_cameras_.clear();

        for(Metavision::CameraDescription desc : Metavision::DeviceDiscovery::list_available_sources()) {
            PropheseeCamera prophesee_camera;

            prophesee_camera.serial_number = desc.serial_;
            prophesee_camera.integrator_name = desc.integrator_name_;
            prophesee_camera.plugin_name = desc.plugin_name_;

            prophesee_camera.synchronization_mode_options = {"Standalone", "Master", "Slave"};

            prophesee_cameras_.insert({prophesee_camera.serial_number, prophesee_camera});
        }
    }

} // Cocapture

#endif //ENABLE_METAVISION_SDK