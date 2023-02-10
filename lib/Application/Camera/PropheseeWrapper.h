#pragma once

#include "Base.h"

namespace Gui {

    class PropheseeWrapper : public Base {
    public:
        PropheseeWrapper() {}
        ~PropheseeWrapper() {}

    public:
        void setupProphesee(Parameters appParams, std::string mode, int index, int id) {
            this->cam = new camera::PropheseeCam(PropheseeParams(appParams, mode, index));
            this->type = PROPHESEE;
            this->id = id;
            setupCamera();
        }

        void updateValues() override {
            camera::PropheseeCam* pCam = (camera::PropheseeCam*) cam;

            // Update Prophesee camera values
            pCam->set_bias_value(PROPHESEE_BIAS_FO_LABEL, biasFo);
            pCam->set_bias_value(PROPHESEE_BIAS_DIFF_OFF_LABEL, biasDiffOff);
            pCam->set_bias_value(PROPHESEE_BIAS_DIFF_ON_LABEL, biasDiffOn);
            pCam->set_bias_value(PROPHESEE_BIAS_HPF_LABEL, biasHpf);
            pCam->set_bias_value(PROPHESEE_BIAS_REFR_LABEL, biasRefr);
        }

    public:
        int* getBiasFoRef() { return &biasFo; }
        int* getBiasDiffOffRef() { return &biasDiffOff; }
        int* getBiasDiffOnRef() { return &biasDiffOn; }
        int* getBiasHpfRef() { return &biasHpf; }
        int* getBiasRefrRef() { return &biasRefr; }

        int getExtTriggerEvts() {
            camera::PropheseeCam* pCam = (camera::PropheseeCam*) cam;
            return pCam->get_ext_trigger_evts();
        }

        void resetExtTriggerEvts() {
            camera::PropheseeCam* pCam = (camera::PropheseeCam*) cam;
            pCam->reset_ext_trigger_evts();
        }

        void setExternalTriggers(int ext) { externalTriggers = ext; }
        int getExternalTriggers() { return externalTriggers; }

        void startRecording(std::string path) override {
            camera::PropheseeCam* pCam = (camera::PropheseeCam*) cam;
            pCam->start_recording_to_path(path);
        }

        void stopRecording() override {
            camera::PropheseeCam* pCam = (camera::PropheseeCam*) cam;
            pCam->stop_recording();
        }

    private:
        int biasFo = PROPHESEE_BIAS_FO_DEFAULT;
        int biasDiffOff = PROPHESEE_BIAS_DIFF_OFF_DEFAULT;
        int biasDiffOn = PROPHESEE_BIAS_DIFF_ON_DEFAULT;
        int biasHpf = PROPHESEE_BIAS_HPF_DEFAULT;
        int biasRefr = PROPHESEE_BIAS_REFR_DEFAULT;
        int externalTriggers = 0;
    };

}