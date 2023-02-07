#pragma once

#include "Component.h"
#include "../Camera/PropheseeWrapper.h"
#include "../Camera/BaslerWrapper.h"
#include "../Camera/RealSenseWrapper.h"

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

            resetContentCount();
        }

        ~Recorder() {}

    public:
        void show() override;

    public:
        void demoRecorder();

    public:
        std::string getCurrTimeRootDirStr();

        void prepareRecording(std::string rootDir);
        void cleanupRecording();

        void startRecording(std::string rootDir);
        void stopRecording();

        void resetContentCount();
        void sanityCheck();

    private:
        std::vector<Base*> camRefs;

        std::string workDir = ".";
        std::string currentTargetDir = "";

        std::string status = "Not currently recording.";
        bool recording = false;
    };

}