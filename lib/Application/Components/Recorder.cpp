#include "Recorder.h"

namespace Gui {

    void Recorder::resetContentCount() {
        for(Base* cam : camRefs) {
            // Prophesee camera
            if(cam->getType() == PROPHESEE) {
                PropheseeWrapper* pCam = (PropheseeWrapper*) cam;
                pCam->setExternalTriggers(0);
            }

            // Basler camera
            if(cam->getType() == BASLER) {
                BaslerWrapper* bCam = (BaslerWrapper*) cam;
                bCam->setCapturedFrames(0);
            }
        }
    }

    std::string Recorder::getCurrTimeRootDirStr() {
        time_t rawTime;
        struct tm * timeInfo;
        char buf[100];

        time(&rawTime);
        timeInfo = localtime(&rawTime);

        strftime(buf, sizeof(buf), "%y%m%d_%H%M%S", timeInfo);
        return std::string(buf);
    }

    void Recorder::makeCameraSubDirsAndRecord(std::string rootDir) {
        resetContentCount();
        currentTargetDir = workDir + "/" + rootDir;

        int i = 0;
        for(Base* cam : camRefs) {
            std::string camDir = currentTargetDir + "/" + cam->getString();
            boost::filesystem::create_directories(camDir);

            // Prophesee camera
            if(cam->getType() == PROPHESEE) {
                PropheseeWrapper* pCam = (PropheseeWrapper*) cam;
                pCam->startRecording(camDir + "/" + PROPHESEE_OUTPUT_FILENAME);
            }

            // Basler camera
            if(cam->getType() == BASLER) {
                BaslerWrapper* bCam = (BaslerWrapper*) cam;
                bCam->startRecording(camDir + "/" + BASLER_OUTPUT_FILENAME);
            }

            i++;
        }
    }

    void Recorder::stopRecording() {
        for(Base* cam : camRefs) {
            // Basler camera
            if(cam->getType() == BASLER) {
                BaslerWrapper* bCam = (BaslerWrapper*) cam;
                bCam->stopRecording();
            }

            // Prophesee camera
            if(cam->getType() == PROPHESEE) {
                PropheseeWrapper* pCam = (PropheseeWrapper*) cam;
                pCam->stopRecording();
            }
        }

        sanityCheck();
    }

    void Recorder::sanityCheck() {
        int i = 0;
        for(Base* cam : camRefs) {
            std::string camDir = currentTargetDir + "/" + cam->getString();
            std::string outputFile;

            // Prophesee camera
            if(cam->getType() == PROPHESEE) {
                PropheseeWrapper* pCam = (PropheseeWrapper*) cam;
                pCam->setExternalTriggers(pCam->getExtTriggerEvts());
                pCam->resetExtTriggerEvts();
            }

            // Basler camera
            if(cam->getType() == BASLER) {
                BaslerWrapper* bCam = (BaslerWrapper*) cam;
                cv::VideoCapture capture (camDir + "/" + BASLER_OUTPUT_FILENAME);
                bCam->setCapturedFrames(capture.get(cv::CAP_PROP_FRAME_COUNT));
            }

            i++;
        }
    }

    void Recorder::demoRecorder() {
        ImGuiIO &io = ImGui::GetIO();

        ImVec2 demoShowSpaceScale = ImVec2(0.25f, 0.25f);
        ImVec2 demoShowSpace = ImVec2(io.DisplaySize.x * demoShowSpaceScale.x,
                                      io.DisplaySize.y * demoShowSpaceScale.y);

        ImVec2 offset, windowPos, windowSize;

        offset = ImVec2(io.DisplaySize.x * (1 - demoShowSpaceScale.x),
                        io.DisplaySize.y * 0.00675f);

        windowPos = ImVec2(demoShowSpace .x / 2 + offset.x,
                           demoShowSpace .y / 2 + offset.y);

        windowSize = ImVec2(demoShowSpace .x * (1 - 0.0275f),
                            demoShowSpace .y * (1 - 0.0565f));

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(windowPos, 0, ImVec2(0.5f, 0.5f));

        ImGui::Begin("Recorder", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // Camera availability check
        bool cameraExists = camRefs.size() > 0 ? true : false;

        bool pExists = false;
        bool bExists = false;
        for(Base* cam : camRefs) {
            if(cam->getType() == PROPHESEE) { pExists = true; }
            if(cam->getType() == BASLER) { bExists = true; }
        }

        if(cameraExists) {
            ImGui::TextUnformatted("Detected camera types:");
            ImGui::Spacing();

            if(bExists) {
                ImGui::TextUnformatted(( "- " + BASLER ).c_str());
                ImGui::Spacing();
            }

            if(pExists) {
                ImGui::TextUnformatted(( "- " + PROPHESEE ).c_str());
                ImGui::Spacing();
            }

            ImGui::Spacing();

            std::string rootDir = getCurrTimeRootDirStr();

            if(ImGui::Button("Record")) {
                if(!recording) {
                    makeCameraSubDirsAndRecord(rootDir);

                    recording = true; status = "Recording in progress . .";
                }

                else if(recording) { ImGui::OpenPopup("recordingPopup"); }
            }

            if(ImGui::BeginPopup("recordingPopup")) {
                ImGui::Text("Currently recording. Proceed anyway?");
                if(ImGui::Selectable("Yes")) {
                    stopRecording();
                    makeCameraSubDirsAndRecord(rootDir);
                }
                ImGui::Selectable("No");
                ImGui::EndPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Stop")) {
                if(recording) {
                    stopRecording();
                    recording = false; status = "Recording successfully ended.";
                }
            }

        } else {
            ImGui::Text("No camera capable of recording found.");
        }

        ImGui::Spacing();

        ImGui::TextUnformatted(status.c_str());
        ImGui::Spacing();

        ImGui::End();
    }

    void Recorder::show() {
        demoRecorder();
    }

}