#ifndef RIP_COCAPTURE_GUI_PROPHESEE_CAM_H
#define RIP_COCAPTURE_GUI_PROPHESEE_CAM_H

#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>

#include "base.h"
#include "../event_visualizer.h"
#include "../utils.h"


namespace camera {

    class PropheseeCam : public Base {
    public:
        explicit PropheseeCam(PropheseeParams &params) : params_(params) {};
        explicit PropheseeCam(PropheseeParams params) : params_(params) {};

        void get_display_frame(cv::Mat &display) override;

        void get_display_frame(cv::Mat &display, const cv::Mat &canvas);

        void setup_camera() override;

    private:
        Metavision::Camera cam_;
        EventVisualizer visualizer_;
        PropheseeParams params_;
    };


} // camera

#endif //RIP_COCAPTURE_GUI_PROPHESEE_CAM_H
