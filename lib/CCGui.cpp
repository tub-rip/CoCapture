#include "CCGui.h"

namespace CCGui {

    // - - - - - - - - - - - - - - - - GUI METHODS - - - - - - - - - - - - - - - - //

    // CCGui constructor, sets up application as well as imgui
    CCGui::CCGui(int argc, const char* argv[]) {
        // Initializes required SDL subsystems,
        // verify data structures and create imgui context
        SDL_Init(SDL_INIT_VIDEO);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Using SDL, creates an OpenGL window as well as an OpenGL context
        // sets up aforementioned OpenGL context for rendering to the corresponding OpenGL window
        SDL_GetCurrentDisplayMode(DISPLAY_INDEX, &container.displayMode);
        container.mainWindow = SDL_CreateWindow(APP_TITLE.c_str(),
                                                SDL_WINDOWPOS_CENTERED,
                                                SDL_WINDOWPOS_CENTERED,
                                                container.displayMode.w,
                                                container.displayMode.h,
                                                WINDOW_FLAGS);
        container.glContext = SDL_GL_CreateContext(container.mainWindow);
        SDL_GL_MakeCurrent(container.mainWindow, container.glContext);

        // Enables vsync
        SDL_GL_SetSwapInterval(1);

        // Sets up platform (SDL2) and renderer (OpenGL3) backend
        ImGui_ImplSDL2_InitForOpenGL(container.mainWindow, container.glContext);

        // Passes in the appropriate OpenGL shader
        ImGui_ImplOpenGL3_Init(GLSL_VERSION.c_str());

        // Parses command line arguments
        utils::parse_comman_line(argc, argv, container.appParameters);

        // Stores the number of connected cameras
        container.numCams = container.appParameters.camera_types.size();

        // Allocates resources for OpenGL textures
        container.glTextures = new GLuint[container.numCams];
    }

    // CCGui destructor, cleans up imgui and other allocated resources
    CCGui::~CCGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(container.glContext);
        SDL_DestroyWindow(container.mainWindow);
        SDL_Quit();

        delete[] container.glTextures;
        for(auto ccc : container.cameras) {
            delete ccc.cam;
        }
    }

    // Starts a new imgui frame
    void CCGui::startFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    // Ends imgui frame and finalizes rendering
    void CCGui::render() {
        ImGui::Render();

        // Sets viewport size
        ImGuiIO& io = ImGui::GetIO();
        glViewport(ZERO, ZERO,
                   (int) io.DisplaySize.x,
                   (int) io.DisplaySize.y);

        // Clears color buffer
        glClearColor(CLEAR_COLOR.x * CLEAR_COLOR.w,
                     CLEAR_COLOR.y * CLEAR_COLOR.w,
                     CLEAR_COLOR.z * CLEAR_COLOR.w,
                     CLEAR_COLOR.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renders data from imgui draw calls
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Uses double buffering
        SDL_GL_SwapWindow(container.mainWindow);
    }

    // Handles events
    void CCGui::handleEvent(bool* done) {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);

            if(e.type == SDL_QUIT)
                *done = true;
        }
    }

    // Sets up an OpenGL texture
    void CCGui::setupTexture(GLuint* tex, int w, int h) {
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

    // Updates an OpenGL texture
    void CCGui::updateTexture(GLuint tex, void* data, int w, int h) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        w, h, GL_RGB, GL_UNSIGNED_BYTE,
                        data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Creates an imgui window and draws the passed OpenGL texture into the window
    void CCGui::drawToWindow(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, int idx) {
        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, pivot);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WINDOW_PADDING);
        ImGui::Begin(std::to_string(idx).c_str(), NULL,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Image((void*) (intptr_t) tex, ImGui::GetWindowSize());
        ImGui::End();
        ImGui::PopStyleVar();
    }

    // NEW FEATURES WEEK 4 //
    void CCGui::writeToDisplay(std::string text, cv::Mat mat) {
        cv::putText(mat,
                    text,
                    ORIGIN,
                    FONT_FACE,
                    FONT_SCALE,
                    FONT_COLOR,
                    LINE_THICKNESS,
                    LINE_TYPE,
                    !BOTTOM_LEFT_ORIGIN);
    }

    void CCGui::displayGeneral() {
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 size = ImVec2(io.DisplaySize.x * GENERAL_SPACE_SCALE.x,
                             io.DisplaySize.y * GENERAL_SPACE_SCALE.y);

        ImVec2 pos = ImVec2(io.DisplaySize.x * (1 - GENERAL_SPACE_SCALE.x),
                            io.DisplaySize.y * 0);

        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, NO_PIVOT);
        ImGui::Begin(GENERAL_TITLE.c_str(), NULL,
                     ImGuiWindowFlags_NoCollapse|
                     ImGuiWindowFlags_NoResize|
                     ImGuiWindowFlags_NoMove);
        ImGui::End();
    }

    void CCGui::displaySettings() {
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 size = ImVec2(io.DisplaySize.x * SETTINGS_SPACE_SCALE.x,
                             io.DisplaySize.y * SETTINGS_SPACE_SCALE.y);

        ImVec2 pos = ImVec2(io.DisplaySize.x * (1 - SETTINGS_SPACE_SCALE.x),
                            io.DisplaySize.y * (GENERAL_SPACE_SCALE.y));

        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, NO_PIVOT);
        ImGui::Begin(SETTINGS_TITLE.c_str(), NULL,
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);
        ImGui::End();
    }

    // - - - - - - - - - - - - - - - - CAMERA METHODS - - - - - - - - - - - - - - - - //

    // Sets up cameras
    void CCGui::setupCameras() {
        // @TODO: Call Pylon::PylonInitialize() somewhere else
        Pylon::PylonInitialize();

        camera::Base* c;

        // Find the index of the last Prophesee camera
        int lastPropheseeIdx = -1;
        for(int i = container.numCams - 1; i >= 0; i--) {
            const auto type = container.appParameters.camera_types[i];
            if(type == PROPHESEE) { lastPropheseeIdx = i; break; }
        }

        for(int i = 0; i < container.numCams; i++) {
            const auto type = container.appParameters.camera_types[i];

            // PROPHESEE
            if(type == PROPHESEE) {
                // Sets all but the last Prophesee camera to slave mode
                std::string mode = SLAVE;
                if(i == lastPropheseeIdx || !container.appParameters.record) { mode = MASTER; }
                c = new camera::PropheseeCam(PropheseeParams(container.appParameters, mode, i));
            }

            // BASLER
            else if(type == BASLER) { c = new camera::BaslerCamera(BaslerParams(container.appParameters)); }

            // Declares and defines variables for the CCCamera wrapper struct
            camera::Base* cam = c;
            cam->setup_camera();

            std::string camType = type;
            int camIdx = i;

            GLuint* camTex = container.glTextures + i;
            setupTexture(camTex, cam->get_width(), cam->get_height());

            cv::Mat camMat = cv::Mat(cam->get_height(), cam->get_width(),
                                      MAT_TYPE, INITIAL_PIXEL_VALUE);
            cam->set_display(camMat);

            container.cameras.push_back( CCCamera { cam, camType, camIdx, camTex, camMat } );
        }
    }

    // Updates cameras
    void CCGui::updateCameras() {
        for(auto ccc : container.cameras) {
            ccc.cam->update_display_frame();

            if(!ccc.camMat.empty()) {
                writeToDisplay(ccc.camType + " " + std::to_string(ccc.camIdx),
                               ccc.camMat);
                updateTexture(*ccc.camTex, (void*) ccc.camMat.data,
                              ccc.cam->get_width(), ccc.cam->get_height());
            }
        }
    }

    // Displays cameras
    void CCGui::displayCameras() {
        ImGuiIO& io = ImGui::GetIO();

        camera::Base* cam;
        ImVec2 gridSize;
        ImVec2 virtualDisplaySize;
        ImVec2 pos;
        ImVec2 size;

        virtualDisplaySize = ImVec2(DISPLAY_SPACE_SCALE.x * io.DisplaySize.x,
                                    DISPLAY_SPACE_SCALE.y * io.DisplaySize.y);

        if(container.numCams == 1) {
            int idx = ZERO;
            cam = container.cameras[idx].cam;

            gridSize = ImVec2(virtualDisplaySize.x, virtualDisplaySize.y);
            size = ImVec2(((float) cam->get_width() / cam->get_height()) * (GRID_SCALE_Y * gridSize.y),
                          GRID_SCALE_Y * gridSize.y);

            pos = ImVec2(0.5 * gridSize.x,
                         0.5 * gridSize.y);

            drawToWindow(*container.cameras[idx].camTex, pos, size, PIVOT_CENTER, idx);
        }

        else {
            int nHorizontalGrids = (container.numCams % 2 == 0 ? container.numCams : container.numCams + 1) / 2;
            gridSize = ImVec2(virtualDisplaySize.x / nHorizontalGrids,
                              virtualDisplaySize.y / 2);

            int xOffset;
            int yOffset;

            for(int i = 0; i < container.numCams; i++) {
                cam = container.cameras[i].cam;

                xOffset = i % (nHorizontalGrids);
                yOffset = i < (nHorizontalGrids) ? 0 : 1;

                size = ImVec2(((float) cam->get_width() / cam->get_height()) * (GRID_SCALE_Y * gridSize.y),
                              GRID_SCALE_Y * gridSize.y);

                pos = ImVec2(xOffset * gridSize.x + 0.5 * gridSize.x,
                             yOffset * gridSize.y + 0.5 * gridSize.y);

                drawToWindow(*container.cameras[i].camTex,
                             pos, size, PIVOT_CENTER, i);
            }
        }
    }
}