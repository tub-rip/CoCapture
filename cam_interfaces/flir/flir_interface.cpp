#ifdef ENABLE_SPINNAKER_SDK

#include <boost/filesystem.hpp>

#include "flir_interface.h"

namespace rcg::cam_interfaces::flir {

//FlirInterface::FlirInterface(cams::flir::FlirCamera* flir_camera, int id) {
//    flir_camera_ = flir_camera;
//    identifier_ = "flir" + std::to_string(id);
//}

    FlirInterface::~FlirInterface() {}

    const char* FlirInterface::GetIdentifier() {
        return identifier_.c_str();
    }

    bool FlirInterface::Start() {
        //return flir_camera_->Start();
        return false;
    }

    bool FlirInterface::Stop() {
        //return flir_camera_->Stop();
        return false;
    }

    bool FlirInterface::StartRecording(const char* output_dir) {
        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

        output_dir_ += "/" + identifier_;
        boost::filesystem::create_directory(output_dir_);
        //return flir_camera_->StartRecording(output_dir_.c_str());
        return false;
    }

    bool FlirInterface::StopRecording() {
        //bool stop_recording {flir_camera_->StopRecording()};
//
        //if(stop_recording) {
        //    flir_camera_->AnalyzeRecording(output_dir_.c_str());
        //}

        //return stop_recording;
        return false;
    }

    bool FlirInterface::IsStarted() {
        //return flir_camera_->IsStarted();
        return false;
    }

    bool FlirInterface::IsRecording() {
        //return flir_camera_->IsRecording();
        return false;
    }

} // rcg::cam_interfaces::flir

#endif //ENABLE_PYLON_SDK