#include "Viewpanel.h"

namespace Gui {

    void Viewpanel::setupTextures() {
        for(int i = 0; i < numCams; i++) {
            Base* cam = actualCams[i];
            setupTexture(textures + i, cam->getWidth(), cam->getHeight());
        }
    }

    void Viewpanel::updateTextures() {
        for(int i = 0; i < numCams; i++) {
            Base* cam = actualCams[i];
            if(!cam->getDisplay().empty()) {
                updateTexture(*(textures + i), cam->getDisplay().data, cam->getWidth(), cam->getHeight());
            }
        }
    }

    void Viewpanel::setupTexture(GLuint *tex,
                                 int w, int h) {
        glGenTextures(1, tex);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *tex);

        uint8_t sample[w * h * 3];
        memset(sample, 0, w * h * 3);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                     w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     (void *) sample);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Viewpanel::updateTexture(GLuint tex, void *mat_data,
                                   int w, int h) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        w, h, GL_RGB, GL_UNSIGNED_BYTE,
                        (void*) mat_data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ImVec2 Viewpanel::calcWindowSize(int w, int h, ImVec2 gridSize) {
        return ImVec2((float(w) / h) * (gridScaleY * gridSize.y),
                      gridScaleY * gridSize.y);
    }

    void Viewpanel::drawToWindow(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, std::string title) {
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos(pos, 0, pivot);

        ImGui::Begin(title.c_str(), NULL,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Image((void*) (intptr_t) tex, ImGui::GetWindowSize());
        ImGui::End();
    }

    // Demo the viewpanel
    void Viewpanel::demoViewpanel() {
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 gridSize, displayPos, displaySize;

        ImVec2 demoShowSpaceScale = ImVec2(0.75f, 1.0f);
        ImVec2 demoShowSpace = ImVec2(io.DisplaySize.x * demoShowSpaceScale.x,
                                      io.DisplaySize.y * demoShowSpaceScale.y);

        // For a single camera
        if(numCams == 1) {
            Base* cam = actualCams[0];
            gridSize = demoShowSpace;

            displaySize = calcWindowSize(cam->getWidth(), cam->getHeight(), gridSize);
            displayPos = ImVec2(0.5f * gridSize.x,
                                0.5f * gridSize.y);

            drawToWindow(*textures, displayPos, displaySize,
                         ImVec2(0.5f, 0.5f), "View " + std::to_string(0));
        }

        else {
            int nHorizontalGrids = (numCams % 2 == 0 ? numCams : numCams + 1) / 2;
            gridSize = ImVec2(demoShowSpace.x / nHorizontalGrids,
                              demoShowSpace.y / 2);

            int xOffset, yOffset;
            for(int i = 0; i < numCams; i++) {
                Base* cam = actualCams[i];

                xOffset = i % nHorizontalGrids;
                yOffset = i < nHorizontalGrids ? 0 : 1;

                displaySize = calcWindowSize(cam->getWidth(), cam->getHeight(), gridSize);
                displayPos = ImVec2(xOffset * gridSize.x + 0.5f * gridSize.x,
                                    yOffset * gridSize.y + 0.5f * gridSize.y);

                drawToWindow(textures[i], displayPos, displaySize,
                             ImVec2(0.5f, 0.5f), cam->getType() + " " + std::to_string(i));
            }
        }
    }

    void Viewpanel::show() {
        updateTextures();
        demoViewpanel();
    }

}