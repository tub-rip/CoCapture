#pragma once

#include <opencv2/core.hpp>

namespace Gui {

    // OpenCV
    const int OPENCV_MAT_TYPE = CV_8UC3;
    const cv::Vec3b OPENCV_INIT_PIXEL_VALUE = cv::Vec3b(0, 0, 0);

    // Basler camera
    const std::string BASLER = "basler";

} // Gui
