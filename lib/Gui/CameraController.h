#pragma once

#include "../camera/basler_camera.h"
#include "../camera/prophesee_cam.h"

#include "BaslerWrapper.h"
#include "PropheseeWrapper.h"

namespace Gui {

    class CameraController {
    public:
        CameraController() {}
        ~CameraController() {}

    public:
        void setupController(Parameters appParams);
        void setupCameras();
        void updateCameras();
        void cleanupCameras();

    public:
        int getNumCams() { return numCams; }
        std::vector<Base> getCams() { return cams; }

        int getLastCam(std::string type) {
            int lastCamIdx = -1;
            for(int i = numCams - 1; i >= 0; i--) {
                std::string t = appParams.camera_types[i];
                if(t == type) { lastCamIdx = i; break; }
            }
            return lastCamIdx;
        }

    private:
        Parameters appParams;
        int numCams;

        std::vector<Base> cams;
    };

} // Gui