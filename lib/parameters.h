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
    explicit PropheseeParams(Parameters &params) {
        set_rois = params.roi;
        show_snr = params.show_snr;
    }

    bool set_rois;
    bool show_snr;
};

struct BaslerParams {
    explicit BaslerParams(Parameters &params) {
        do_warp = params.do_warp;
        homography = params.homography;
        target_width = params.target_width;
        target_height = params.target_height;
    }

    bool do_warp{};
    int target_width{};
    int target_height{};
    cv::Matx33d homography;
};

#endif //RIP_COCAPTURE_GUI_PARAMETERS_H
