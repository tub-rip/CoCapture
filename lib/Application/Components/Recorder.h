#pragma once

#include "Component.h"
#include "../Camera/PropheseeWrapper.h"
#include "../Camera/BaslerWrapper.h"

#include <iostream>
#include <ctime>
#include <boost/filesystem.hpp>

namespace Gui {

    class Recorder : public Component {
    public:
        Recorder(std::vector<Base*> camRefs, std::string outputDir) {
            this->camRefs = camRefs;

            this->workDir = outputDir;
            if(workDir.back() == '/') { workDir.pop_back(); }
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
        void sanityCheck();

    private:
        std::vector<Base*> camRefs;
        std::vector<int> contentCount;

        std::string workDir = ".";
        std::string currentTargetDir = "";

        std::string status = "Not currently recording.";
        bool recording = false;
    };

}