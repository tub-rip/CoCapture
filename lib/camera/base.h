//
// Created by friedhelm on 22.11.22.
//

#ifndef RIP_COCAPTURE_GUI_BASE_H
#define RIP_COCAPTURE_GUI_BASE_H

#include <opencv2/opencv.hpp>

namespace camera {

    class Base {
    public:
        virtual void setup_camera() = 0;
        virtual void get_display_frame(cv::Mat &display) = 0;

        int get_height() {return height_;}
        int get_width() {return width_;}

    protected:
        int width_;
        int height_;
    };

} // camera

#endif //RIP_COCAPTURE_GUI_BASE_H
