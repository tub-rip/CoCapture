#include "base.h"

namespace camera {
    void Base::set_display(cv::Mat display) {
        display_ = display;
    }

    cv::Mat Base::get_display() {
        return display_;
    }

    void Base::update_display_frame() {
        this->get_display_frame(display_);
    }
} // camera