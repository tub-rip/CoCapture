#ifndef RIP_COCAPTURE_GUI_PROPHESEE_CAM_H
#define RIP_COCAPTURE_GUI_PROPHESEE_CAM_H

#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>
#include <metavision/hal/facilities/i_device_control.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <metavision/sdk/core/utils/rate_estimator.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "base.h"
#include "../utils.h"


namespace camera {

    class PropheseeCam : public Base {
    public:
        explicit PropheseeCam(PropheseeParams &params) : params_(params) {};

        explicit PropheseeCam(PropheseeParams params) : params_(params) {};

        void get_display_frame(cv::Mat &display) override;


        void setup_camera() override;

        void start_camera();

        void stop_camera();

        void set_mode_master();

        void set_mode_slave();

        int get_bias_value(std::string bias_name);

        void set_bias_value(std::string bias_name, int bias_value);

        void start_recording_to_path(std::string path) override;

        void stop_recording() override;

        void increment_ext_trigger_evts();

        void reset_ext_trigger_evts();

        int get_ext_trigger_evts();

    private:
        Metavision::Camera cam_;
        PropheseeParams params_;
        cv::Mat cd_frame_;
        std::unique_ptr<Metavision::CDFrameGenerator> cd_frame_generator;

        cv::Scalar color_bg_ = cv::Scalar{255, 255, 255};
        cv::Scalar color_on_ = cv::Scalar{0, 0, 255};
        cv::Scalar color_off_ = cv::Scalar{255, 0, 0};

        bool is_recording_{false};
        int ext_trigger_evts_{0};
        int ext_trigger_evts_cb_id_{false};
    };


} // camera

#endif //RIP_COCAPTURE_GUI_PROPHESEE_CAM_H