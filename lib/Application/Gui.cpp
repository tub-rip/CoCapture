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

}