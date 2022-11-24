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
    bool record;
    std::vector<std::string> camera_types;
};

struct PropheseeParams {
    explicit PropheseeParams(const Parameters &params) :
            set_rois(params.roi),
            show_snr(params.show_snr),
            mode("master"),
            record_from_startup(params.record),
            id(0) {}

    explicit PropheseeParams(const Parameters &params, const std::string &mode, int new_id = 0) :
            set_rois(params.roi),
            show_snr(params.show_snr),
            mode(mode),
            record_from_startup(params.record),
            id(new_id) {}

    bool set_rois;
    bool show_snr;
    bool record_from_startup;
    std::string mode;  // master/slave
    int id;
};

struct BaslerParams {
    explicit BaslerParams(Parameters &params) : do_warp(params.do_warp),
                                                homography(params.homography),
                                                target_height(params.target_height),
                                                target_width(params.target_width) {}

    bool do_warp{};
    int target_width{};
    int target_height{};
    cv::Matx33d homography;
};

#endif //RIP_COCAPTURE_GUI_PARAMETERS_H
