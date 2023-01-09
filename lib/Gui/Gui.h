#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "SDL2/SDL.h"
#include <SDL_opengl.h>

#include "Constants.h"

// wajdhwjlakmdö,jwakdwa
#include "../camera/base.h"

namespace Gui {

    class Gui {
    public:
        Gui() {}
        ~Gui() {}

    public:
        void setupGui();
        void cleanupGui();
        void startFrame();
        void endFrameAndFinalizeRender();
        void handleEvent(bool* done);

        // sajd.w,madwa
        void setup_texture_cam(GLuint* tex,
                               int w, int h);

        void update_texture_cam(GLuint tex, void* data,
                                int w, int h);

        void draw(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, int idx);
        void display_cams(GLuint* tex, std::vector<camera::Base *> cameras);

    private:
        SDL_Window* mainWindow;
        SDL_DisplayMode displayMode;
        SDL_GLContext glContext;
    };

}
