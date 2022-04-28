#ifndef RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
#define RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H

#include <opencv2/opencv.hpp>
#include <metavision/sdk/base/events/event_cd.h>


class EventVisualizer {
public:
    EventVisualizer() = default;
    ~EventVisualizer() = default;
    void setup_display(int width, int height);
    void get_display_frame(cv::Mat &display);
    void process_events(const Metavision::EventCD *begin,
                        const Metavision::EventCD *end);

private:
    cv::Mat img_;
    cv::Mat img_swap_;
    std::mutex m;

    // Display colors
    cv::Vec3b color_bg_ = cv::Vec3b(30, 37, 52);
    cv::Vec3b color_on_ = cv::Vec3b(216, 223, 236);
    cv::Vec3b color_off_ = cv::Vec3b(64, 126, 201);
};


#endif //RIP_COCAPTURE_GUI_EVENT_VISUALIZER_H
