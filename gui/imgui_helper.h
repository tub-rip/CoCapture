#ifndef RIP_COCAPTURE_GUI_IMGUI_HELPER_H
#define RIP_COCAPTURE_GUI_IMGUI_HELPER_H

#include <string>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "SDL2/SDL.h"
#include <SDL_opengl.h>

namespace rcg::gui {

    void Initialize();
    void Terminate();
    void CreateWindow(const char* title, int width, int height);
    void DestroyWindow();
    void NewFrame();
    void EndFrame();
    void HandleEvent(bool& done);
    void GenerateTextureRGB8(GLuint* texture_id_ref, int image_width, int image_height);
    void UpdateTextureRGB8(GLuint* texture_id_ref, unsigned char* image_data, int image_width, int image_height);
    void DeleteTexture(GLuint* texture_id_ref);
    void DisplayImage(GLuint* texture_id_ref, const char* title, int image_width, int image_height, bool* show = NULL);

} // rcg::gui

#endif //RIP_COCAPTURE_GUI_IMGUI_HELPER_H