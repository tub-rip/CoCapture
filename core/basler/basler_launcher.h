#ifdef ENABLE_PYLON_SDK
#ifndef COCAPTURE_BASLER_LAUNCHER_H
#define COCAPTURE_BASLER_LAUNCHER_H

#include <map>

#include <pylon/PylonIncludes.h>
#include <pylon/DeviceInfo.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "camera.h"
#include "launcher_interface.h"
#include "basler_camera.h"

namespace Cocapture {

    class BaslerModel : public IModel {
    public:
        BaslerModel(int id, int type, std::unique_ptr<ICamera>& camera);

    public:
        void* GetRef(int specifier) override;
        void SetRef(int specifier, void* ref) override;

    private:
        char title_[64];

        char serial_number_[64];
        char model_name_[64];
        char vendor_name_[64];

        char trigger_selector_[64];
        char trigger_source_[64];
        char trigger_activation_[64];

        void* texture_;
        char output_directory_path_[256];

        int exposure_time_;
        bool trigger_mode_;

        int frame_count_;
    };

    class BaslerLauncher : public ILauncher {
    public:
        BaslerLauncher();

    private:
        struct BaslerCamera {
            std::string serial_number;
            std::string model_name;
            std::string vendor_name;

            std::vector<std::string> trigger_selector_options;
            std::vector<std::string> trigger_source_options;
            std::vector<std::string> trigger_activation_options;

            std::string trigger_selector;
            std::string trigger_source;
            std::string trigger_activation;
        };

    public:
        std::vector<std::string> GetCameraIdentifiers() override;
        std::string GetCameraInformation(std::string identifier, int info) override;
        std::vector<std::string> GetCameraLaunchOptions(std::string identifier, int setting) override;
        void SetCameraLaunchOption(std::string identifier, int setting, std::string option) override;
        std::unique_ptr<ICamera> GetCamera(std::string identifier) override;
        std::unique_ptr<IModel> GetModel(std::unique_ptr<ICamera>& camera, int id) override;
        void Reset() override;

    private:
        Pylon::PylonAutoInitTerm auto_init_term_;
        std::map<std::string, BaslerCamera> basler_cameras_;
    };

} // Cocapture

#endif //COCAPTURE_BASLER_LAUNCHER_H
#endif //ENABLE_PYLON_SDK