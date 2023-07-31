#ifdef ENABLE_REALSENSE_SDK

#include <boost/filesystem.hpp>

#include "realsense_gui.h"
#include "imgui_helper.h"

namespace rcg::gui::realsense {

    RealSenseGui::RealSenseGui(cams::realsense::RealSenseCamera* realsense_camera, int id) {
        // Camera
        realsense_camera_ = realsense_camera;

        // Information
        identifier_ = "realsense" + std::to_string(id);
        name_ = realsense_camera_->GetDevice().get_info(RS2_CAMERA_INFO_NAME);
        serial_number_ = realsense_camera_->GetDevice().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
        product_line_ = realsense_camera_->GetDevice().get_info(RS2_CAMERA_INFO_PRODUCT_LINE);

        // Display Frame
        display_frame_width_ = realsense_camera_->GetOutputFrameDimensions().first;
        display_frame_height_ = realsense_camera_->GetOutputFrameDimensions().second;

        // GUI
        GenerateTexture(&display_frame_texture_id_, display_frame_width_, display_frame_height_);
        show_visualization_ = false;

        window_main_label_ = identifier_ + ("###" + identifier_ + "window_main");
        window_viz_label_ = identifier_ + ("###" + identifier_ + "window_viz");
        checkbox_viz_label_ = "###" + identifier_ + "checkbox_viz";
        button_start_label_ = "Start" + ("###" + identifier_ + "button_start");
        button_stop_label_ = "Stop" + ("###" + identifier_ + "button_stop");
        button_start_recording_label_ = "Record" + ("###" + identifier_ + "button_start_recording");
        button_stop_recording_label_ = "End" + ("###" + identifier_ + "button_stop_recording");
        collapsing_header_general_label_ = "General" + ("###" + identifier_ + "collapsing_header_general");
        input_text_output_dir_label_ = "###" + identifier_ + "input_text_output_dir";

        memset(input_text_output_dir_, 0, BUF_SIZE);
    }

    RealSenseGui::~RealSenseGui() {
        DeleteTexture(&display_frame_texture_id_);
    }

    void RealSenseGui::Show(bool *show) {
        // Visualize display frame
        if(show_visualization_) {
            realsense_camera_->OutputFrame(display_frame_);
            UpdateTexture(&display_frame_texture_id_, display_frame_.data, display_frame_width_, display_frame_height_);
            DisplayImage(&display_frame_texture_id_, window_viz_label_.c_str(), display_frame_width_, display_frame_height_, &show_visualization_);
        }

        if(show == nullptr || *(show)) {
            // Display camera information
            ImGui::Begin(identifier_.c_str(), show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            ImGui::Text("Name         : %s", name_.c_str());
            ImGui::Spacing();
            ImGui::Text("Serial number: %s", serial_number_.c_str());
            ImGui::Spacing();
            ImGui::Text("Product line : %s", product_line_.c_str());
            ImGui::Spacing();

            // Start or stop camera
            ImGui::BeginDisabled(realsense_camera_->IsStarted());
            if(ImGui::Button(button_start_label_.c_str())) {
                realsense_camera_->Start();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!realsense_camera_->IsStarted() || realsense_camera_->IsRecording());
            if(ImGui::Button(button_stop_label_.c_str())) {
                realsense_camera_->Stop();
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Display general settings
            if(realsense_camera_->IsStarted()) {
                if(ImGui::CollapsingHeader(collapsing_header_general_label_.c_str())) {
                    ImGui::Spacing();

                    // Visualization
                    ImGui::Text("visualize    ");
                    ImGui::SameLine();
                    ImGui::Checkbox(checkbox_viz_label_.c_str(), &show_visualization_);
                    ImGui::Spacing();

                    // Recording
                    ImGui::BeginDisabled(realsense_camera_->IsRecording());
                    ImGui::Text("output_dir   ");
                    ImGui::SameLine();
                    ImGui::InputText(input_text_output_dir_label_.c_str(), input_text_output_dir_, BUF_SIZE);
                    ImGui::EndDisabled();
                    ImGui::Spacing();

                    ImGui::BeginDisabled(realsense_camera_->IsRecording());
                    if(ImGui::Button(button_start_recording_label_.c_str())) {
                        output_dir_ = std::string {input_text_output_dir_};
                        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

                        output_dir_ += "/" + identifier_;
                        boost::filesystem::create_directory(output_dir_);
                        realsense_camera_->StartRecording(output_dir_.c_str());
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!realsense_camera_->IsRecording());
                    if(ImGui::Button(button_stop_recording_label_.c_str())) {
                        realsense_camera_->StopRecording();
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                }
            }

            ImGui::End();
        }
    }

} // rcg::gui::realsense

#endif //ENABLE_REALSENSE_SDK