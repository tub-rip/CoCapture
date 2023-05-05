#ifdef ENABLE_DEAR_IMGUI

#include "gui.h"

namespace Cocapture {

    void Init() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    void Exit() {
        ImGui::DestroyContext();
        SDL_Quit();
    }

    Window* CreateWindow(const std::string title, const int width, const int height) {
        Window* win = new Window;
        win->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
        win->gl_context = SDL_GL_CreateContext(win->window);
        SDL_GL_MakeCurrent(win->window, win->gl_context);
        SDL_GL_SetSwapInterval(1);
        ImGui_ImplSDL2_InitForOpenGL(win->window, win->gl_context);
        ImGui_ImplOpenGL3_Init("#version 130");
        return win;
    }

    void DestroyWindow(Window* win) {
        SDL_GL_DeleteContext(win->gl_context);
        SDL_DestroyWindow(win->window);
        delete win;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }

    void ResizeWindow(Window* win, const int width, const int height) {
        SDL_HideWindow(win->window);
        SDL_SetWindowSize(win->window, width, height);
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = width;
        io.DisplaySize.y = height;
        SDL_SetWindowPosition(win->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(win->window);
    }

    void NewFrame(Window* win) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(win->window);
        ImGui::NewFrame();
    }

    void EndFrame(Window* win, const ImVec4 clear_color) {
        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(win->window, win->gl_context);
        ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(win->window);
    }

    void HandleEvent(Window* win, bool& done) {
        SDL_Event e;

        while(SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if(e.type == SDL_QUIT) { done = true; }
            if(e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_CLOSE &&
            e.window.windowID == SDL_GetWindowID(win->window)) { done = true; }
        }
    }

    void GenerateTexture(GLuint *texture, const int width, const int height) {
        glGenTextures(1, texture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *texture);

        uint8_t sample[width * height * 3];
        memset(sample, 0, width * height * 3);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     (void*) sample);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void UpdateTexture(GLuint* texture, const void *data, const int width, const int height) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        width, height, GL_RGB, GL_UNSIGNED_BYTE,
                        (void*) data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void DeleteTexture(GLuint* texture) {
        glDeleteTextures(1, texture);
        delete texture;
    }

    void ShowViz(GLuint* texture, const ImVec2 pos, const ImVec2 size, const char* title, const bool show) {
        if(!show) { return; }

        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, {0.5f, 0.5f});

        ImGui::Begin(title, nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

        float aspect_ratio = size.x / size.y;
        ImVec2 current_size = ImGui::GetWindowSize();
        if(current_size.x / current_size.y != aspect_ratio) {
            float size_scale_y = current_size.x / (current_size.y * aspect_ratio);
            ImGui::SetWindowSize({current_size.x, size_scale_y * current_size.y});
        }

        ImGui::Image((void*) (intptr_t) *texture, ImGui::GetWindowSize());

        ImGui::End();
    }

    ImVec2 ResizeViz(const int width, const int height, const ImVec2 available_space) {
        float space_scale_y = 0.7f;
        return {(float(width)/height) * (space_scale_y * available_space.y),
                space_scale_y * available_space.y};
    }

} // Cocapture

#endif //ENABLE_DEAR_IMGUI