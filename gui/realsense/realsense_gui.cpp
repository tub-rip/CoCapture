#ifdef ENABLE_REALSENSE_SDK

#include "realsense_gui.h"
#include "imgui_helper.h"

namespace rcg::gui::realsense {

    RealSenseGui::RealSenseGui(const char *serial_number, int id) {
        // Camera
        realsense_camera_ = std::make_unique<cams::realsense::RealSenseCamera>(serial_number);

        // Information
        std::string identifier = "realsense" + std::to_string(id);
        std::string serial_number_str = std::string {serial_number};

        strncpy(identifier_, identifier.c_str(), identifier.size() + 1);
        strncpy(serial_number_, serial_number_str.c_str(), serial_number_str.size() + 1);

        // GUI
        std::string collapsing_header_general_label = "General" + ("###" + identifier + "collapsing_header_general");
        strncpy(collapsing_header_general_label_, collapsing_header_general_label.c_str(), collapsing_header_general_label.size() + 1);
        std::string window_main_label = identifier + ("###" + identifier + "window_main");
        strncpy(window_main_label_, window_main_label.c_str(), window_main_label.size() + 1);

        std::string window_viz_color_label = identifier + " " + "[Color]" + ("###" + identifier + "window_viz_color");
        strncpy(window_viz_color_label_, window_viz_color_label.c_str(), window_viz_color_label.size() + 1);
        std::string checkbox_viz_color_label = "###" + identifier + "checkbox_viz_color";
        strncpy(checkbox_viz_color_label_, checkbox_viz_color_label.c_str(), checkbox_viz_color_label.size() + 1);

        std::string window_viz_depth_label = identifier + " " + "[Depth]" + ("###" + identifier + "window_viz_depth");
        strncpy(window_viz_depth_label_, window_viz_depth_label.c_str(), window_viz_depth_label.size() + 1);
        std::string checkbox_viz_depth_label = "###" + identifier + "checkbox_viz_depth";
        strncpy(checkbox_viz_depth_label_, checkbox_viz_depth_label.c_str(), checkbox_viz_depth_label.size() + 1);

        std::string window_viz_infrared_label = identifier + " " + "[Infrared]" + ("###" + identifier + "window_viz_infrared");
        strncpy(window_viz_infrared_label_, window_viz_infrared_label.c_str(), window_viz_infrared_label.size() + 1);
        std::string checkbox_viz_infrared_label = "###" + identifier + "checkbox_viz_infrared";
        strncpy(checkbox_viz_infrared_label_, checkbox_viz_infrared_label.c_str(), checkbox_viz_infrared_label.size() + 1);

        color_frame_dimensions_ = realsense_camera_->GetFrameDimensions(cams::realsense::FRAME_COLOR);
        GenerateTextureRGB8(&color_frame_tid_, color_frame_dimensions_.first, color_frame_dimensions_.second);
        show_viz_color_ = false;

        depth_frame_dimensions_ = realsense_camera_->GetFrameDimensions(cams::realsense::FRAME_DEPTH);
        GenerateTextureRGB8(&depth_frame_tid_, depth_frame_dimensions_.first, depth_frame_dimensions_.second);
        show_viz_depth_ = false;

        infrared_frame_dimensions_ = realsense_camera_->GetFrameDimensions(cams::realsense::FRAME_INFRARED);
        GenerateTextureRGB8(&infrared_frame_tid_, infrared_frame_dimensions_.first, infrared_frame_dimensions_.second);
        show_viz_infrared_ = false;
    }

    RealSenseGui::~RealSenseGui() {
        DeleteTexture(&color_frame_tid_);
        DeleteTexture(&depth_frame_tid_);
        DeleteTexture(&infrared_frame_tid_);
    }

    void RealSenseGui::Show(bool* show) {
        // Display color frame
        if(show_viz_color_) {
            realsense_camera_->OutputFrame(cams::realsense::FRAME_COLOR, color_frame_);
            UpdateTextureRGB8(&color_frame_tid_, color_frame_.data, color_frame_dimensions_.first, color_frame_dimensions_.second);
            DisplayImage(&color_frame_tid_, window_viz_color_label_, color_frame_dimensions_.first, color_frame_dimensions_.second, &show_viz_color_);
        }

        // Display depth frame
        if(show_viz_depth_) {
            realsense_camera_->OutputFrame(cams::realsense::FRAME_DEPTH, depth_frame_);
            UpdateTextureRGB8(&depth_frame_tid_, depth_frame_.data, depth_frame_dimensions_.first, depth_frame_dimensions_.second);
            DisplayImage(&depth_frame_tid_, window_viz_depth_label_, depth_frame_dimensions_.first, depth_frame_dimensions_.second, &show_viz_depth_);
        }

        // Display infrared frame
        if(show_viz_infrared_) {
            realsense_camera_->OutputFrame(cams::realsense::FRAME_INFRARED, infrared_frame_);
            UpdateTextureRGB8(&infrared_frame_tid_, infrared_frame_.data, infrared_frame_dimensions_.first, infrared_frame_dimensions_.second);
            DisplayImage(&infrared_frame_tid_, window_viz_infrared_label_, infrared_frame_dimensions_.first, infrared_frame_dimensions_.second, &show_viz_infrared_);
        }

        if(*(show)) {
            ImGui::Begin(identifier_, show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            ImGui::Text("Identifier    : %s", identifier_);
            ImGui::Spacing();
            ImGui::Text("Serial number : %s", serial_number_);
            ImGui::Spacing();

            if (ImGui::CollapsingHeader(collapsing_header_general_label_)) {
                ImGui::Spacing();

                ImGui::Text("Color        ");
                ImGui::SameLine();
                ImGui::Checkbox(checkbox_viz_color_label_, &show_viz_color_);
                ImGui::Spacing();

                ImGui::Text("Depth        ");
                ImGui::SameLine();
                ImGui::Checkbox(checkbox_viz_depth_label_, &show_viz_depth_);
                ImGui::Spacing();

                ImGui::Text("Infrared     ");
                ImGui::SameLine();
                ImGui::Checkbox(checkbox_viz_infrared_label_, &show_viz_infrared_);
                ImGui::Spacing();
            }

            ImGui::End();
        }
    }

    void RealSenseGui::StartRecording(const char* output_dir) {
        std::string output_dir_str = std::string {output_dir};
        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }
        std::string recording_dir = output_dir_str + "/" + std::string {identifier_};
        boost::filesystem::create_directories(recording_dir);
        realsense_camera_->StartRecording(recording_dir.c_str());
    }

    void RealSenseGui::StopRecording() {
        realsense_camera_->StopRecording();
    }

} // rcg::gui::realsense

#endif //ENABLE_REALSENSE_SDK