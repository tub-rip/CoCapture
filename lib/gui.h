#ifndef RIP_COCAPTURE_GUI_GUI_H
#define RIP_COCAPTURE_GUI_GUI_H

#include <string>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "SDL2/SDL.h"
#include <SDL_opengl.h>

#include "camera/base.h"

namespace gui {
    struct window_settings {
        std::string title;
        int x, y, w, h;
        int flags;
    };

    struct color_settings {
        ImVec4 clear_color;
    };

    class gui {
        public:
             gui();
            ~gui();

            SDL_Window* get_window();
            SDL_GLContext  get_context();
            window_settings get_window_settings();
            color_settings get_color_settings();

            void create_window(window_settings ws, color_settings cs);
            void start_frame();
            void render();
            void draw(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, int idx);
            void handle_event(bool* done);

            void setup_texture_cam(GLuint* tex,
                                   int w, int h);

            void update_texture_cam(GLuint tex, void* data,
                                    int w, int h);

            void display_cams(GLuint* tex, std::vector<camera::Base *> cameras);

        private:
            SDL_Window* window;
            SDL_GLContext gl_context;

            window_settings ws;
            color_settings cs;
    };
} // gui

#endif //RIP_COCAPTURE_GUI_GUI_H