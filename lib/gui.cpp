#include "gui.h"

namespace gui {
    gui::gui() {
        SDL_Init(SDL_INIT_VIDEO);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }

    gui::~gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void gui::create_window(window_settings ws, color_settings cs) {
        this->ws = ws;
        this->cs = cs;

        window = SDL_CreateWindow(ws.title.c_str(),
                                  ws.x, ws.y,
                                  ws.w, ws.h,
                                  ws.flags);

        gl_context = SDL_GL_CreateContext(window);

        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1);

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);

        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void gui::start_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void gui::render() {
        ImGui::Render();

        ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0,
                   (int) io.DisplaySize.x, (int) io.DisplaySize.y);

        glClearColor(cs.clear_color.x * cs.clear_color.w,
                     cs.clear_color.y * cs.clear_color.w,
                     cs.clear_color.z * cs.clear_color.w,
                     cs.clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    void gui::draw(GLuint tex, ImVec2 pos, ImVec2 size, int idx) {
        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));

        ImGui::Begin(std::to_string(idx).c_str(), NULL,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Image((void*) (intptr_t) tex, ImGui::GetWindowSize());
        ImGui::End();
    }

    void gui::handle_event(bool* done) {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);

            if(e.type == SDL_QUIT)
                *done = true;
        }
    }

    void gui::setup_texture_cam(GLuint* tex,
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
                     (void*) sample);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void gui::update_texture_cam(GLuint tex, void* mat_data,
                                 int w, int h) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        w, h, GL_RGB, GL_UNSIGNED_BYTE,
                        mat_data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void gui::display_cams(GLuint* tex, std::vector<camera::Base *> cameras) {
        int n_cameras = cameras.size();
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 pos;
        ImVec2 size;

        if(n_cameras == 1) {
            size = ImVec2(io.DisplaySize.x, io.DisplaySize.y);
            pos = ImVec2(0.0f, 0.0f);

            draw(*tex, pos, size, 0);
        }

        else if(n_cameras == 2) {
            size = ImVec2(io.DisplaySize.x, io.DisplaySize.y / 2);
            ImVec2 poses[2] = { ImVec2(0.0f, 0.0f), ImVec2(0.0f, size.y) };

            for(int i = 0; i < 2; i++)
                draw(tex[i], poses[i], size, i);
        }

        else {
            int n_horizontal_grids = (n_cameras % 2 == 0 ? n_cameras : n_cameras + 1) / 2;
            int x_offset; int y_offset;

            for(int i = 0; i < n_cameras; i++) {
                x_offset = i % (n_horizontal_grids);
                y_offset = i < (n_horizontal_grids) ? 0 : 1;

                size = ImVec2(io.DisplaySize.x / n_horizontal_grids,
                              io.DisplaySize.y / 2);

                pos = ImVec2(x_offset * size.x, y_offset * size.y);

                draw(tex[i], pos, size, i);
            }
        }
    }

    SDL_Window* gui::get_window() {
        return window;
    }

    SDL_GLContext gui::get_context() {
        return gl_context;
    }

    window_settings gui::get_window_settings() {
        return ws;
    }

    color_settings gui::get_color_settings() {
        return cs;
    }
} // gui