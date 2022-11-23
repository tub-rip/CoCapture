#include "constants.h"

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
