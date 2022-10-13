#ifndef RIP_COCAPTURE_GUI_CONSTANTS_H
#define RIP_COCAPTURE_GUI_CONSTANTS_H

struct Bbox {
    int x_min;
    int x_max;
    int y_min;
    int y_max;

    cv::Rect get_cv_rect() {
        return cv::Rect(x_min, y_min, x_max - x_min, y_max - y_min);
    }
};

Bbox get_signal_box(const int width, const int height, const int box_length) {
    int x_min = (int) (width / 2) - (int) (box_length / 2);
    int x_max = (int) (width / 2) + (int) (box_length / 2);
    int y_min = (int) (height / 2) - (int) (box_length / 2);
    int y_max = (int) (height / 2) + (int) (box_length / 2);
    return Bbox{x_min, x_max, y_min, y_max};
}

Bbox get_noise_box(const int width, const int height, const int box_length) {
    int x_min = width - box_length - 1;
    int x_max = width - 1;
    int y_min = 0;
    int y_max = box_length - 1;
    return Bbox{x_min, x_max, y_min, y_max};
}

#endif //RIP_COCAPTURE_GUI_CONSTANTS_H
