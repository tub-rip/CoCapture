#pragma once

#include <opencv2/core.hpp>

#include "SDL2/SDL.h"
#include <SDL_opengl.h>
#include "imgui.h"

namespace Gui {

    // Backend (SDL and OpenGL)
    const int BE_INIT_SUBSYSTEMS = SDL_INIT_VIDEO;
    const int BE_DISPLAY_INDEX = 0;
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

    // Prophesee camera
    const std::string PROPHESEE = "prophesee";
    const std::string MASTER = "master";
    const std::string SLAVE = "slave";

} // Gui
