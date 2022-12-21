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

    int BaslerCamera::get_exposure_time() {
        return int(cam_.ExposureTime.GetValue());
    }

    void BaslerCamera::set_exposure_time(int exposure_time) {
        cam_.ExposureTime.SetValue(float(exposure_time));
    }

} // camera