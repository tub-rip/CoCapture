#ifdef ENABLE_DEAR_IMGUI
#ifndef COCAPTURE_GUI_H
#define COCAPTURE_GUI_H

#include <string>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "SDL2/SDL.h"
#include <SDL_opengl.h>

namespace Cocapture {

    struct Window {
        SDL_Window* window;
        SDL_GLContext gl_context;
    };

    void Init();
    void Exit();
    Window* CreateWindow(const std::string title, const int width, const int height);
    void DestroyWindow(Window* win);
    void ResizeWindow(Window* win, const int width, const int height);
    void NewFrame(Window* win);
    void EndFrame(Window* win, const ImVec4 clear_color);
    void HandleEvent(Window* win, bool& done);
    void GenerateTexture(GLuint* texture, const int width, const int height);
    void UpdateTexture(GLuint* texture, const void* data, const int width, const int height);
    void DeleteTexture(GLuint* texture);
    void ShowViz(GLuint* texture, const ImVec2 pos, const ImVec2 size, const char* title, const bool show = true);
    ImVec2 ResizeViz(const int width, const int height, const ImVec2 available_space);

} // Cocapture

#endif //COCAPTURE_GUI_H
#endif //ENABLE_DEAR_IMGUI