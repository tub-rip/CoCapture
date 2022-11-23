#ifndef RIP_COCAPTURE_GUI_PARAMETERS_H
#define RIP_COCAPTURE_GUI_PARAMETERS_H

#include <iostream>
#include <cstring>

#include <opencv2/core.hpp>


struct Parameters {
    cv::Matx33d homography;
    bool do_warp;
    int target_width;
    int target_height;
    bool overlay;
    bool show_snr;
    bool roi;
    std::vector<std::string> camera_types;
};

struct PropheseeParams {
    bool set_rois;
    bool show_snr;
};

struct BaslerParams {
    bool do_warp{};
    int target_width{};
    int target_height{};
    cv::Matx33d homography;
};

#endif //RIP_COCAPTURE_GUI_PARAMETERS_H
