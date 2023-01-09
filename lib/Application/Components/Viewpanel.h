#pragma once

#include "Component.h"
#include "../Camera/Base.h"

namespace Gui {

    class Viewpanel : public Component {
    public:
        Viewpanel(float gridScaleY, std::vector<Base> cams) {
            this->cams = cams;
            this->numCams = cams.size();
            this->textures = new GLuint[numCams];
            this->gridScaleY = gridScaleY;
            setupTextures();
        }

        ~Viewpanel() {
            delete[] textures;
        }

    public:
        void show() override;

    public:

        void drawToWindow(GLuint tex,
                          ImVec2 pos, ImVec2 size, ImVec2 pivot,
                          std::string title);

        ImVec2 calcWindowSize(int w, int h, ImVec2 gridSize);
        void demoViewpanel();

    public:
        void setupTextures();
        void updateTextures();
        void setupTexture(GLuint* tex,
                          int w, int h);
        void updateTexture(GLuint tex, void* data,
                           int w, int h);

    private:
        int numCams;
        std::vector<Base> cams;
        GLuint* textures;
        float gridScaleY;
    };

}