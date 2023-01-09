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

            if(type == BASLER) {
                BaslerWrapper bCam = BaslerWrapper();
                bCam.setupBasler(appParams);
                cam = bCam;
            }

            cams.push_back(cam);
        }
    }

    void CameraController::updateCameras() {
        for(Base cam : cams) {
            cam.updateCamera();
        }
    }

} // Gui
