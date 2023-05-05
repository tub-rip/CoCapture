#ifndef COCAPTURE_LAUNCHER_INTERFACE_H
#define COCAPTURE_LAUNCHER_INTERFACE_H

#include <string>
#include <vector>

#include "camera.h"
#include "camera_interface.h"

namespace Cocapture {

    class IModel {
    public:
        IModel(int id, int type) {
            Id = id;
            Type = type;
        }

    public:
        virtual void* GetRef(int specifier) = 0;
        virtual void SetRef(int specifier, void* ref) = 0;

    public:
        int Id;
        int Type;
    };

    class ILauncher {
    public:
        virtual std::vector<std::string> GetCameraIdentifiers() = 0;
        virtual std::string GetCameraInformation(std::string identifier, int info) { return {}; }
        virtual std::vector<std::string> GetCameraLaunchOptions(std::string identifier, int setting) { return {}; }
        virtual void SetCameraLaunchOption(std::string identifier, int setting, std::string option) {}
        virtual std::unique_ptr<ICamera> GetCamera(std::string identifier) { return {}; }
        virtual std::unique_ptr<IModel> GetModel(std::unique_ptr<ICamera>& camera, int id) { return {}; }
        virtual void Reset() {}

    public:
        int Type {CAM_TYPE_NONE};
    };

} // Cocapture

#endif //COCAPTURE_LAUNCHER_INTERFACE_H