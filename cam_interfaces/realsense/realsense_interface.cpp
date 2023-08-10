#ifdef ENABLE_REALSENSE_SDK

#include <boost/filesystem.hpp>

#include "realsense_interface.h"

namespace rcg::cam_interfaces::realsense {

    RealSenseInterface::RealSenseInterface(cams::realsense::RealSenseCamera* realsense_camera, int id) {
        realsense_camera_ = realsense_camera;
        identifier_ = "realsense" + std::to_string(id);
    }

    RealSenseInterface::~RealSenseInterface() {}

    const char* RealSenseInterface::GetIdentifier() {
        return identifier_.c_str();
    }

    bool RealSenseInterface::Start() {
        return realsense_camera_->Start();
    }

    bool RealSenseInterface::Stop() {
        return realsense_camera_->Stop();
    }

    bool RealSenseInterface::StartRecording(const char* output_dir) {
        output_dir_ = std::string {output_dir};
        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

        output_dir_ += "/" + identifier_;
        boost::filesystem::create_directory(output_dir_);
        return realsense_camera_->StartRecording(output_dir_.c_str());
    }

    bool RealSenseInterface::StopRecording() {
        return realsense_camera_->StopRecording();
    }

    bool RealSenseInterface::IsStarted() {
        return realsense_camera_->IsStarted();
    }

    bool RealSenseInterface::IsRecording() {
        return realsense_camera_->IsRecording();
    }

} // rcg::cam_interfaces::realsense

#endif //ENABLE_REALSENSE_SDK