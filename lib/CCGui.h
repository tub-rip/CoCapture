#ifndef RIP_COCAPTURE_GUI_CCGUI_H
#define RIP_COCAPTURE_GUI_CCGUI_H

#include <opencv2/core.hpp>

#include <string>
#include <iostream>

#include "SDL2/SDL.h"
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <pylon/BaslerUniversalInstantCamera.h>
#include "basler_event_handler.h"
#include "utils.h"
#include "camera/prophesee_cam.h"
#include "camera/basler_camera.h"

namespace CCGui {

    // Constants
    const std::string TITLE = "Recorder";
    const std::string GLSL_VERSION = "#version 130";
    const std::string PROPHESEE = "prophesee";
    const std::string BASLER = "basler";
    const std::string MASTER = "master";
    const std::string SLAVE = "slave";

    const int DISPLAY_INDEX = 0;
    const int ORIGIN = 0;
    const int MAT_TYPE = CV_8UC3;
    const int WINDOW_FLAGS = SDL_WINDOW_OPENGL |
                             SDL_WINDOW_RESIZABLE |
                             SDL_WINDOW_ALLOW_HIGHDPI;

    const float SCALE = 0.9f;
    const float ALPHA = 1.0f;
    const float MAX_VAL_RGBU8 = 255.0f;

    const char CC_R = 0x3B; const char CC_G = 0x3B; const char CC_B = 0x3B;

    const ImVec2 PIVOT_CENTER = ImVec2(0.5f, 0.5f);
    const ImVec4 CLEAR_COLOR = ImVec4(CC_R/MAX_VAL_RGBU8,
                                      CC_G/MAX_VAL_RGBU8,
                                      CC_B/MAX_VAL_RGBU8,
                                      ALPHA);

    const cv::Vec3b INITIAL_PIXEL_VALUE = cv::Vec3b(0, 0, 0);

    // CCGui structs
    // CCGui camera
    struct CCCamera {
        camera::Base* cam;
        std::string camType;
        GLuint* camTex;
        cv::Mat camMat;
    };

    // CCGui container
    struct CCContainer {
        SDL_DisplayMode displayMode;
        SDL_Window* mainWindow;
        SDL_GLContext glContext;

        GLuint* glTextures;

        Parameters appParameters;

        int numCams;
        std::vector<CCCamera> cameras;
    };

    // CCGui class
    class CCGui {
    public:
        // Constructor and destructor
        CCGui(int argc, const char* argv[]);
        ~CCGui();

        // GUI methods
        void startFrame();
        void render();
        void handleEvent(bool* done);
        void setupTexture(GLuint* tex, int w, int h);
        void updateTexture(GLuint tex, void* data, int w, int h);
        void drawToWindow(GLuint tex, ImVec2 pos, ImVec2 size, ImVec2 pivot, int idx);

        // Camera methods
        void setupCameras();
        void updateCameras();
        void displayCameras();

    private:
        CCContainer container;
    };
}

#endif //RIP_COCAPTURE_GUI_CCGUI_H