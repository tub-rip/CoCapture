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
    const std::string APP_TITLE = "RIP_COCAPTURE_GUI";
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
    const ImVec2 GENERAL_SPACE_SCALE = ImVec2(0.25f, 0.15f);
    const ImVec2 SETTINGS_SPACE_SCALE = ImVec2(0.25f, 0.85f);

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

    const std::string BASLER_EXPOSURE_LABEL = "ExposureTime";
    const int BASLER_EXPOSURE_DEFAULT = 5000;
    const int BASLER_EXPOSURE_MIN = 500;
    const int BASLER_EXPOSURE_MAX = 20000;

    const std::string PROPHESEE_BIAS_FO_LABEL = "bias_fo";
    const int PROPHESEE_BIAS_FO_DEFAULT = 70;
    const int PROPHESEE_BIAS_FO_MIN = 45;
    const int PROPHESEE_BIAS_FO_MAX = 110;

    const std::string PROPHESEE_BIAS_DIFF_OFF_LABEL = "bias_diff_off";
    const int PROPHESEE_BIAS_DIFF_OFF_DEFAULT = 40;
    const int PROPHESEE_BIAS_DIFF_OFF_MIN = 25;
    const int PROPHESEE_BIAS_DIFF_OFF_MAX = 65;

    const std::string PROPHESEE_BIAS_DIFF_ON_LABEL = "bias_diff_on";
    const int PROPHESEE_BIAS_DIFF_ON_DEFAULT = 125;
    const int PROPHESEE_BIAS_DIFF_ON_MIN = 95;
    const int PROPHESEE_BIAS_DIFF_ON_MAX = 140;

    const std::string PROPHESEE_BIAS_HPF_LABEL = "bias_hpf";
    const int PROPHESEE_BIAS_HPF_DEFAULT = 60;
    const int PROPHESEE_BIAS_HPF_MIN = 0;
    const int PROPHESEE_BIAS_HPF_MAX = 120;

    const std::string PROPHESEE_BIAS_REFR_LABEL = "bias_refr";
    const int PROPHESEE_BIAS_REFR_DEFAULT = 60;
    const int PROPHESEE_BIAS_REFR_MIN = 20;
    const int PROPHESEE_BIAS_REFR_MAX = 100;

    const std::vector<std::string> PROPHESEE_BIAS_VALUES { PROPHESEE_BIAS_FO_LABEL,
                                                           PROPHESEE_BIAS_DIFF_OFF_LABEL,
                                                           PROPHESEE_BIAS_DIFF_ON_LABEL,
                                                           PROPHESEE_BIAS_HPF_LABEL,
                                                           PROPHESEE_BIAS_REFR_LABEL };

    // CCGui structs
    // @TODO: Window triple struct (pos, size, pivot)

    // Basler Settings
    struct BaslerSettings {
        int exposureTime;
    };

    struct PropheseeSettings {
        int bias_fo;
        int bias_diff_off;
        int bias_diff_on;
        int bias_hpf;
        int bias_refr;
    };

    // Prophesee Settings

    // CCGui camera
    struct CCCamera {
        camera::Base* cam;
        std::string camType;
        int camIdx;
        GLuint* camTex;
        cv::Mat camMat;

        BaslerSettings basSet;
        PropheseeSettings proSet;
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