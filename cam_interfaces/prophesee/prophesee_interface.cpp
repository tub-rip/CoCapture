#ifdef ENABLE_METAVISION_SDK

#include "prophesee_interface.h"

namespace rcg::cam_interfaces::prophesee {

    PropheseeInterface::PropheseeInterface(cams::prophesee::PropheseeCamera* prophesee_camera, int id) {
        prophesee_camera_ = prophesee_camera;
        identifier_ = "prophesee" + std::to_string(id);
    }

    PropheseeInterface::~PropheseeInterface() {}

    const char* PropheseeInterface::GetIdentifier() {
        return identifier_.c_str();
    }

    bool PropheseeInterface::Start() {
        return prophesee_camera_->Start();
    }

    bool PropheseeInterface::Stop() {
        return prophesee_camera_->Stop();
    }

    bool PropheseeInterface::StartRecording(const char* output_dir) {
        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

        output_dir_ += "/" + identifier_;
        boost::filesystem::create_directory(output_dir_);
        return prophesee_camera_->StartRecording(output_dir_.c_str());
    }

    bool PropheseeInterface::StopRecording() {
        bool stop_recording {prophesee_camera_->StopRecording()};

        if(stop_recording) {
            prophesee_camera_->AnalyzeRecording(output_dir_.c_str());
        }

        return stop_recording;
    }

    bool PropheseeInterface::IsStarted() {
        return prophesee_camera_->IsStarted();
    }

    bool PropheseeInterface::IsRecording() {
        return prophesee_camera_->IsRecording();
    }

} // rcg::cam_interfaces::prophesee

#endif //ENABLE_METAVISION_SDK