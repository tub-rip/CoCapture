#ifdef ENABLE_SPINNAKER_SDK

#include <chrono>
#include <thread>
#include <fstream>

#include "flir_camera.h"

namespace rcg::cams::flir {

    FlirCamera::FlirCamera(const char *serial_number) :

    is_started_  (false),
    is_recording_(false),
    system_(Spinnaker::System::GetInstance())

    {
            Spinnaker::CameraList cam_list = system_->GetCameras();

            for (size_t i = 0; i < cam_list.GetSize(); ++i) {
                Spinnaker::CameraPtr current_cam = cam_list.GetByIndex(i);
                Spinnaker::GenApi::CStringPtr current_serial =
                        current_cam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");

                if (std::strcmp(serial_number, current_serial->GetValue().c_str()) == 0) {
                    camera_ = current_cam;
                    break;
                }
            }
            // TODO: Set hardware parameters

            cam_list.Clear();
    }

    FlirCamera::~FlirCamera() {
        system_->ReleaseInstance();
    }

    std::string FlirCamera::GetSerialNumber() {
        //return std::string {camera_.GetDeviceInfo().GetSerialNumber()};
        return "0";
    }

    std::string FlirCamera::GetModelName() {
        //return std::string {camera_.GetDeviceInfo().GetModelName()};
        return "0";
    }

    std::string FlirCamera::GetVendorName() {
        //return std::string {camera_.GetDeviceInfo().GetVendorName()};
    }

    int FlirCamera::GetImageFrameWidth() {
        //return camera_.Width.GetValue();
    }

    int FlirCamera::GetImageFrameHeight() {
        //return camera_.Height.GetValue();
    }

    int FlirCamera::GetExposureTime() {
        //return camera_.ExposureTime.GetValue();
    }

    bool FlirCamera::SetExposureTime(int exposure_time_us) {
        //camera_.ExposureTime.SetValue(exposure_time_us);
        return true;
    }

    bool FlirCamera::GetTriggerMode() {
        //if(camera_.TriggerMode.GetValue() == Flir_UniversalCameraParams::TriggerMode_On) {
        //    return true;
        //} else {
        //    return false;
        //}
    }

    bool FlirCamera::SetTriggerMode(bool trigger_mode) {
        //if(trigger_mode) {
        //    camera_.TriggerMode.SetValue(Flir_UniversalCameraParams::TriggerMode_On);
        //} else {
        //    camera_.TriggerMode.SetValue(Flir_UniversalCameraParams::TriggerMode_Off);
        //}

        return true;
    }

    bool FlirCamera::GetReverseX() {
        //return camera_.ReverseX.GetValue();
    }

    bool FlirCamera::SetReverseX(bool reverse_x) {
        //camera_.ReverseX.SetValue(reverse_x);
        return true;
    }

    bool FlirCamera::Start() {
        if(is_started_) {
            return false;
        }

        //camera_.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

        // Wait until the image event handler stabilizes
        std::this_thread::sleep_for(std::chrono::seconds(1));

        is_started_ = true;

        return true;
    }

    bool FlirCamera::Stop() {
        if(!is_started_) {
            return false;
        }

        //camera_.StopGrabbing();

        is_started_ = false;

        return true;
    }

    void FlirCamera::OutputFrame(cv::Mat& image_frame) {
        //image_event_handler_->OutputFrame(image_frame);
    }

    bool FlirCamera::StartRecording(const char* output_dir) {
        if(is_recording_) {
            return false;
        }

        //image_event_handler_->StartRecording(output_dir);

        is_recording_ = true;

        return true;
    }

    bool FlirCamera::StopRecording() {
        if(!is_recording_) {
            return false;
        }

        //image_event_handler_->StopRecording();

        is_recording_ = false;

        return true;
    }

    void FlirCamera::AnalyzeRecording(const char* output_dir) {
        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

        std::fstream recording_info;
        recording_info.open(output_dir_str + "/recording_info.txt", std::ios::out);

        // Count frames
        cv::VideoCapture video_capture {output_dir_str + "/frames.mp4"};
        int frames_count = video_capture.get(cv::CAP_PROP_FRAME_COUNT);

        recording_info << "Frames: " << std::to_string(frames_count);
    }

    std::vector<std::string> FlirCamera::ListConnectedCameras() {
        Spinnaker::SystemPtr system = Spinnaker::System::GetInstance();
        Spinnaker::CameraList cam_list = system->GetCameras();

        std::vector<std::string> serial_numbers;
        for(unsigned int i = 0; i < cam_list.GetSize(); ++i) {
            Spinnaker::CameraPtr cam = cam_list.GetByIndex(i);
            Spinnaker::GenApi::CStringPtr serial = cam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
            serial_numbers.push_back(serial->GetValue().c_str());
            return serial_numbers;
        }
        cam_list.Clear();
        system->ReleaseInstance();
        return serial_numbers;
    }

} // rcg::cams::flir

#endif //ENABLE_SPINNAKER_SDK