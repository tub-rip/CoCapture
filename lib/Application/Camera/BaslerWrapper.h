#pragma once

#include "Base.h"

namespace Gui {

    class BaslerWrapper : public Base {
    public:
        BaslerWrapper() {}
        ~BaslerWrapper() {}

    public:
        void setupBasler(Parameters appParams) {
            cam = new camera::BaslerCamera(BaslerParams(appParams));
            type = BASLER;
            setupCamera();
        }

        void updateValues() override {
            camera::BaslerCamera* bCam = (camera::BaslerCamera*) cam;

            // Update Basler camera values
            bCam->set_exposure_time(exposureTime);
        }

    public:
        int* getExposureTimeRef() { return &exposureTime; }

        void startRecording(std::string path) override {
            camera::BaslerCamera* bCam = (camera::BaslerCamera*) cam;
            bCam->startup_recorder(path, BASLER_RECORDING_FPS);
        }

        void stopRecording() override {
            camera::BaslerCamera* bCam = (camera::BaslerCamera*) cam;
            bCam->stop_recording();
        }

    private:
        int exposureTime = BASLER_EXPOSURE_DEFAULT;
    };

}