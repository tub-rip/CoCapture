#ifdef ENABLE_METAVISION_SDK

#include "prophesee_gui.h"
#include "imgui_helper.h"

namespace rcg::gui::prophesee {

    PropheseeGui::PropheseeGui(cams::prophesee::PropheseeCamera* prophesee_camera, int id) {
        // Camera
        prophesee_camera_ = prophesee_camera;

        // Information
        std::string identifier = "prophesee" + std::to_string(id);
        std::string serial_number_str = prophesee_camera_->GetSerialNumber();
        std::string generation = prophesee_camera_->GetGeneration();
        std::string plugin_name = prophesee_camera_->GetPluginName();
        std::string mode = prophesee_camera_->GetMode();

        strncpy(identifier_, identifier.c_str(), identifier.size() + 1);
        strncpy(serial_number_, serial_number_str.c_str(), serial_number_str.size() + 1);
        strncpy(generation_, generation.c_str(), generation.size() + 1);
        strncpy(plugin_name_, plugin_name.c_str(), plugin_name.size() + 1);
        strncpy(mode_, mode.c_str(), mode.size() + 1);

        // CD Frame
        cd_frame_width_ = prophesee_camera_->GetCDFrameWidth();
        cd_frame_height_ = prophesee_camera_->GetCDFrameHeight();

        // Biases
        std::map<std::string, std::pair<int, int>> bias_ranges = prophesee_camera_->ReadBiasRanges("../cams/prophesee/bias_ranges.txt",
                                                                                                   prophesee_camera_->GetGeneration().c_str());

        bias_diff_off_ = prophesee_camera_->GetBiasValue("bias_diff_off");
        bias_diff_on_ = prophesee_camera_->GetBiasValue("bias_diff_on");
        bias_fo_ = prophesee_camera_->GetBiasValue("bias_fo");
        bias_hpf_ = prophesee_camera_->GetBiasValue("bias_hpf");
        bias_refr_ = prophesee_camera_->GetBiasValue("bias_refr");

        bias_diff_off_range_ = bias_ranges["bias_diff_off"];
        bias_diff_on_range_ = bias_ranges["bias_diff_on"];
        bias_fo_range_ = bias_ranges["bias_fo"];
        bias_hpf_range_ = bias_ranges["bias_hpf"];
        bias_refr_range_ = bias_ranges["bias_refr"];

        // GUI
        GenerateTexture(&cd_frame_texture_id_, cd_frame_width_, cd_frame_height_);
        show_visualization_ = false;
        mode_selected_ = 0;

        std::string window_main_label = identifier + ("###" + identifier + "window_main");
        std::string window_viz_label = identifier + ("###" + identifier + "window_viz");
        std::string checkbox_viz_label = "###" + identifier + "checkbox_viz";
        std::string combo_modes_label = "###" + identifier + "combo_modes";
        std::string button_start_label = "Start" + ("###" + identifier + "button_start");
        std::string button_stop_label = "Stop" + ("###" + identifier + "button_stop");
        std::string button_start_recording_label = "Record" + ("###" + identifier + "button_start_recording");
        std::string button_stop_recording_label = "End" + ("###" + identifier + "button_stop_recording");
        std::string collapsing_header_general_label = "General" + ("###" + identifier + "collapsing_header_general");
        std::string slider_bias_diff_off_label = "###" + identifier + "slider_bias_diff_off";
        std::string slider_bias_diff_on_label = "###" + identifier + "slider_bias_diff_on";
        std::string slider_bias_fo_label = "###" + identifier + "slider_bias_fo";
        std::string slider_bias_hpf_label = "###" + identifier + "slider_bias_hpf";
        std::string slider_bias_refr_label = "###" + identifier + "slider_bias_refr";
        std::string input_text_output_dir_label = "###" + identifier + "input_text_output_dir";

        strncpy(window_main_label_, window_main_label.c_str(), window_main_label.size() + 1);
        strncpy(window_viz_label_, window_viz_label.c_str(), window_viz_label.size() + 1);
        strncpy(checkbox_viz_label_, checkbox_viz_label.c_str(), checkbox_viz_label.size() + 1);
        strncpy(combo_modes_label_, combo_modes_label.c_str(), combo_modes_label.size() + 1);
        strncpy(button_start_label_, button_start_label.c_str(), button_start_label.size() + 1);
        strncpy(button_stop_label_, button_stop_label.c_str(), button_stop_label.size() + 1);
        strncpy(button_start_recording_label_, button_start_recording_label.c_str(), button_start_recording_label.size() + 1);
        strncpy(button_stop_recording_label_, button_stop_recording_label.c_str(), button_stop_recording_label.size() + 1);
        strncpy(collapsing_header_general_label_, collapsing_header_general_label.c_str(), collapsing_header_general_label.size() + 1);
        strncpy(slider_bias_diff_off_label_, slider_bias_diff_off_label.c_str(), slider_bias_diff_off_label.size() + 1);
        strncpy(slider_bias_diff_on_label_, slider_bias_diff_on_label.c_str(), slider_bias_diff_on_label.size() + 1);
        strncpy(slider_bias_fo_label_, slider_bias_fo_label.c_str(), slider_bias_fo_label.size() + 1);
        strncpy(slider_bias_hpf_label_, slider_bias_hpf_label.c_str(), slider_bias_hpf_label.size() + 1);
        strncpy(slider_bias_refr_label_, slider_bias_refr_label.c_str(), slider_bias_refr_label.size() + 1);
        strncpy(input_text_output_dir_label_, input_text_output_dir_label.c_str(), input_text_output_dir_label.size() + 1);

        memset(input_text_output_dir_, 0, BUF_SIZE);
    }

    PropheseeGui::~PropheseeGui() {
        DeleteTexture(&cd_frame_texture_id_);
    }

    void PropheseeGui::Show(bool* show) {
        // Display CD frame
        if(show_visualization_) {
            prophesee_camera_->OutputCDFrame(cd_frame_);
            UpdateTexture(&cd_frame_texture_id_, cd_frame_.data, cd_frame_width_, cd_frame_height_);
            DisplayImage(&cd_frame_texture_id_, window_viz_label_, cd_frame_width_, cd_frame_height_, &show_visualization_);
        }

        if(show == nullptr || *(show)) {
            // Display camera information
            ImGui::Begin(identifier_, show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            ImGui::Text("Identifier           : %s", identifier_);
            ImGui::Spacing();
            ImGui::Text("Serial number        : %s", serial_number_);
            ImGui::Spacing();
            ImGui::Text("Generation           : %s", generation_);
            ImGui::Spacing();
            ImGui::Text("Plugin name          : %s", plugin_name_);
            ImGui::Spacing();
            ImGui::Text("Resolution           : %d * %d", cd_frame_width_, cd_frame_height_);
            ImGui::Spacing();
            ImGui::BeginDisabled(prophesee_camera_->IsStarted());
            ImGui::Text("Synchronization mode");
            ImGui::SameLine();
            if(ImGui::Combo(combo_modes_label_, &mode_selected_, modes_, IM_ARRAYSIZE(modes_))) {
                prophesee_camera_->SetMode(modes_[mode_selected_]);
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Start or stop camera
            ImGui::BeginDisabled(prophesee_camera_->IsStarted() || !mode_selected_);
            if(ImGui::Button(button_start_label_)) {
                prophesee_camera_->Start();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!prophesee_camera_->IsStarted() || prophesee_camera_->IsRecording());
            if(ImGui::Button(button_stop_label_)) {
                prophesee_camera_->Stop();
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Display general settings
            if(prophesee_camera_->IsStarted()) {
                if (ImGui::CollapsingHeader(collapsing_header_general_label_)) {
                    ImGui::Spacing();

                    // Visualization
                    ImGui::Text("visualize    ");
                    ImGui::SameLine();
                    ImGui::Checkbox(checkbox_viz_label_, &show_visualization_);
                    ImGui::Spacing();

                    // Bias settings
                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    ImGui::Text("bias_diff_off");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_diff_off_label_, &bias_diff_off_, bias_diff_off_range_.first, bias_diff_off_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_diff_off", bias_diff_off_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_diff_on ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_diff_on_label_, &bias_diff_on_, bias_diff_on_range_.first, bias_diff_on_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_diff_on", bias_diff_on_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_fo      ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_fo_label_, &bias_fo_, bias_fo_range_.first, bias_fo_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_fo", bias_fo_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_hpf     ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_hpf_label_, &bias_hpf_, bias_hpf_range_.first, bias_hpf_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_hpf", bias_hpf_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_refr    ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_refr_label_, &bias_refr_, bias_refr_range_.first, bias_refr_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_refr", bias_refr_);
                    }
                    ImGui::Spacing();
                    ImGui::EndDisabled();

                    // Recording
                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    ImGui::Text("output_dir   ");
                    ImGui::SameLine();
                    ImGui::InputText(input_text_output_dir_label_, input_text_output_dir_, BUF_SIZE);
                    ImGui::EndDisabled();
                    ImGui::Spacing();

                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    if(ImGui::Button(button_start_recording_label_)) {
                        prophesee_camera_->StartRecording(input_text_output_dir_);
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!prophesee_camera_->IsRecording());
                    if(ImGui::Button(button_stop_recording_label_)) {
                        prophesee_camera_->StopRecording();
                        prophesee_camera_->AnalyzeRecording(input_text_output_dir_);
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                }
            }

            ImGui::End();
        }
    }

} // rcg::gui::prophesee

#endif //ENABLE_METAVISION_SDK