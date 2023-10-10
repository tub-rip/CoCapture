#ifdef ENABLE_SPINNAKER_SDK

#include <thread>
#include <fstream>

#include "flir_camera.h"

namespace rcg::cams::flir {

    FlirCamera::FlirCamera(const char *serial_number) :

    is_started_  (false),
    is_recording_(false),
    system_(Spinnaker::System::GetInstance())

    {
        std::unique_lock<std::mutex> lock(init_mutex_);
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

        Spinnaker::GenApi::INodeMap & node_map = camera_->GetNodeMap();

        height_ = this->GetImageFrameHeight();
        width_ = this->GetImageFrameWidth();

        image_handler_ = new PngImageEventHandler(height_, width_, 600);

        // Trigger Mode
        Spinnaker::GenApi::CEnumerationPtr triggerMode = node_map.GetNode("TriggerMode");
        if (IsAvailable(triggerMode) && IsWritable(triggerMode)) {
            triggerMode->SetIntValue(triggerMode->GetEntryByName("Off")->GetValue());
        }

        // Trigger Source
        Spinnaker::GenApi::CEnumerationPtr trigger_source = node_map.GetNode("TriggerSource");
        if (IsAvailable(trigger_source) && IsWritable(trigger_source)) {
            trigger_source->SetIntValue(trigger_source->GetEntryByName("Line2")->GetValue());
        }

        // Trigger Activation
        Spinnaker::GenApi::CEnumerationPtr trigger_activation = node_map.GetNode("TriggerActivation");
        if (IsAvailable(trigger_activation) && IsWritable(trigger_activation)) {
            trigger_activation->SetIntValue(trigger_activation->GetEntryByName("RisingEdge")->GetValue());
        }

        // Trigger Overlap
        Spinnaker::GenApi::CEnumerationPtr trigger_overlap = node_map.GetNode("TriggerOverlap");
        if (IsAvailable(trigger_overlap) && IsWritable(trigger_overlap)) {
            trigger_overlap->SetIntValue(trigger_overlap->GetEntryByName("ReadOut")->GetValue());
        }

        // Exposure Auto
        Spinnaker::GenApi::CEnumerationPtr exposure_auto = node_map.GetNode("ExposureAuto");
        if (IsAvailable(exposure_auto) && IsWritable(exposure_auto)) {
            exposure_auto->SetIntValue(exposure_auto->GetEntryByName("Off")->GetValue());
        }

        // Exposure Time
        Spinnaker::GenApi::CFloatPtr exposure_time = node_map.GetNode("ExposureTime");
        if (IsAvailable(exposure_time) && IsWritable(exposure_time)) {
            exposure_time->SetValue(8000);
        }

        camera_->RegisterEventHandler(*image_handler_);
        cam_list.Clear();
    }

    FlirCamera::~FlirCamera() {
        this->Stop();
        camera_->DeInit();
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
        Spinnaker::GenApi::INodeMap& node_map = camera_->GetNodeMap();
        double exposure_time = 0;
        Spinnaker::GenApi::CFloatPtr exposureTimeNode = node_map.GetNode("ExposureTime");
        if (IsAvailable(exposureTimeNode) && IsReadable(exposureTimeNode)) {
            exposure_time = exposureTimeNode->GetValue();
        }
        return (int)exposure_time;
    }

    int FlirCamera::GetMaxExposureTime() {
        Spinnaker::GenApi::INodeMap& node_map = camera_->GetNodeMap();
        double max_exposure_time = 0;
        Spinnaker::GenApi::CFloatPtr exposureTimeNode = node_map.GetNode("ExposureTime");
        if (IsAvailable(exposureTimeNode) && IsReadable(exposureTimeNode)) {
            max_exposure_time = exposureTimeNode->GetMax();
        }
        return (int)max_exposure_time;
    }

    bool FlirCamera::SetExposureTime(int exposure_time_us) {
        Spinnaker::GenApi::INodeMap& nodeMap = camera_->GetNodeMap();
        Spinnaker::GenApi::CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
        if (IsAvailable(exposureAuto) && IsWritable(exposureAuto)) {
            exposureAuto->FromString("Off");
        }

        Spinnaker::GenApi::CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
        if (IsAvailable(exposureTime) && IsWritable(exposureTime)) {
            //exposureTime->SetValue(exposure_time_us);
            exposureTime->SetValue(exposure_time_us);
        }

        return false;
    }

    bool FlirCamera::GetTriggerMode() {
        Spinnaker::GenApi::INodeMap& node_map = camera_->GetNodeMap();
        std::string trigger_mode = "nan";
        Spinnaker::GenApi::CFloatPtr node = node_map.GetNode("TriggerMode");
        if (IsAvailable(node) && IsReadable(node)) {
            trigger_mode = node->GetValue();
        }
        return false;
    }

    bool FlirCamera::SetTriggerMode(bool trigger_mode) {
        std::string new_mode = trigger_mode ? "On" : "Off";
        Spinnaker::GenApi::INodeMap& node_map = camera_->GetNodeMap();
        Spinnaker::GenApi::CEnumerationPtr triggerMode = node_map.GetNode("TriggerMode");
        if (IsAvailable(triggerMode) && IsWritable(triggerMode)) {
            triggerMode->SetIntValue(triggerMode->GetEntryByName(new_mode.c_str())->GetValue());
        } else {
            return false;
        }

        if (!trigger_mode) {
            return true;
        }

        // In case of activating the trigger, we need to set this too
        Spinnaker::GenApi::CEnumerationPtr trigger_overlap = node_map.GetNode("TriggerOverlap");
        if (IsAvailable(trigger_overlap) && IsWritable(trigger_overlap)) {
            trigger_overlap->SetIntValue(trigger_overlap->GetEntryByName("ReadOut")->GetValue());
        } else {
            return false;
        }
        return true;
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

        std::unique_lock<std::mutex> lock(init_mutex_);

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
        image_handler_->StartRecording(output_dir);
        is_recording_ = true;
        return true;
    }

    bool FlirCamera::StopRecording() {
        if(!is_recording_) {
            return false;
        }
        image_handler_->StopRecording();
        is_recording_ = false;
        return true;
    }

    void FlirCamera::AnalyzeRecording(const char* output_dir) {
        image_handler_->AnalyzeRecording(output_dir);
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