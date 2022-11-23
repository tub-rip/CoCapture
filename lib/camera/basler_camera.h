#ifndef RIP_COCAPTURE_GUI_BASLER_CAMERA_H
#define RIP_COCAPTURE_GUI_BASLER_CAMERA_H

#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "base.h"
#include "../basler_event_handler.h"

namespace camera {

    class BaslerCamera : public Base {
    public:
        BaslerCamera(BaslerParams &params) : params_(params),
                                             cam_(Pylon::CTlFactory::GetInstance().CreateFirstDevice()) {};
        ~BaslerCamera() {delete event_handler_;}
        void get_display_frame(cv::Mat &display) override;

        void setup_camera() override;

    private:
        BaslerParams params_;
        Pylon::CBaslerUniversalInstantCamera cam_;
        basler::BaslerEventHandler* event_handler_;
    };

} // camera

#endif //RIP_COCAPTURE_GUI_BASLER_CAMERA_H
