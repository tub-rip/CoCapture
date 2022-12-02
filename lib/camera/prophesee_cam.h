#ifndef RIP_COCAPTURE_GUI_PROPHESEE_CAM_H
#define RIP_COCAPTURE_GUI_PROPHESEE_CAM_H

#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>
#include <metavision/hal/facilities/i_device_control.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <metavision/sdk/core/utils/rate_estimator.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

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

        void start_camera();

        void stop_camera();

        void set_mode_master();

        void set_mode_slave();

    private:
        Metavision::Camera cam_;
        EventVisualizer visualizer_;
        PropheseeParams params_;
        cv::Mat cd_frame_;
#ifdef USE_METAVISION_VIEWER
        Metavision::CDFrameGenerator *cd_frame_generator;
#endif
    };


} // camera

#endif //RIP_COCAPTURE_GUI_PROPHESEE_CAM_H
