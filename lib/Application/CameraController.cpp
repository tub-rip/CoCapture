#include "CameraController.h"

namespace Gui {

    void CameraController::setupController(Parameters appParams) {
        Pylon::PylonInitialize();

        this->appParams = appParams;
        this->numCams = appParams.camera_types.size();
    }

    void CameraController::setupCameras() {
        Base* camRef;
        int bCounter = 0;
        int pCounter = 0;

        for(int i = 0; i < numCams; i++) {
            std::string type = appParams.camera_types[i];
            int lastPropheseeIdx = getLastCam(PROPHESEE);

            // Basler
            if(type == BASLER) {
                BaslerWrapper* bCam = new BaslerWrapper();

                bCam->setupBasler(appParams, bCounter++);
                camRef = bCam;
            }

            // Prophesee
            else if(type == PROPHESEE) {
                PropheseeWrapper* pCam = new PropheseeWrapper();

                std::string mode = SLAVE;
                // Sets all but the last Prophesee camera to slave mode
                if(i == lastPropheseeIdx) { mode = MASTER; }

                pCam->setupProphesee(appParams, mode, i, pCounter++);
                camRef = pCam;
            }

            camRefs.push_back(camRef);
        }
    }

    void CameraController::updateCameras() {
        for(Base* cam : camRefs) {
            cam->updateCamera();
        }
    }

    void CameraController::cleanupCameras() {
        for(Base* cam : camRefs) {
            delete cam->getActual();
            delete cam;
        }
    }

}
