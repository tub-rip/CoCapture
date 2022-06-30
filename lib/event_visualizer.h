#ifndef RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
#define RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H

#include <opencv2/opencv.hpp>
#include <metavision/sdk/base/events/event_cd.h>


class EventVisualizer {
public:
    EventVisualizer() = default;
    ~EventVisualizer() = default;

    void setup_display(const int width, const int height) {
        img_ = cv::Mat(height, width, CV_8UC3);
        img_swap_ = cv::Mat(height, width, CV_8UC3);
        img_.setTo(color_bg_);
    }

    void get_display_frame(cv::Mat &display, const cv::Mat &canvas) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
            //img_.setTo(color_bg_);
            canvas.copyTo(img_);
        }
        cv::flip(img_swap_, display, 0);
    }

    void get_display_frame(cv::Mat &display) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
            img_.setTo(color_bg_);
        }
        cv::flip(img_swap_, display, 0);
    }

    void process_events(const Metavision::EventCD *begin,
                        const Metavision::EventCD *end) {
        {
            std::unique_lock<std::mutex> lock(m_);
            for (auto it = begin; it != end; ++it) {
                img_.at<cv::Vec3b>(it->y, it->x) = (it->p) ? color_on_ : color_off_;
            }
        }
    }

private:
    cv::Mat img_;
    cv::Mat img_swap_;
    std::mutex m_;

    // Display colors
    cv::Vec3b color_bg_ = cv::Vec3b(255, 255, 255);
    //cv::Vec3b color_on_ = cv::Vec3b(216, 223, 236);
    //cv::Vec3b color_off_ = cv::Vec3b(64, 126, 201);
    cv::Vec3b color_on_ = cv::Vec3b(0, 0, 255);
    cv::Vec3b color_off_ = cv::Vec3b(255, 0, 0);
};


#endif //RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
