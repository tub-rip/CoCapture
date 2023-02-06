#pragma once

#include "Base.h"

namespace Gui {

    class BaslerWrapper : public Base {
    public:
        BaslerWrapper() {}
        ~BaslerWrapper() {}

    public:
        void setupBasler(Parameters appParams, int id) {
            this->cam = new camera::BaslerCamera(BaslerParams(appParams));
            this->type = BASLER;
            this->id = id;
            setupCamera();
        }

        void updateValues() override {
            camera::BaslerCamera* bCam = (camera::BaslerCamera*) cam;

            // Update Basler camera values
            bCam->set_exposure_time(exposureTime);
            bCam->set_trigger_mode(triggerMode);
        }

    public:
        int* getExposureTimeRef() { return &exposureTime; }
        bool* getTriggerModeRef() { return &triggerMode; }

        void setCapturedFrames(int cpt) { capturedFrames = cpt; }
        int getCapturedFrames() { return capturedFrames; }

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
        bool triggerMode = false;
        int capturedFrames = 0;
    };

}