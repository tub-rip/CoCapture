//
// Created by friedhelm on 22.11.22.
//

#ifndef RIP_COCAPTURE_GUI_BASE_H
#define RIP_COCAPTURE_GUI_BASE_H

#include <opencv2/opencv.hpp>

namespace camera {

    class Base {
    public:
        Base() : offset_(-1), width_(-1), height_(-1) {}
        virtual ~Base() = default;

        // Pure virtuals
        virtual void setup_camera() = 0;

        virtual void get_display_frame(cv::Mat &display) = 0;

        // Getter & Setter
        [[nodiscard]] int get_height() const { return height_; }

        [[nodiscard]] int get_width() const { return width_; }

        void set_display(cv::Mat display);

        cv::Mat get_display();

        void set_offset(int offset) { offset_ = offset; }

        [[nodiscard]] int get_offset() const { return offset_; }

        // Further
        void update_display_frame();

        virtual void start_recording_to_path(std::string path) = 0;

        virtual void stop_recording() = 0;

    protected:
        int width_;
        int height_;

    private:
        cv::Mat display_;
        int offset_;
    };

} // camera

#endif //RIP_COCAPTURE_GUI_BASE_H
