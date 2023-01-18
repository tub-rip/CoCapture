#include "Sidepanel.h"

namespace Gui {

    void Sidepanel::demoSidepanel() {
        ImGuiIO &io = ImGui::GetIO();

        ImVec2 demoShowSpaceScale = ImVec2(0.25f, 0.75f);
        ImVec2 demoShowSpace = ImVec2(io.DisplaySize.x * demoShowSpaceScale.x,
                                      io.DisplaySize.y * demoShowSpaceScale.y);

        ImVec2 offset, windowPos, windowSize;

        offset = ImVec2(io.DisplaySize.x * (1 - demoShowSpaceScale.x),
                        io.DisplaySize.y * (0.9975f - demoShowSpaceScale.y));

        windowPos = ImVec2(demoShowSpace.x / 2 + offset.x,
                           demoShowSpace.y / 2 + offset.y);

        windowSize = ImVec2(demoShowSpace.x - 0.0275f * demoShowSpace.x,
                            demoShowSpace.y - 0.0195f * demoShowSpace.y);

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(windowPos, 0, ImVec2(0.5f, 0.5f));

        ImGui::Begin("Sidepanel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        int i = 0;
        for(Base* cam : actualCams) {

            // Basler camera
            if(cam->getType() == BASLER) {
                BaslerWrapper* bCam = (BaslerWrapper*) cam;
                ImGui::TextUnformatted((bCam->getType() + " " + std::to_string(i)).c_str());

                ImGui::SliderInt((BASLER_EXPOSURE_LABEL + "##").c_str(), bCam->getExposureTimeRef(),
                                 BASLER_EXPOSURE_MIN, BASLER_EXPOSURE_MAX);
                ImGui::Spacing();
            }

                // Prophesee camera
            else if(cam->getType() == PROPHESEE) {
                PropheseeWrapper* pCam = (PropheseeWrapper*) cam;
                ImGui::TextUnformatted((pCam->getType() + " " + std::to_string(i)).c_str());

                ImGui::SliderInt("Off event th. ##", pCam->getBiasDiffOffRef(),
                                 PROPHESEE_BIAS_DIFF_OFF_MIN, PROPHESEE_BIAS_DIFF_OFF_MAX);
                ImGui::Spacing();

                ImGui::SliderInt("On event th. ##", pCam->getBiasDiffOnRef(),
                                 PROPHESEE_BIAS_DIFF_ON_MIN, PROPHESEE_BIAS_DIFF_ON_MAX);
                ImGui::Spacing();

                ImGui::SliderInt("Low pass filter ##", pCam->getBiasFoRef(),
                                 PROPHESEE_BIAS_FO_MIN, PROPHESEE_BIAS_FO_MAX);
                ImGui::Spacing();

                ImGui::SliderInt("High pass filter ##", pCam->getBiasHpfRef(),
                                 PROPHESEE_BIAS_HPF_MIN, PROPHESEE_BIAS_HPF_MAX);
                ImGui::Spacing();

                ImGui::SliderInt("Refractory period ##", pCam->getBiasRefrRef(),
                                 PROPHESEE_BIAS_REFR_MIN, PROPHESEE_BIAS_REFR_MAX);
                ImGui::Spacing();
            }

            ImGui::Spacing();

            i++;
        }

        ImGui::End();
    }

    void Sidepanel::show() {
        demoSidepanel();
    };

}