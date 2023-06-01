#include <boost/filesystem.hpp>

// Metavision SDK
#ifdef ENABLE_METAVISION_SDK
#include "prophesee_gui.h"
#endif //ENABLE_METAVISION_SDK

// RealSense SDK
#ifdef ENABLE_REALSENSE_SDK
#include "realsense_gui.h"
#endif //ENABLE_REALSENSE_SDK

#include "imgui_helper.h"

using namespace rcg;

std::string GetTimeStamp() {
    time_t raw_time;
    struct tm* time_info;
    char time_stamp[64];

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(time_stamp, sizeof(time_stamp), "%y%m%d_%H%M%S", time_info);

    return std::string {time_stamp};
}

int main() {
    gui::Initialize();
    ImGui::StyleColorsLight();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    gui::CreateWindow("Recorder", display_mode.w, display_mode.h);

    // Set up user interfaces
    bool is_recording = false;

    gui::prophesee::PropheseeGui prophesee_gui_0 = gui::prophesee::PropheseeGui("00050131", 0);
    bool show_prophesee_gui_0 = false;

    gui::prophesee::PropheseeGui prophesee_gui_1 = gui::prophesee::PropheseeGui("00050138", 1);
    bool show_prophesee_gui_1 = false;

    gui::realsense::RealSenseGui realsense_gui_0 = gui::realsense::RealSenseGui("f1150775", 0);
    bool show_realsense_gui_0 = false;

    bool done = false;
    while(!done) {
        gui::HandleEvent(done);
        gui::NewFrame();

        // Render user interfaces
        prophesee_gui_0.Show(&show_prophesee_gui_0);
        prophesee_gui_1.Show(&show_prophesee_gui_1);
        realsense_gui_0.Show(&show_realsense_gui_0);

        // Render simple controls
        ImGui::Begin("Recorder", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Spacing();

        ImGui::Text("prophesee0 ");
        ImGui::SameLine();
        ImGui::Checkbox("###prophesee0", &show_prophesee_gui_0);
        ImGui::Spacing();

        ImGui::Text("prophesee1 ");
        ImGui::SameLine();
        ImGui::Checkbox("###prophesee1", &show_prophesee_gui_1);
        ImGui::Spacing();

        ImGui::Text("realsense0 ");
        ImGui::SameLine();
        ImGui::Checkbox("###realsense0", &show_realsense_gui_0);
        ImGui::Spacing();

        // Start or stop recording
        ImGui::BeginDisabled(is_recording);
        if(ImGui::Button("Record")) {
            std::string time_stamp = GetTimeStamp();
            prophesee_gui_0.StartRecording(time_stamp.c_str());
            prophesee_gui_1.StartRecording(time_stamp.c_str());
            realsense_gui_0.StartRecording(time_stamp.c_str());
            is_recording = true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(!is_recording);
        if(ImGui::Button("End")) {
            prophesee_gui_0.StopRecording();
            prophesee_gui_1.StopRecording();
            realsense_gui_0.StopRecording();
            is_recording = false;
        }
        ImGui::EndDisabled();
        ImGui::Spacing();

        ImGui::End();

        gui::EndFrame();
    }

    gui::DestroyWindow();
    gui::Terminate();
}