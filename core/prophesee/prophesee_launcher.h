#ifdef ENABLE_METAVISION_SDK
#ifndef COCAPTURE_PROPHESEE_LAUNCHER_H
#define COCAPTURE_PROPHESEE_LAUNCHER_H

#include <metavision/hal/device/device_discovery.h>

#include "launcher_interface.h"
#include "prophesee_camera.h"

namespace Cocapture {

    class PropheseeModel : public IModel {
    public:
        PropheseeModel(int id, int type, std::unique_ptr<ICamera>& camera);

    public:
        void* GetRef(int specifier) override;
        void SetRef(int specifier, void* ref) override;

    private:
        char title_[64];

        char serial_number_[64];
        char integrator_name_[64];
        char plugin_name_[64];
        char generation_[64];
        char raw_formats_[64];

        char synchronization_mode_[64];

        void* texture_;
        char output_directory_path_[256];

        int bias_diff_off_;
        int bias_diff_on_;
        int bias_fo_;
        int bias_hpf_;
        int bias_refr_;

        int ext_trigger_event_count_;
    };

    class PropheseeLauncher : public ILauncher {
    public:
        PropheseeLauncher();

    private:
        struct PropheseeCamera {
            std::string serial_number;
            std::string integrator_name;
            std::string plugin_name;

            std::vector<std::string> synchronization_mode_options;
            std::string synchronization_mode;
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
        std::map<std::string, PropheseeCamera> prophesee_cameras_;
    };

} // Cocapture

#endif //COCAPTURE_PROPHESEE_LAUNCHER_H
#endif //ENABLE_METAVISION_SDK