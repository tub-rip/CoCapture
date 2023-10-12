#ifdef ENABLE_SPINNAKER_SDK

#include <boost/filesystem.hpp>

#include "flir_gui.h"
#include "imgui_helper.h"

namespace rcg::gui::flir {

    FlirGui::FlirGui(cams::flir::FlirCamera* flir_camera, int id) {
        // Camera
        flir_camera_ = flir_camera;

        // Information
        identifier_ = "flir" + std::to_string(id);
        serial_number_ = flir_camera_->GetSerialNumber();
        model_name_ = flir_camera_->GetModelName();
        vendor_name_ = flir_camera_->GetVendorName();
        max_exposure_time_ = flir_camera_->GetMaxExposureTime();

        // Image Frame
        image_frame_width_ = flir_camera_->GetImageFrameWidth();
        image_frame_height_ = flir_camera_->GetImageFrameHeight();

        // Settings
        exposure_time_ = flir_camera_->GetExposureTime();
        trigger_mode_ = flir_camera->GetTriggerMode();
        //reverse_x_ = flir_camera_->GetReverseX();

        // GUI
        GenerateTexture(&image_frame_texture_id_, image_frame_width_, image_frame_height_);
        show_visualization_ = false;

        window_main_label_ = identifier_ + ("###" + identifier_ + "window_main");
        window_viz_label_ = identifier_ + ("###" + identifier_ + "window_viz");
        checkbox_viz_label_ = "###" + identifier_ + "checkbox_viz";
        button_start_label_ = "Start" + ("###" + identifier_ + "button_start");
        button_stop_label_ = "Stop" + ("###" + identifier_ + "button_stop");
        button_start_recording_label_ = "Record" + ("###" + identifier_ + "button_start_recording");
        button_stop_recording_label_ = "End" + ("###" + identifier_ + "button_stop_recording");
        collapsing_header_general_label_ = "General" + ("###" + identifier_ + "collapsing_header_general");
        slider_exposure_time_label_ = "###" + identifier_ + "slider_exposure_time";
        checkbox_trigger_mode_label_ = "###" + identifier_ + "checkbox_trigger_mode";
        //checkbox_reverse_x_label_ = "###" + identifier_ + "checkbox_reverse_x";
        input_text_output_dir_label_ = "###" + identifier_ + "input_text_output_dir";

        memset(input_text_output_dir_, 0, BUF_SIZE);
    }

    FlirGui::~FlirGui() {
        DeleteTexture(&image_frame_texture_id_);
    }

    void FlirGui::Show(bool* show) {
        // Display image frame
        if(show_visualization_) {
            flir_camera_->OutputFrame(image_frame_);

            UpdateTexture(&image_frame_texture_id_,
                          image_frame_.data, image_frame_width_,
                          image_frame_height_);
            DisplayImage(&image_frame_texture_id_, window_viz_label_.c_str(),
                         image_frame_width_/4, image_frame_height_/4,
                         &show_visualization_);
        }

        if(show == nullptr || *(show)) {
            // Display camera information
            ImGui::Begin(identifier_.c_str(), show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            ImGui::Text("Identifier   : %s", identifier_.c_str());
            ImGui::Spacing();
            ImGui::Text("Serial number: %s", serial_number_.c_str());
            ImGui::Spacing();
            ImGui::Text("Model name   : %s", model_name_.c_str());
            ImGui::Spacing();
            ImGui::Text("Vendor name  : %s", vendor_name_.c_str());
            ImGui::Spacing();
            ImGui::Text("Resolution   : %d * %d", image_frame_width_, image_frame_height_);
            ImGui::Spacing();

            // Start or stop camera
            ImGui::BeginDisabled(flir_camera_->IsStarted());
            if(ImGui::Button(button_start_label_.c_str())) {
                flir_camera_->Start();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!flir_camera_->IsStarted() || flir_camera_->IsRecording());
            if(ImGui::Button(button_stop_label_.c_str())) {
                flir_camera_->Stop();
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Display general settings
            if(flir_camera_->IsStarted()) {
                if(ImGui::CollapsingHeader(collapsing_header_general_label_.c_str())) {
                    ImGui::Spacing();

                    // Visualization
                    ImGui::Text("visualize    ");
                    ImGui::SameLine();
                    ImGui::Checkbox(checkbox_viz_label_.c_str(), &show_visualization_);
                    ImGui::Spacing();

                    // Settings
                    ImGui::BeginDisabled(flir_camera_->IsRecording());
                    ImGui::Text("exposure_time");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_exposure_time_label_.c_str(), &exposure_time_,
                                        500, max_exposure_time_)) {
                        flir_camera_->SetExposureTime(exposure_time_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("trigger_mode ");
                    ImGui::SameLine();
                    if(ImGui::Checkbox(checkbox_trigger_mode_label_.c_str(), &trigger_mode_)) {
                        flir_camera_->SetTriggerMode(trigger_mode_);
                    }
                    ImGui::Spacing();

                    //ImGui::Text("reverse_x    ");
                    //ImGui::SameLine();
                    //if(ImGui::Checkbox(checkbox_reverse_x_label_.c_str(), &reverse_x_)) {
                    //    flir_camera_->SetReverseX(reverse_x_);
                    //}
                    ImGui::Spacing();
                    ImGui::EndDisabled();

                    // Recording
                    ImGui::BeginDisabled(flir_camera_->IsRecording());
                    ImGui::Text("output_dir   ");
                    ImGui::SameLine();
                    ImGui::InputText(input_text_output_dir_label_.c_str(), input_text_output_dir_, BUF_SIZE);
                    ImGui::EndDisabled();
                    ImGui::Spacing();

                    ImGui::BeginDisabled(flir_camera_->IsRecording());
                    if(ImGui::Button(button_start_recording_label_.c_str())) {
                        output_dir_ = std::string {input_text_output_dir_};
                        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

                        output_dir_ += "/" + identifier_;
                        boost::filesystem::create_directory(output_dir_);
                        flir_camera_->StartRecording(output_dir_.c_str());
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!flir_camera_->IsRecording());
                    if(ImGui::Button(button_stop_recording_label_.c_str())) {
                        flir_camera_->StopRecording();
                        flir_camera_->AnalyzeRecording(output_dir_.c_str());
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                }
            }

            ImGui::End();
        }
    }

} // rcg::gui::flir

#endif //ENABLE_SPINNAKER_SDK