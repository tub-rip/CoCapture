#include "gui.h"

namespace gui {
    gui::gui(window_settings ws, color_settings cs) {
        this->ws = ws;
        this->cs = cs;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

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

    gui::~gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
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

        if(n_cameras == 1) {
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_None);
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f),
                                    ImGuiCond_Always, ImVec2(0.0f, 0.0f));

            ImGui::Begin(std::to_string(0).c_str());
            ImGui::Image((void*) (intptr_t) tex[0], ImGui::GetWindowSize());
            ImGui::End();
        } else if(n_cameras == 2) {
            float grid_width = io.DisplaySize.x / 2;
            float grid_height = io.DisplaySize.y;

            ImGui::SetNextWindowSize(ImVec2(grid_width, grid_height), ImGuiCond_None);
            ImGui::SetNextWindowPos(ImVec2(0 * grid_width, 0 * grid_height),
                                    ImGuiCond_Always, ImVec2(0.0f, 0.0f));

            ImGui::Begin(std::to_string(0).c_str());
            ImGui::Image((void*) (intptr_t) tex[0], ImGui::GetWindowSize());
            ImGui::End();

            ImGui::SetNextWindowSize(ImVec2(grid_width, grid_height), ImGuiCond_None);
            ImGui::SetNextWindowPos(ImVec2(1 * grid_width, 0 * grid_height),
                                    ImGuiCond_Always, ImVec2(0.0f, 0.0f));

            ImGui::Begin(std::to_string(1).c_str());
            ImGui::Image((void*) (intptr_t) tex[1], ImGui::GetWindowSize());
            ImGui::End();
        } else {
            int n_horizontal_grids = (n_cameras % 2 == 0 ? n_cameras : n_cameras + 1) / 2;

            int x_offset; int y_offset;
            float grid_width; float grid_height;
            for(int i = 0; i < n_cameras; i++) {
                x_offset = i % (n_horizontal_grids);
                y_offset = i < (n_horizontal_grids) ? 0 : 1;

                grid_width = io.DisplaySize.x / n_horizontal_grids;
                grid_height = io.DisplaySize.y / 2 ;

                ImGui::SetNextWindowSize(ImVec2(grid_width, grid_height), ImGuiCond_None);
                ImGui::SetNextWindowPos(ImVec2(x_offset * grid_width, y_offset * grid_height),
                                        ImGuiCond_Always, ImVec2(0.0f, 0.0f));

                ImGui::Begin(std::to_string(i).c_str());
                ImGui::Image((void*) (intptr_t) tex[i], ImGui::GetWindowSize());
                ImGui::End();
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