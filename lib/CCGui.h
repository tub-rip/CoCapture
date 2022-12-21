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
    const std::string APP_TITLE = "Recorder";
    const std::string GENERAL_TITLE = "General";
    const std::string SETTINGS_TITLE = "Settings";
    const std::string GLSL_VERSION = "#version 130";
    const std::string PROPHESEE = "prophesee";
    const std::string BASLER = "basler";
    const std::string MASTER = "master";
    const std::string SLAVE = "slave";

    const int DISPLAY_INDEX = 0;
    const int ZERO = 0;
    const int MAT_TYPE = CV_8UC3;
    const int WINDOW_FLAGS = SDL_WINDOW_OPENGL |
                             SDL_WINDOW_RESIZABLE |
                             SDL_WINDOW_ALLOW_HIGHDPI;

    const float GRID_SCALE_Y = 0.9f;
    const float ALPHA = 1.0f;

    const short MAX_VAL_RGBU8 = 255;
    const short CC_R = 0x3B; const int CC_G = 0x3B; const int CC_B = 0x3B;

    const ImVec2 PIVOT_CENTER = ImVec2(0.5, 0.5);
    const ImVec2 NO_PIVOT = ImVec2(0, 0);
    const ImVec2 WINDOW_PADDING = ImVec2(2.5f, 2.5f);

    const ImVec2 DISPLAY_SPACE_SCALE = ImVec2(0.75f, 1.0f);
    const ImVec2 GENERAL_SPACE_SCALE = ImVec2(0.25f, 0.35f);
    const ImVec2 SETTINGS_SPACE_SCALE = ImVec2(0.25f, 0.65f);

    const ImVec4 CLEAR_COLOR = ImVec4(float(CC_R)/MAX_VAL_RGBU8,
                                      float(CC_G)/MAX_VAL_RGBU8,
                                      float(CC_B)/MAX_VAL_RGBU8,
                                      float(ALPHA));

    const cv::Vec3b INITIAL_PIXEL_VALUE = cv::Vec3b(0, 0, 0);

    const cv::Point ORIGIN = cv::Point(30, 60);
    const cv::HersheyFonts FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
    const double FONT_SCALE = 1.25;
    const cv::Scalar FONT_COLOR = cv::Scalar(0, MAX_VAL_RGBU8, 0);
    const int LINE_THICKNESS = 1;
    const int LINE_TYPE = cv::LINE_8;
    const bool BOTTOM_LEFT_ORIGIN = true;

    // CCGui structs
    // @TODO: Window triple struct (pos, size, pivot)

    // CCGui camera
    struct CCCamera {
        camera::Base* cam;
        std::string camType;
        int camIdx;
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
        void writeToDisplay(std::string text, cv::Mat mat);
        void displayGeneral();
        void displaySettings();

        // Camera methods
        void setupCameras();
        void updateCameras();
        void displayCameras();

    private:
        CCContainer container;
    };
}

#endif //RIP_COCAPTURE_GUI_CCGUI_H