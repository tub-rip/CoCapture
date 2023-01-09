#include "Gui.h"

namespace Gui {

    void Gui::setupGui() {
        SDL_Init(BE_INIT_SUBSYSTEMS);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        SDL_GetCurrentDisplayMode(BE_DISPLAY_INDEX, &displayMode);
        mainWindow = SDL_CreateWindow(BE_WINDOW_TITLE.c_str(),
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      displayMode.w,
                                      displayMode.h,
                                      BE_WINDOW_FLAGS);

        glContext = SDL_GL_CreateContext(mainWindow);
        SDL_GL_MakeCurrent(mainWindow, glContext);

        // VSync
        SDL_GL_SetSwapInterval(BE_SWAP_INTERVAL);

        ImGui_ImplSDL2_InitForOpenGL(mainWindow, glContext);
        ImGui_ImplOpenGL3_Init(BE_GLSL_VERSION.c_str());
    }

    void Gui::cleanupGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
    }

    void Gui::startFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void Gui::endFrameAndFinalizeRender() {
        ImGui::Render();

        ImGuiIO &io = ImGui::GetIO();
        glViewport(0, 0,
                   (int) io.DisplaySize.x,
                   (int) io.DisplaySize.y);

        glClearColor(BE_CLEAR_COLORS.x,
                     BE_CLEAR_COLORS.y,
                     BE_CLEAR_COLORS.z,
                     BE_CLEAR_COLORS.w);
        glClear(BE_BUFFER_FLAGS);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(mainWindow);
    }

    void Gui::handleEvent(bool* done) {
        SDL_Event e;

        while(SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);

            if(e.type == SDL_QUIT) { *done = true; }
        }
    }

    // sa,nndwlkjakhdjwakdwa
    void Gui::setup_texture_cam(GLuint* tex,
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

    void Gui::update_texture_cam(GLuint tex, void* mat_data,
                                 int w, int h) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        w, h, GL_RGB, GL_UNSIGNED_BYTE,
                        mat_data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Gui::draw(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, int idx) {
        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, pivot);

        ImGui::Begin(std::to_string(idx).c_str(), NULL,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Image((void*) (intptr_t) tex, ImGui::GetWindowSize());
        ImGui::End();
    }

    void Gui::display_cams(GLuint* tex, std::vector<camera::Base *> cameras) {
        int n_cameras = cameras.size();
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 grid_size;
        ImVec2 pos;
        ImVec2 size;

        float scale = 0.9f;

        if(n_cameras == 1) {
            camera::Base* cam = cameras[0];

            grid_size = ImVec2(io.DisplaySize.x, io.DisplaySize.y);

            size = ImVec2(((float) cam->get_width() / cam->get_height()) * (scale * grid_size.y),
                          scale * grid_size.y);

            pos = ImVec2(0.5 * grid_size.x,
                         0.5 * grid_size.y);

            draw(*tex, pos, size, ImVec2(0.5f, 0.5f), 0);
        }

        else {
            int n_horizontal_grids = (n_cameras % 2 == 0 ? n_cameras : n_cameras + 1) / 2;
            grid_size = ImVec2(io.DisplaySize.x / n_horizontal_grids,
                               io.DisplaySize.y / 2);

            int x_offset;
            int y_offset;

            for(int i = 0; i < n_cameras; i++) {
                camera::Base* cam = cameras[i];

                x_offset = i % (n_horizontal_grids);
                y_offset = i < (n_horizontal_grids) ? 0 : 1;

                size = ImVec2(((float) cam->get_width() / cam->get_height()) * (scale * grid_size.y),
                              scale * grid_size.y);

                pos = ImVec2(x_offset * grid_size.x + 0.5 * grid_size.x,
                             y_offset * grid_size.y + 0.5 * grid_size.y);

                draw(tex[i], pos, size, ImVec2(0.5f, 0.5f), i);
            }
        }
    }

}