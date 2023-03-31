#pragma once

#include "../camera/basler_camera.h"
#include "../camera/prophesee_cam.h"
#include "../camera/realsense_cam.h"

#include "Camera/BaslerWrapper.h"
#include "Camera/PropheseeWrapper.h"
#include "Camera/RealSenseWrapper.h"

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
        std::vector<Base*> getCamRefs() { return camRefs; }

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

        rs2::context ctx;
        std::vector<Base*> camRefs;
    };

}