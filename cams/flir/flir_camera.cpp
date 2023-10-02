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
            if (!camera_->IsInitialized()) {
                camera_->Init();
            }

            //Spinnaker::GenApi::INodeMap & node_map = camera_->GetNodeMap();
            //Spinnaker::GenApi::CEnumerationPtr pixel_format_handle = node_map.GetNode("PixelFormat");
            //Spinnaker::GenApi::CEnumEntryPtr format_bgr8_handle =
            //        pixel_format_handle->GetEntryByName("BGR8");

            //if (Spinnaker::GenApi::IsAvailable(format_bgr8_handle)
            //    && Spinnaker::GenApi::IsReadable(format_bgr8_handle)) {
            //int64_t format_rgb8 = format_bgr8_handle->GetValue();
            //pixel_format_handle->SetIntValue(format_rgb8);
            //}
            //else {
            //    std::cout << "Could not set correct pixel format for Flir camera." << std::endl;
            //}

            height_ = this->GetImageFrameHeight();
            width_ = this->GetImageFrameWidth();
            image_handler_ = new ImageEventHandler(height_, width_);

            camera_->RegisterEventHandler(*image_handler_);

            // TODO: Set hardware parameters

            cam_list.Clear();
    }

    FlirCamera::~FlirCamera() {
        system_->ReleaseInstance();
    }

    std::string FlirCamera::GetSerialNumber() {
        Spinnaker::GenApi::CStringPtr serial =
                camera_->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
        return serial->GetValue().c_str();
    }

    std::string FlirCamera::GetModelName() {
        return "Flir";
    }

    std::string FlirCamera::GetVendorName() {
        return "Flir";
    }

    int FlirCamera::GetImageFrameWidth() {
        if (camera_ && camera_->IsValid()) {
            Spinnaker::GenApi::CIntegerPtr widthNode = camera_->GetNodeMap().GetNode("Width");
            if (Spinnaker::GenApi::IsReadable(widthNode)) {
                return static_cast<int>(widthNode->GetValue());
            }
        }
        return -1;
    }

    int FlirCamera::GetImageFrameHeight() {
        if (camera_ && camera_->IsValid()) {
            Spinnaker::GenApi::CIntegerPtr heightNode = camera_->GetNodeMap().GetNode("Height");
            if (Spinnaker::GenApi::IsReadable(heightNode)) {
                return static_cast<int>(heightNode->GetValue());
            }
        }
        return -1;
    }

    int FlirCamera::GetExposureTime() {
        return 0;
    }

    bool FlirCamera::SetExposureTime(int exposure_time_us) {
        return false;
    }

    bool FlirCamera::GetTriggerMode() {
        return false;
    }

    bool FlirCamera::SetTriggerMode(bool trigger_mode) {
        return false;
    }

    bool FlirCamera::GetReverseX() {
        //return camera_.ReverseX.GetValue();
        return false;
    }

    bool FlirCamera::SetReverseX(bool reverse_x) {
        //camera_.ReverseX.SetValue(reverse_x);
        return false;
    }

    bool FlirCamera::Start() {
        if(is_started_) {
            return false;
        }

        if (camera_ && !camera_->IsStreaming()) {
            camera_->BeginAcquisition();
        }
        is_started_ = true;
        return true;
    }

    bool FlirCamera::Stop() {
        if(!is_started_) {
            return false;
        }

        if (camera_ && camera_->IsStreaming()) {
            camera_->EndAcquisition();
        }
        is_started_ = false;

        return true;
    }

    void FlirCamera::OutputFrame(cv::Mat& image_frame) {
        return image_handler_->OutputFrame(image_frame);
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