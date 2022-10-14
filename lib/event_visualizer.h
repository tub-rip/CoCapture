#ifndef RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
#define RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H

#include <queue>
#include <numeric>

#include <opencv2/opencv.hpp>
#include <metavision/sdk/base/events/event_cd.h>

#include "constants.h"


class EventVisualizer {
public:
    EventVisualizer() = default;

    ~EventVisualizer() = default;

    void setup_display(const int width, const int height, bool show_snr) {
        img_ = cv::Mat(height, width, CV_8UC3);
        img_swap_ = cv::Mat(height, width, CV_8UC3);
        img_.setTo(color_bg_);
        signal_box_ = get_signal_box(width, height, 120);
        noise_box_ = get_noise_box(width, height, 120);
        show_snr_ = show_snr;
    }

    void get_display_frame(cv::Mat &display, const cv::Mat &canvas) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
            canvas.copyTo(img_);
        }
        if (show_snr_) {
            draw_snr(img_swap_);
        }
        cv::flip(img_swap_, display, 0);
    }

    void get_display_frame(cv::Mat &display) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
            img_.setTo(color_bg_);
        }
        if (show_snr_) {
            draw_snr(img_swap_);
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

    void estimate_snr(const Metavision::EventCD *begin,
                      const Metavision::EventCD *end) {
        int cnt_signal = 0;
        int cnt_noise = 0;
        for (auto it = begin; it != end; ++it) {
            if (it->x > signal_box_.x_min && it->x < signal_box_.x_max
                && it->y > signal_box_.y_min && it->y < signal_box_.y_max) {
                cnt_signal++;
            }
            if (it->x > noise_box_.x_min && it->x < noise_box_.x_max
                && it->y > noise_box_.y_min && it->y < noise_box_.y_max) {
                cnt_noise++;
            }
        }
        {
            std::unique_lock<std::mutex> lock(m_snr_);
            if (cnt_noise != 0) {
                last_snrs_.push_back((int) 100 * (cnt_signal - cnt_noise) / cnt_noise);
            }

            if (last_snrs_.size() > max_size_snr_queue_) {
                last_snrs_.pop_front();
            }
        }
    }

    void draw_snr(cv::Mat& display) {
        int current_snr;
        {
            std::unique_lock<std::mutex> lock(m_snr_);
            current_snr = (int) (std::reduce(last_snrs_.begin(), last_snrs_.end(), 0.0) / last_snrs_.size());
        }
        cv::Rect signal_box = signal_box_.get_cv_rect();
        cv::Rect noise_box = noise_box_.get_cv_rect();
        cv::rectangle(display, signal_box, color_bbox_, 3);
        cv::rectangle(display, noise_box, color_bbox_, 3);
        //cv::putText(display, std::to_string(current_snr), cv::Point(20, 40),
        //            cv::FONT_HERSHEY_SIMPLEX, 1, color_bbox_, 2);
    }

    int get_snr() {
        std::unique_lock<std::mutex> lock(m_snr_);
        return (int) (std::reduce(last_snrs_.begin(), last_snrs_.end(), 0.0) / last_snrs_.size());
    }

private:
    cv::Mat img_;
    cv::Mat img_swap_;
    std::mutex m_;
    std::mutex m_snr_;
    Bbox signal_box_;
    Bbox noise_box_;
    bool show_snr_;
    int max_size_snr_queue_ = 5000;
    std::deque<int> last_snrs_;

    // Display colors
    cv::Vec3b color_bg_ = cv::Vec3b(255, 255, 255);
    cv::Vec3b color_on_ = cv::Vec3b(0, 0, 255);
    cv::Vec3b color_off_ = cv::Vec3b(255, 0, 0);
    cv::Scalar color_bbox_ = cv::Scalar (22, 100, 8);
};


#endif //RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
