#include "basler_camera.h"

namespace camera {
    void BaslerCamera::setup_camera() {
        std::cout << "Opening Basler Camera: " << cam_.GetDeviceInfo().GetModelName() << std::endl;

        event_handler_ = new basler::BaslerEventHandler(params_);
        cam_.RegisterImageEventHandler(
                event_handler_,
                Pylon::RegistrationMode_ReplaceAll,
                Pylon::Cleanup_Delete
        );
        cam_.Open();
        width_ = cam_.Width.GetValue();
        height_ = cam_.Height.GetValue();
        cam_.ReverseX.SetValue(true);
        cam_.StartGrabbing(Pylon::GrabStrategy_OneByOne,
                           Pylon::GrabLoop_ProvidedByInstantCamera);
    }

    void BaslerCamera::get_display_frame(cv::Mat &display) {
        event_handler_->get_display_frame(display);
    }

} // camera