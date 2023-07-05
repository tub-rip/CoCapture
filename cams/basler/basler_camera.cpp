#ifdef ENABLE_PYLON_SDK

#include <chrono>
#include <thread>
#include <fstream>

#include "basler_camera.h"

namespace rcg::cams::basler {

    BaslerCamera::BaslerCamera(const char *serial_number) :

    is_started_  (false),
    is_recording_(false)

    {
        Pylon::PylonInitialize();
        camera_.Attach(Pylon::CTlFactory::GetInstance().CreateDevice(Pylon::CDeviceInfo().SetSerialNumber(serial_number)));
        image_event_handler_ = new ImageEventHandler();
        camera_.RegisterImageEventHandler(image_event_handler_,
                                          Pylon::RegistrationMode_Append,
                                          Pylon::Cleanup_Delete);

        camera_.Open();

        // @TODO: Allow following settings to be modified programmatically
        camera_.TriggerActivation.SetValue("RisingEdge");
        camera_.AcquisitionFrameRateEnable.SetValue(true);
        camera_.AcquisitionFrameRate.SetValue(30);
    }

    BaslerCamera::~BaslerCamera() {
        camera_.Close();
        camera_.DestroyDevice();
        Pylon::PylonTerminate();
    }

    std::string BaslerCamera::GetSerialNumber() {
        return std::string {camera_.GetDeviceInfo().GetSerialNumber()};
    }

    std::string BaslerCamera::GetModelName() {
        return std::string {camera_.GetDeviceInfo().GetModelName()};
    }

    std::string BaslerCamera::GetVendorName() {
        return std::string {camera_.GetDeviceInfo().GetVendorName()};
    }

    int BaslerCamera::GetImageFrameWidth() {
        return camera_.Width.GetValue();
    }

    int BaslerCamera::GetImageFrameHeight() {
        return camera_.Height.GetValue();
    }

    int BaslerCamera::GetExposureTime() {
        return camera_.ExposureTime.GetValue();
    }

    bool BaslerCamera::SetExposureTime(int exposure_time_us) {
        camera_.ExposureTime.SetValue(exposure_time_us);
        return true;
    }

    bool BaslerCamera::GetTriggerMode() {
        if(camera_.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) {
            return true;
        } else {
            return false;
        }
    }

    bool BaslerCamera::SetTriggerMode(bool trigger_mode) {
        if(trigger_mode) {
            camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
        } else {
            camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
        }

        return true;
    }

    bool BaslerCamera::GetReverseX() {
        return camera_.ReverseX.GetValue();
    }

    bool BaslerCamera::SetReverseX(bool reverse_x) {
        camera_.ReverseX.SetValue(reverse_x);
        return true;
    }

    bool BaslerCamera::Start() {
        if(is_started_) {
            return false;
        }

        camera_.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

        // Wait until the image event handler stabilizes
        std::this_thread::sleep_for(std::chrono::seconds(1));

        is_started_ = true;

        return true;
    }

    bool BaslerCamera::Stop() {
        if(!is_started_) {
            return false;
        }

        camera_.StopGrabbing();

        is_started_ = false;

        return true;
    }

    void BaslerCamera::OutputFrame(cv::Mat& image_frame) {
        image_event_handler_->OutputFrame(image_frame);
    }

    bool BaslerCamera::StartRecording(const char* output_dir) {
        if(is_recording_) {
            return false;
        }

        image_event_handler_->StartRecording(output_dir);

        is_recording_ = true;

        return true;
    }

    bool BaslerCamera::StopRecording() {
        if(!is_recording_) {
            return false;
        }

        image_event_handler_->StopRecording();

        is_recording_ = false;

        return true;
    }

    void BaslerCamera::AnalyzeRecording(const char* output_dir) {
        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

        std::fstream recording_info;
        recording_info.open(output_dir_str + "/recording_info.txt", std::ios::out);

        // Count frames
        cv::VideoCapture video_capture {output_dir_str + "/frames.mp4"};
        int frames_count = video_capture.get(cv::CAP_PROP_FRAME_COUNT);

        recording_info << "Frames: " << std::to_string(frames_count);
    }

    std::vector<std::string> BaslerCamera::ListConnectedCameras() {
        Pylon::PylonAutoInitTerm pylon_auto_init_term;
        Pylon::CTlFactory& tl_factory = Pylon::CTlFactory::GetInstance();
        Pylon::DeviceInfoList device_info_list;
        tl_factory.EnumerateDevices(device_info_list);

        std::vector<std::string> serial_numbers;
        for(Pylon::DeviceInfoList::const_iterator it = device_info_list.begin(); it != device_info_list.end(); ++it) {
            serial_numbers.push_back(std::string {it->GetSerialNumber()});
        }

        return serial_numbers;
    }

} // rcg::cams::basler

#endif //ENABLE_PYLON_SDK