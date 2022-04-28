#include "event_visualizer.h"


void EventVisualizer::setup_display(const int width, const int height) {
    img_ = cv::Mat(height, width, CV_8UC3);
    img_swap_ = cv::Mat(height, width, CV_8UC3);
    img_.setTo(color_bg_);
}


void EventVisualizer::get_display_frame(cv::Mat &display) {
    {
        std::unique_lock<std::mutex> lock(m);
        std::swap(img_, img_swap_);
        img_.setTo(color_bg_);
    }
    img_swap_.copyTo(display);
}


void EventVisualizer::process_events(const Metavision::EventCD *begin,
                                     const Metavision::EventCD *end) {
    {
        std::unique_lock<std::mutex> lock(m);
        for (auto it = begin; it != end; ++it) {
            img_.at<cv::Vec3b>(it->y, it->x) = (it->p) ? color_on_ : color_off_;
        }
    }
}

