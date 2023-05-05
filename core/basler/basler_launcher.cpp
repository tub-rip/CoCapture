#ifdef ENABLE_PYLON_SDK

#include "basler_launcher.h"

namespace Cocapture {

    BaslerModel::BaslerModel(int id, int type, std::unique_ptr<ICamera>& camera) : IModel(id, type) {
        std::string title = "basler" + std::to_string(id);
        strncpy(title_, title.c_str(), title.size() + 1);

        strncpy(serial_number_, camera->GetInfo(BASLER_INFO_SERIAL).c_str(), camera->GetInfo(BASLER_INFO_SERIAL).size() + 1);
        strncpy(model_name_, camera->GetInfo(BASLER_INFO_MODEL).c_str(), camera->GetInfo(BASLER_INFO_MODEL).size() + 1);
        strncpy(vendor_name_, camera->GetInfo(BASLER_INFO_VENDOR).c_str(), camera->GetInfo(BASLER_INFO_VENDOR).size() + 1);

        strncpy(trigger_selector_, camera->GetInfo(BASLER_TRIGGER_SELECTOR).c_str(), camera->GetInfo(BASLER_TRIGGER_SELECTOR).size() + 1);
        strncpy(trigger_source_, camera->GetInfo(BASLER_TRIGGER_SOURCE).c_str(), camera->GetInfo(BASLER_TRIGGER_SOURCE).size() + 1);
        strncpy(trigger_activation_, camera->GetInfo(BASLER_TRIGGER_ACTIVATION).c_str(), camera->GetInfo(BASLER_TRIGGER_ACTIVATION).size() + 1);

        std::memset(output_directory_path_, 0, sizeof(output_directory_path_));

        exposure_time_ = camera->GetProp(BASLER_EXPOSURE_TIME);
        trigger_mode_ = camera->GetProp(BASLER_TRIGGER_MODE) == BASLER_TRIGGER_MODE_ON ? true : false;
    }

    void* BaslerModel::GetRef(int specifier) {
        switch(specifier) {
            case BASLER_GUI_TITLE:
                return title_;

            case BASLER_INFO_SERIAL:
                return serial_number_;

            case BASLER_INFO_MODEL:
                return model_name_;

            case BASLER_INFO_VENDOR:
                return vendor_name_;

            case BASLER_TRIGGER_SELECTOR:
                return trigger_selector_;

            case BASLER_TRIGGER_SOURCE:
                return trigger_source_;

            case BASLER_TRIGGER_ACTIVATION:
                return trigger_activation_;

            case BASLER_EXPOSURE_TIME:
                return &exposure_time_;

            case BASLER_TRIGGER_MODE:
                return &trigger_mode_;

            case BASLER_GUI_TEXTURE:
                return texture_;

            case BASLER_GUI_PATH:
                return output_directory_path_;

            case BASLER_GUI_FRAMES:
                return &frame_count_;
        }

        return {};
    }

    void BaslerModel::SetRef(int specifier, void *ref) {
        switch(specifier) {
            case BASLER_GUI_TEXTURE:
                texture_ = ref;
                break;

            case BASLER_GUI_FRAMES:
                frame_count_ = *((int*) ref);
        }
    }

    BaslerLauncher::BaslerLauncher() {
        Type = CAM_TYPE_BASLER;
        Reset();
    }

    std::vector<std::string> BaslerLauncher::GetCameraIdentifiers() {
        std::vector<std::string> basler_camera_identifiers;
        for(std::pair<std::string, BaslerCamera> basler_camera : basler_cameras_) {
            basler_camera_identifiers.push_back(basler_camera.first);
        }
        return basler_camera_identifiers;
    }

    std::string BaslerLauncher::GetCameraInformation(std::string identifier, int info) {
        switch(info) {
            case BASLER_INFO_SERIAL:
                return basler_cameras_[identifier].serial_number;

            case BASLER_INFO_MODEL:
                return basler_cameras_[identifier].model_name;

            case BASLER_INFO_VENDOR:
                return basler_cameras_[identifier].vendor_name;
        }

        return {};
    }

    std::vector<std::string> BaslerLauncher::GetCameraLaunchOptions(std::string identifier, int setting) {
        switch(setting) {
            case BASLER_TRIGGER_SELECTOR:
                return basler_cameras_[identifier].trigger_selector_options;

            case BASLER_TRIGGER_SOURCE:
                return basler_cameras_[identifier].trigger_source_options;

            case BASLER_TRIGGER_ACTIVATION:
                return basler_cameras_[identifier].trigger_activation_options;
        }

        return {};
    }

    void BaslerLauncher::SetCameraLaunchOption(std::string identifier, int setting, std::string option) {
        switch(setting) {
            case BASLER_TRIGGER_SELECTOR:
                basler_cameras_[identifier].trigger_selector = option;
                break;

            case BASLER_TRIGGER_SOURCE:
                basler_cameras_[identifier].trigger_source = option;
                break;

            case BASLER_TRIGGER_ACTIVATION:
                basler_cameras_[identifier].trigger_activation = option;
        }
    }

    std::unique_ptr<ICamera> BaslerLauncher::GetCamera(std::string identifier) {
        std::unique_ptr<Cocapture::BaslerCamera> basler_camera = std::make_unique<Cocapture::BaslerCamera>(identifier);

        std::string trigger_selector = basler_cameras_[identifier].trigger_selector;
        if(!trigger_selector.empty()) { basler_camera->SetProp(BASLER_TRIGGER_SELECTOR, Cocapture::BaslerCamera::ToEnum(trigger_selector.c_str())); }

        std::string trigger_source = basler_cameras_[identifier].trigger_source;
        if(!trigger_source.empty()) { basler_camera->SetProp(BASLER_TRIGGER_SOURCE, Cocapture::BaslerCamera::ToEnum(trigger_source.c_str())); }

        std::string trigger_activation = basler_cameras_[identifier].trigger_activation;
        if(!trigger_activation.empty()) { basler_camera->SetProp(BASLER_TRIGGER_ACTIVATION, Cocapture::BaslerCamera::ToEnum(trigger_activation.c_str())); }

        return basler_camera;
    }

    std::unique_ptr<IModel> BaslerLauncher::GetModel(std::unique_ptr<ICamera>& camera, int id) {
        return std::make_unique<BaslerModel>(id, CAM_TYPE_BASLER, camera);
    }

    void BaslerLauncher::Reset() {
        basler_cameras_.clear();

        Pylon::CTlFactory& tl_factory = Pylon::CTlFactory::GetInstance();
        Pylon::DeviceInfoList device_info_list;
        tl_factory.EnumerateDevices(device_info_list);

        for(Pylon::DeviceInfoList::const_iterator it = device_info_list.begin(); it != device_info_list.end(); ++it) {
            Pylon::CBaslerUniversalInstantCamera camera;
            camera.Attach(tl_factory.CreateDevice(Pylon::CDeviceInfo().SetSerialNumber(it->GetSerialNumber())));
            camera.Open();

            BaslerCamera basler_camera;

            basler_camera.serial_number = std::string {camera.GetDeviceInfo().GetSerialNumber()};
            basler_camera.model_name = std::string {camera.GetDeviceInfo().GetModelName()};
            basler_camera.vendor_name = std::string {camera.GetDeviceInfo().GetVendorName()};

            GenApi::StringList_t values;

            camera.TriggerSelector.GetSettableValues(values);
            for(Pylon::String_t value : values) { basler_camera.trigger_selector_options.push_back(std::string {value}); }

            camera.TriggerSource.GetSettableValues(values);
            for(Pylon::String_t value : values) {
                if(std::string(value).find("Line") != std::string::npos) {
                    basler_camera.trigger_source_options.push_back(std::string {value});
                }
            }

            camera.TriggerActivation.GetSettableValues(values);
            for(Pylon::String_t value : values) { basler_camera.trigger_activation_options.push_back(std::string {value}); }

            basler_cameras_.insert({basler_camera.serial_number, basler_camera});

            camera.Close();
            camera.DestroyDevice();
        }
    }

} // Cocapture

#endif //ENABLE_PYLON_SDK