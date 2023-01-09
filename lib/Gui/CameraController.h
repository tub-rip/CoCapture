#pragma once

#include "../camera/basler_camera.h"
#include "../camera/prophesee_cam.h"

#include "BaslerWrapper.h"

namespace Gui {

    class CameraController {
    public:
        CameraController() {}
        ~CameraController() {}

    public:
        void setupController(Parameters appParams);
        void setupCameras();
        void updateCameras();

    public:
        int getNumCams() { return numCams; }
        std::vector<Base> getCams() { return cams; }

    private:
        Parameters appParams;
        int numCams;

        std::vector<Base> cams;
    };

} // Gui