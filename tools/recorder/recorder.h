#ifndef RIP_COCAPTURE_GUI_RECORDER_H
#define RIP_COCAPTURE_GUI_RECORDER_H

#include <chrono>
#include <vector>

#include <boost/filesystem.hpp>

#include "imgui_helper.h"
#include "camera_gui.h"
#include "camera_wrapper.h"

namespace rcg::tools {

    class Recorder {
    public:
        Recorder();
        ~Recorder();

    public:
        std::string GetCurrentTimeStamp();
        void RegisterWrapper(wrappers::ICameraWrapper& camera_wrapper, bool& show);
        bool StartRecording(const char* output_dir);
        bool StopRecording();
        void Show(bool* show = nullptr);

    private:
        std::vector<std::pair<wrappers::ICameraWrapper&, bool&>> camera_wrappers_;
        bool is_recording_;
        bool is_available_;
        char output_dir_[gui::BUF_SIZE];
        char output_tag_[gui::BUF_SIZE];
        std::chrono::time_point<std::chrono::steady_clock> timer_start_;
        std::pair<int, int> timer_duration_s_;
        bool timer_on_;
    };

} // rcg::tools

#endif //RIP_COCAPTURE_GUI_RECORDER_H