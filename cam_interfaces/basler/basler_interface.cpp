#ifdef ENABLE_PYLON_SDK

#include <boost/filesystem.hpp>

#include "basler_interface.h"

namespace rcg::cam_interfaces::basler {

    BaslerInterface::BaslerInterface(cams::basler::BaslerCamera* basler_camera, int id) {
        basler_camera_ = basler_camera;
        identifier_ = "basler" + std::to_string(id);
    }

    BaslerInterface::~BaslerInterface() {}

    const char* BaslerInterface::GetIdentifier() {
        return identifier_.c_str();
    }

    bool BaslerInterface::Start() {
        return basler_camera_->Start();
    }

    bool BaslerInterface::Stop() {
        return basler_camera_->Stop();
    }

    bool BaslerInterface::StartRecording(const char* output_dir) {
        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

        output_dir_ += "/" + identifier_;
        boost::filesystem::create_directory(output_dir_);
        return basler_camera_->StartRecording(output_dir_.c_str());
    }

    bool BaslerInterface::StopRecording() {
        bool stop_recording {basler_camera_->StopRecording()};

        if(stop_recording) {
            basler_camera_->AnalyzeRecording(output_dir_.c_str());
        }

        return stop_recording;
    }

    bool BaslerInterface::IsStarted() {
        return basler_camera_->IsStarted();
    }

    bool BaslerInterface::IsRecording() {
        return basler_camera_->IsRecording();
    }

} // rcg::cam_interfaces::basler

#endif //ENABLE_PYLON_SDK