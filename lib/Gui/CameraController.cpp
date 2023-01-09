#include "CameraController.h"

namespace Gui {

    void CameraController::setupController(Parameters appParams) {
        Pylon::PylonInitialize();

        this->appParams = appParams;
        this->numCams = appParams.camera_types.size();
    }

    void CameraController::setupCameras() {
        Base cam;

        for(int i = 0; i < numCams; i++) {
            std::string type = appParams.camera_types[i];
            int lastPropheseeIdx = getLastCam(PROPHESEE);

            // Basler
            if(type == BASLER) {
                BaslerWrapper bCam = BaslerWrapper();
                bCam.setupBasler(appParams);
                cam = bCam;
            }

            // Prophesee
            else if(type == PROPHESEE) {
                PropheseeWrapper pCam = PropheseeWrapper();

                std::string mode = SLAVE;
                // Sets all but the last Prophesee camera to slave mode
                if(i == lastPropheseeIdx || !appParams.record) { mode = MASTER; }

                pCam.setupProphesee(appParams, mode, i);
                cam = pCam;
            }

            cams.push_back(cam);
        }
    }

    void CameraController::updateCameras() {
        for(Base cam : cams) {
            cam.updateCamera();
        }
    }

    void CameraController::cleanupCameras() {
        for(Base cam : cams) {
            delete cam.getActual();
        }
    }

} // Gui
