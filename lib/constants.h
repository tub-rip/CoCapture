#ifndef RIP_COCAPTURE_GUI_CONSTANTS_H
#define RIP_COCAPTURE_GUI_CONSTANTS_H

#include <opencv2/opencv.hpp>

struct Bbox {
    int x_min;
    int x_max;
    int y_min;
    int y_max;

    cv::Rect get_cv_rect() {
        return cv::Rect(x_min, y_min, x_max - x_min, y_max - y_min);
    }
};

Bbox get_signal_box(const int width, const int height, const int box_length);
Bbox get_noise_box(const int width, const int height, const int box_length);

#endif //RIP_COCAPTURE_GUI_CONSTANTS_H
