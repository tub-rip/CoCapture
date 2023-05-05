#ifdef ENABLE_PYLON_SDK

#include "basler_camera.h"

namespace Cocapture {

    BaslerCamera::BaslerCamera(std::string serial_number) {
        Pylon::PylonInitialize();
        camera_.Attach(Pylon::CTlFactory::GetInstance().CreateDevice(Pylon::CDeviceInfo().SetSerialNumber(serial_number.c_str())));
        image_event_handler_ = new ImageEventHandler();
        camera_.RegisterImageEventHandler(image_event_handler_,
                                          Pylon::RegistrationMode_Append,
                                          Pylon::Cleanup_Delete);

        camera_.Open();

        camera_.ReverseX.SetValue(true);
        camera_.AcquisitionFrameRateEnable.SetValue(true);
        camera_.AcquisitionFrameRate.SetValue(30);
    }

    void BaslerCamera::Start() { camera_.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera); }

    std::string BaslerCamera::GetInfo(int info)  {
        Pylon::String_t info_string;

        switch(info) {
            case BASLER_INFO_SERIAL:
                info_string = camera_.GetDeviceInfo().GetSerialNumber();
                break;

            case BASLER_INFO_MODEL:
                info_string = camera_.GetDeviceInfo().GetModelName();
                break;

            case BASLER_INFO_VENDOR:
                info_string = camera_.GetDeviceInfo().GetVendorName();
                break;

            case BASLER_TRIGGER_SELECTOR:
                info_string = camera_.TriggerSelector.ToString();
                break;

            case BASLER_TRIGGER_SOURCE:
                info_string = camera_.TriggerSource.ToString();
                break;

            case BASLER_TRIGGER_ACTIVATION:
                info_string = camera_.TriggerActivation.ToString();
        }

        return std::string {info_string};
    }

    double BaslerCamera::GetProp(int property) {
        switch(property) {
            case BASLER_DIM_WIDTH:
                return camera_.Width.GetValue();

            case BASLER_DIM_HEIGHT:
                return camera_.Height.GetValue();

            case BASLER_EXPOSURE_TIME:
                return camera_.ExposureTime.GetValue();

            case BASLER_EXPOSURE_TIME_MIN:
                return camera_.ExposureTime.GetMin();

            case BASLER_EXPOSURE_TIME_MAX:
                return camera_.ExposureTime.GetMax();

            case BASLER_TRIGGER_MODE:
                if(camera_.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) { return BASLER_TRIGGER_MODE_ON; }
                else if(camera_.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_Off) { return BASLER_TRIGGER_MODE_OFF; }
        }

        return {};
    }

    void BaslerCamera::SetProp(int property, double value) {
        switch(property) {
            case BASLER_TRIGGER_SELECTOR:
                camera_.TriggerSelector.SetValue(ToString_t(value));
                break;

            case BASLER_TRIGGER_SOURCE:
                camera_.TriggerSource.SetValue(ToString_t(value));
                break;

            case BASLER_TRIGGER_ACTIVATION:
                camera_.TriggerActivation.SetValue(ToString_t(value));
                break;

            case BASLER_EXPOSURE_TIME:
                camera_.ExposureTime.SetValue((int) value);
                break;

            case BASLER_TRIGGER_MODE:
                if(value == BASLER_TRIGGER_MODE_ON) { camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On); }
                else if(value == BASLER_TRIGGER_MODE_OFF) { camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off); }
        }
    }

    void BaslerCamera::OutputFrame(int, cv::Mat& image_frame) { image_event_handler_->OutputFrame(image_frame); }

    void BaslerCamera::StartRecording(std::string output_directory_path) { image_event_handler_->StartRecording(output_directory_path); };

    void BaslerCamera::StopRecording() { image_event_handler_->StopRecording(); }

    void BaslerCamera::Clean() {
        camera_.StopGrabbing();

        camera_.Close();
        camera_.DestroyDevice();

        Pylon::PylonTerminate();
    }

    Pylon::String_t BaslerCamera::ToString_t(int value) {
        switch(value) {
            case BASLER_TRIGGER_SELECTOR_FRAME_START:
                return "FrameStart";

            case BASLER_TRIGGER_SELECTOR_FRAME_END:
                return "FrameEnd";

            case BASLER_TRIGGER_SELECTOR_FRAME_ACTIVE:
                return "FrameActive";

            case BASLER_TRIGGER_SELECTOR_FRAME_BURST_START:
                return "FrameBurstStart";

            case BASLER_TRIGGER_SELECTOR_FRAME_BURST_END:
                return "FrameBurstEnd";

            case BASLER_TRIGGER_SELECTOR_FRAME_BURST_ACTIVE:
                return "FrameBurstActive";

            case BASLER_TRIGGER_SELECTOR_EXPOSURE_START:
                return "ExposureStart";

            case BASLER_TRIGGER_SELECTOR_EXPOSURE_END:
                return "ExposureEnd";

            case BASLER_TRIGGER_SELECTOR_EXPOSURE_ACTIVE:
                return "ExposureActive";

            case BASLER_TRIGGER_SOURCE_LINE1:
                return "Line1";

            case BASLER_TRIGGER_SOURCE_LINE2:
                return "Line2";

            case BASLER_TRIGGER_SOURCE_LINE3:
                return "Line3";

            case BASLER_TRIGGER_SOURCE_LINE4:
                return "Line4";

            case BASLER_TRIGGER_SOURCE_LINE5:
                return "Line5";

            case BASLER_TRIGGER_SOURCE_LINE6:
                return "Line6";

            case BASLER_TRIGGER_ACTIVATION_RISING_EDGE:
                return "RisingEdge";

            case BASLER_TRIGGER_ACTIVATION_FALLING_EDGE:
                return "FallingEdge";

            case BASLER_TRIGGER_ACTIVATION_ANY_EDGE:
                return "AnyEdge";

            case BASLER_TRIGGER_ACTIVATION_LEVEL_HIGH:
                return "LevelHigh";

            case BASLER_TRIGGER_ACTIVATION_LEVEL_LOW:
                return "LevelLow";
        }

        return {};
    }

    int BaslerCamera::ToEnum(Pylon::String_t value) {
        if(value == "FrameStart") { return BASLER_TRIGGER_SELECTOR_FRAME_START; }

        else if(value == "FrameEnd") { return BASLER_TRIGGER_SELECTOR_FRAME_END; }

        else if(value == "FrameActive") { return BASLER_TRIGGER_SELECTOR_FRAME_ACTIVE; }

        else if(value == "FrameBurstStart") { return BASLER_TRIGGER_SELECTOR_FRAME_BURST_START; }

        else if(value == "FrameBurstEnd") { return BASLER_TRIGGER_SELECTOR_FRAME_BURST_END; }

        else if(value == "FrameBurstActive") { return BASLER_TRIGGER_SELECTOR_FRAME_BURST_ACTIVE; }

        else if(value == "ExposureStart") { return BASLER_TRIGGER_SELECTOR_EXPOSURE_START; }

        else if(value == "ExposureEnd") { return BASLER_TRIGGER_SELECTOR_EXPOSURE_END; }

        else if(value == "ExposureActive") { return BASLER_TRIGGER_SELECTOR_EXPOSURE_ACTIVE; }

        else if(value == "Line1") { return BASLER_TRIGGER_SOURCE_LINE1; }

        else if(value == "Line2") { return BASLER_TRIGGER_SOURCE_LINE2; }

        else if(value == "Line3") { return BASLER_TRIGGER_SOURCE_LINE3; }

        else if(value == "Line4") { return BASLER_TRIGGER_SOURCE_LINE4; }

        else if(value == "Line5") { return BASLER_TRIGGER_SOURCE_LINE5; }

        else if(value == "Line6") { return BASLER_TRIGGER_SOURCE_LINE6; }

        else if(value == "RisingEdge") { return BASLER_TRIGGER_ACTIVATION_RISING_EDGE; }

        else if(value == "FallingEdge") { return BASLER_TRIGGER_ACTIVATION_FALLING_EDGE; }

        else if(value == "AnyEdge") { return BASLER_TRIGGER_ACTIVATION_ANY_EDGE; }

        else if(value == "LevelHigh") { return BASLER_TRIGGER_ACTIVATION_LEVEL_HIGH; }

        else if(value == "LevelLow") { return BASLER_TRIGGER_ACTIVATION_LEVEL_LOW; }

        else { return {}; }
    }

} // Cocapture

#endif //ENABLE_PYLON_SDK