#include <chrono>

#include "imgui_helper.h"
#include "prophesee_wrapper.h"
#include "basler_wrapper.h"

using namespace rcg;

class Recorder {
public:
    Recorder() : is_recording_(false), timer_duration_s_({0, 0}), timer_on_(false) {
        memset(output_dir_, 0, gui::BUF_SIZE);
        memset(output_tag_, 0, gui::BUF_SIZE);
    }

    ~Recorder() {}

public:
    std::string GetCurrentTimeStamp() {
        time_t raw_time;
        struct tm* time_info;
        char time_stamp[gui::BUF_SIZE];

        time(&raw_time);
        time_info = localtime(&raw_time);

        strftime(time_stamp, sizeof(time_stamp), "%y%m%d_%H%M%S", time_info);

        return std::string {time_stamp};
    }

    void RegisterWrapper(wrappers::ICameraWrapper& camera_wrapper, bool& show) {
        camera_wrappers_.push_back({camera_wrapper, show});
    }

    bool StartRecording(const char* output_dir) {
        if(is_recording_) {
            return false;
        }

        for(const auto& camera_wrapper : camera_wrappers_) {
            if(camera_wrapper.second) {
                camera_wrapper.first.GetInterface().StartRecording(output_dir);
            }
        }

        is_recording_ = true;

        return true;
    }

    bool StopRecording() {
        if(!is_recording_) {
            return false;
        }

        for(const auto& camera_wrapper : camera_wrappers_) {
            if(camera_wrapper.second) {
                camera_wrapper.first.GetInterface().StopRecording();
            }
        }

        is_recording_ = false;

        return true;
    }

    void Show(bool* show = nullptr) {
        if(show == nullptr || *(show)) {
            ImGui::Begin("Recorder", show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            // Check availability
            is_available_ = true;
            int show_counter = 0;
            for(const auto& camera_wrapper : camera_wrappers_) {
                if(camera_wrapper.second) {
                    if(camera_wrapper.first.GetInterface().IsRecording() || !camera_wrapper.first.GetInterface().IsStarted()) {
                        is_available_ = false;
                    }
                    ++show_counter;
                }
            }

            if(show_counter == 0) {
                is_available_ = false;
            }

            // Display recorder status
            if(is_recording_) {
                ImGui::Text("Status: Recording %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
            } else {
                if(is_available_) {
                    ImGui::Text("Status: Available");
                } else {
                    ImGui::Text("Status: Unavailable");
                }
            }
            ImGui::Spacing();

            ImGui::Separator();
            ImGui::Spacing();

            // Set output configurations
            ImGui::BeginDisabled(is_recording_ || !is_available_);
            ImGui::Text("output_dir");
            ImGui::SameLine();
            ImGui::InputText("###Recorderoutput_dir", output_dir_, gui::BUF_SIZE);
            ImGui::Spacing();

            ImGui::Text("output_tag");
            ImGui::SameLine();
            ImGui::InputText("###Recorderoutput_tag", output_tag_, gui::BUF_SIZE);
            ImGui::Spacing();
            ImGui::EndDisabled();

            ImGui::Separator();
            ImGui::Spacing();

            // Select camera(s)
            for(const auto& camera_wrapper : camera_wrappers_) {
                ImGui::BeginDisabled(is_recording_);
                camera_wrapper.first.GetGui().Show(&camera_wrapper.second);

                ImGui::Checkbox(camera_wrapper.first.GetInterface().GetIdentifier(), &camera_wrapper.second);
                ImGui::Spacing();

                ImGui::EndDisabled();
            }

            ImGui::Separator();
            ImGui::Spacing();

            // Update timer
            if(timer_on_) {
                timer_duration_s_.first = timer_duration_s_.second - std::chrono::duration_cast<std::chrono::seconds>
                                                                     (std::chrono::steady_clock::now() - timer_start_).count();

                if(timer_duration_s_.first == 0) {
                    StopRecording();
                    timer_on_ = false;
                }
            }

            ImGui::BeginDisabled(is_recording_ || !is_available_);
            ImGui::Text("duration_s");
            ImGui::SameLine();
            ImGui::InputInt("###Recorderduration_s", &timer_duration_s_.first);
            ImGui::EndDisabled();
            ImGui::Spacing();

            ImGui::Separator();
            ImGui::Spacing();

            // Start or stop recording
            ImGui::BeginDisabled(is_recording_ || !is_available_);
            if(ImGui::Button("Record")) {
                std::string output_dir = std::string {output_dir_} == "" ? "." : std::string {output_dir_};
                if(output_dir.back() == '/') {
                    output_dir.pop_back();
                }

                std::string output_tag = std::string {output_tag_} == "" ? "" : "_" + std::string {output_tag_};

                output_dir = output_dir + "/" + GetCurrentTimeStamp() + output_tag;
                boost::filesystem::create_directory(output_dir);

                StartRecording(output_dir.c_str());

                if(timer_duration_s_.first > 0) {
                    timer_start_ = std::chrono::steady_clock::now();
                    timer_duration_s_.second = timer_duration_s_.first;
                    timer_on_ = true;
                }
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!is_recording_ || timer_on_);
            if(ImGui::Button("End")) {
                StopRecording();
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            ImGui::End();
        }
    }

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

int main() {
    gui::Initialize();

    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsLight();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    gui::CreateWindow("Recorder", display_mode.w, display_mode.h);

    // Set up camera(s)
    wrappers::prophesee::PropheseeWrapper psee_wrapper_1 {"00042177", 0};
    bool show_psee_1 = false;

    wrappers::basler::BaslerWrapper basler_wrapper_1 {"24162143", 0};
    bool show_basler_1 = false;

    // Set up recorder
    Recorder recorder;
    bool show_recorder = true;

    // Register camera(s) to the recorder
    recorder.RegisterWrapper(psee_wrapper_1, show_psee_1);
    recorder.RegisterWrapper(basler_wrapper_1, show_basler_1);

    bool done = false;
    while(!done) {
        gui::HandleEvent(done);
        gui::NewFrame();

        // Display recorder
        recorder.Show(&show_recorder);

        gui::EndFrame();
    }

    gui::DestroyWindow();
    gui::Terminate();
}