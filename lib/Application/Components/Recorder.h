#pragma once

#include "Component.h"
#include "../Camera/PropheseeWrapper.h"

#include <iostream>
#include <ctime>
#include <boost/filesystem.hpp>

namespace Gui {

    class Recorder : public Component {
    public:
        Recorder(std::vector<Base*> camRefs) {
            this->camRefs = camRefs;
        }

        ~Recorder() {}

    public:
        void show() override;

    public:
        void demoRecorder();

    public:
        std::string getCurrTimeRootDirStr();
        void makeCameraSubDirsAndRecord(std::string rootDir);
        void stopRecording();

    private:
        std::vector<Base*> camRefs;
        std::string workDir = ".";
        std::string status = "Not currently recording.";

        bool recording = false;
    };

}