#pragma once

#include <opencv2/core.hpp>

#include "SDL2/SDL.h"
#include <SDL_opengl.h>
#include "imgui.h"

namespace Gui {

    // Backend (SDL and OpenGL)
    const int BE_INIT_SUBSYSTEMS = SDL_INIT_VIDEO;
    const int BE_DEFAULT_DISPLAY_INDEX = 0;
    const std::string BE_WINDOW_TITLE = "Recorder";
    const int BE_WINDOW_FLAGS = SDL_WINDOW_OPENGL |
                                SDL_WINDOW_RESIZABLE |
                                SDL_WINDOW_ALLOW_HIGHDPI;
    const int BE_SWAP_INTERVAL = 1;
    const std::string BE_GLSL_VERSION = "#version 130";
    const int BE_BUFFER_FLAGS = GL_COLOR_BUFFER_BIT;

    const short BE_MAX_PIXEL_VALUE = 0xFF;
    const short BE_CLEAR_COLOR_RED = 0x3B;
    const short BE_CLEAR_COLOR_GREEN = 0x3B;
    const short BE_CLEAR_COLOR_BLUE = 0x3B;
    const ImVec4 BE_CLEAR_COLORS = ImVec4(float(BE_CLEAR_COLOR_RED)/BE_MAX_PIXEL_VALUE,
                                          float(BE_CLEAR_COLOR_GREEN)/BE_MAX_PIXEL_VALUE,
                                          float(BE_CLEAR_COLOR_BLUE)/BE_MAX_PIXEL_VALUE,
                                          float(BE_MAX_PIXEL_VALUE)/BE_MAX_PIXEL_VALUE);

    // OpenCV
    const int OPENCV_MAT_TYPE = CV_8UC3;
    const cv::Vec3b OPENCV_INIT_PIXEL_VALUE = cv::Vec3b(0, 0, 0);

    // Basler camera
    const std::string BASLER = "basler";

    const std::string BASLER_OUTPUT_FILENAME = "frames.mp4";

    const std::string BASLER_EXPOSURE_LABEL = "Exposure time";
    const int BASLER_EXPOSURE_DEFAULT = 5000;
    const int BASLER_EXPOSURE_MIN = 500;
    const int BASLER_EXPOSURE_MAX = 20000;

    const std::string BASLER_TRIGGER_MODE_LABEL = "External trigger";

    const double BASLER_RECORDING_FPS = 30;

    // Prophesee camera
    const std::string PROPHESEE = "prophesee";
    const std::string MASTER = "master";
    const std::string SLAVE = "slave";

    const std::string PROPHESEE_OUTPUT_FILENAME = "output";

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

    // Realsense camera
    const std::string REALSENSE = "realsense";
    const std::string REALSENSE_FRAME_OUTPUT_FILENAME = "frame";

}
