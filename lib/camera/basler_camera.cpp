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
        cam_.LineSelector.SetValue("Line1");
        cam_.LineMode.SetValue("Input");
        cam_.TriggerSelector.SetValue("FrameStart");
        cam_.TriggerSource.SetValue("Line1");
        cam_.TriggerActivation.SetValue("RisingEdge");

        cam_.AcquisitionFrameRateEnable.SetValue(true);
        cam_.AcquisitionFrameRate.SetValue(30);

        cam_.StartGrabbing(Pylon::GrabStrategy_OneByOne,
                           Pylon::GrabLoop_ProvidedByInstantCamera);
    }

    void BaslerCamera::get_display_frame(cv::Mat &display) {
        event_handler_->get_display_frame(display);
    }

    void BaslerCamera::set_exposure_time(int exposure_time) { cam_.ExposureTime.SetValue(float(exposure_time)); }

    void BaslerCamera::set_trigger_mode(bool on) {
        if(on) { cam_.TriggerMode.SetValue("On"); }
        else { cam_.TriggerMode.SetValue("Off"); }
    }

    void BaslerCamera::start_recording_to_path(std::string path) {
        recorder_->set_file_path(path);
        cam_.RegisterImageEventHandler(recorder_,
                                       Pylon::RegistrationMode_Append,
                                       Pylon::Cleanup_None);
    }

    void BaslerCamera::startup_recorder(std::string file_path, double fps) {
        recorder_ = new basler::BaslerRecorder(fps);
        start_recording_to_path(file_path);
    }

    void BaslerCamera::stop_recording() {
        cam_.DeregisterImageEventHandler(recorder_);
    }

} // camera