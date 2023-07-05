#ifdef ENABLE_METAVISION_SDK

#include <boost/filesystem.hpp>

#include "prophesee_gui.h"
#include "imgui_helper.h"

namespace rcg::gui::prophesee {

    PropheseeGui::PropheseeGui(cams::prophesee::PropheseeCamera* prophesee_camera, int id) {
        // Camera
        prophesee_camera_ = prophesee_camera;

        // Information
        identifier_ = "prophesee" + std::to_string(id);
        serial_number_ = prophesee_camera_->GetSerialNumber();
        generation_ = prophesee_camera_->GetGeneration();
        plugin_name_ = prophesee_camera_->GetPluginName();

        // CD Frame
        cd_frame_width_ = prophesee_camera_->GetCDFrameWidth();
        cd_frame_height_ = prophesee_camera_->GetCDFrameHeight();

        // Biases
        std::map<std::string, std::pair<int, int>> bias_ranges = prophesee_camera_->GetBiasRanges();

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

        window_main_label_ = identifier_ + ("###" + identifier_ + "window_main");
        window_viz_label_ = identifier_ + ("###" + identifier_ + "window_viz");
        checkbox_viz_label_ = "###" + identifier_ + "checkbox_viz";
        combo_modes_label_ = "###" + identifier_ + "combo_modes";
        button_start_label_ = "Start" + ("###" + identifier_ + "button_start");
        button_stop_label_ = "Stop" + ("###" + identifier_ + "button_stop");
        button_start_recording_label_ = "Record" + ("###" + identifier_ + "button_start_recording");
        button_stop_recording_label_ = "End" + ("###" + identifier_ + "button_stop_recording");
        collapsing_header_general_label_ = "General" + ("###" + identifier_ + "collapsing_header_general");
        slider_bias_diff_off_label_ = "###" + identifier_ + "slider_bias_diff_off";
        slider_bias_diff_on_label_ = "###" + identifier_ + "slider_bias_diff_on";
        slider_bias_fo_label_ = "###" + identifier_ + "slider_bias_fo";
        slider_bias_hpf_label_ = "###" + identifier_ + "slider_bias_hpf";
        slider_bias_refr_label_ = "###" + identifier_ + "slider_bias_refr";
        input_text_output_dir_label_ = "###" + identifier_ + "input_text_output_dir";

        memset(input_text_output_dir_, 0, BUF_SIZE);

        // Event Signal Processor
        if(cams::prophesee::EventSignalProcessor* esp = prophesee_camera_->GetEventSignalProcessor(); esp) {
            collapsing_header_esp_label_ = "Event Signal Processing" + ("###" + identifier_ + "collapsing_header_esp");

            // Anti-Flicker (AFK)
            ESP_AFK_is_enabled_ = esp->AFK_IsEnabled();
            ESP_AFK_frequency_band_ = esp->AFK_GetFrequencyBand();
            ESP_AFK_max_frequency_band_ = esp->AFK_GetMaxFrequencyBand();
            ESP_AFK_duty_cycle_ = esp->AFK_GetDutyCycle();
            ESP_AFK_duty_cycle_range_ = esp->AFK_GetDutyCycleRange();
            ESP_AFK_start_threshold_ = esp->AFK_GetStartThreshold();
            ESP_AFK_start_threshold_range_ = esp->AFK_GetStartThresholdRange();
            ESP_AFK_stop_threshold_ = esp->AFK_GetStopThreshold();
            ESP_AFK_stop_threshold_range_ = esp->AFK_GetStopThresholdRange();

            for(int i = 0; i < IM_ARRAYSIZE(ESP_AFK_modes_); ++i) {
                if(esp->AFK_GetFilteringMode() == ESP_AFK_modes_[i]) {
                    esp_afk_mode_selected_ = i;
                    break;
                }
            }

            separator_text_esp_afk_label_ = "Anti-Flicker (AFK)" + ("###" + identifier_ + "separator_text_esp_afk");
            combo_esp_afk_modes_label_ = "###" + identifier_ + "combo_esp_afk_modes";
            checkbox_esp_afk_is_enabled_label_ = "###" + identifier_ + "checkbox_esp_afk_is_enabled";
            slider_esp_afk_freq_band_lower_bound_label_ = "###" + identifier_ + "slider_esp_afk_freq_band_lower_bound";
            slider_esp_afk_freq_band_upper_bound_label_ = "###" + identifier_ + "slider_esp_afk_freq_band_upper_bound";
            slider_esp_afk_duty_cycle_label_ = "###" + identifier_ + "slider_esp_afk_duty_cycle";
            slider_esp_afk_start_threshold_label_ = "###" + identifier_ + "slider_esp_afk_start_threshold";
            slider_esp_afk_stop_threshold_label_ = "###" + identifier_ + "slider_esp_afk_stop_threshold";

            // Event Trail Filter (STC/Trail)
            ESP_ETF_is_enabled_ = esp->ETF_IsEnabled();
            ESP_ETF_filter_types_ = esp->ETF_GetAvailableFilterTypes();
            ESP_ETF_filter_threshold_delay_ = esp->ETF_GetFilterThresholdDelay();
            ESP_ETF_filter_threshold_delay_range_ = esp->ETF_GetFilterThresholdDelayRange();

            for(int i = 0; i < ESP_ETF_filter_types_.size(); ++i) {
                if(esp->ETF_GetFilterType() == ESP_ETF_filter_types_[i]) {
                    esp_etf_filter_type_selected_ = i;
                    break;
                }
            }

            separator_text_esp_etf_label_ = "Event Trail Filter (STC/Trail)" + ("###" + identifier_ + "separator_text_esp_etf");
            checkbox_esp_etf_is_enabled_label_ = "###" + identifier_ + "checkbox_esp_etf_is_enabled";
            combo_esp_etf_filter_types_label_ = "###" + identifier_ + "combo_esp_etf_filter_types";
            slider_esp_etf_filter_threshold_delay_label_ = "###" + identifier_ + "slider_esp_etf_filter_threshold_delay";

            // Event Rate Controller (ERC)
            ESP_ERC_is_enabled_ = esp->ERC_IsEnabled();
            ESP_ERC_cd_event_rate_ = esp->ERC_GetCDEventRate();
            ESP_ERC_cd_event_rate_range_ = esp->ERC_GetCDEventRateRange();

            separator_text_esp_erc_label_ = "Event Rate Controller (ERC)" + ("###" + identifier_ + "separator_text_esp_erc");
            checkbox_esp_erc_is_enabled_label_ = "###" + identifier_ + "checkbox_esp_erc_is_enabled";
            slider_esp_erc_cd_event_rate_label_ = "###" + identifier_ + "slider_esp_erc_cd_event_rate";
        }

        // Debug
        collapsing_header_debug_label_ = "Debug" + ("###" + identifier_ + "collapsing_header_debug");
        table_debug_label_ = "###" + (identifier_ + "table_debug");
    }

    PropheseeGui::~PropheseeGui() {
        DeleteTexture(&cd_frame_texture_id_);
    }

    void PropheseeGui::Show(bool* show) {
        // Display CD frame
        if(show_visualization_) {
            prophesee_camera_->OutputCDFrame(cd_frame_);
            UpdateTexture(&cd_frame_texture_id_, cd_frame_.data, cd_frame_width_, cd_frame_height_);
            DisplayImage(&cd_frame_texture_id_, window_viz_label_.c_str(), cd_frame_width_, cd_frame_height_, &show_visualization_);
        }

        if(show == nullptr || *(show)) {
            // Display camera information
            ImGui::Begin(identifier_.c_str(), show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Spacing();

            ImGui::Text("Identifier           : %s", identifier_.c_str());
            ImGui::Spacing();
            ImGui::Text("Serial number        : %s", serial_number_.c_str());
            ImGui::Spacing();
            ImGui::Text("Generation           : %s", generation_.c_str());
            ImGui::Spacing();
            ImGui::Text("Plugin name          : %s", plugin_name_.c_str());
            ImGui::Spacing();
            ImGui::Text("Resolution           : %d * %d", cd_frame_width_, cd_frame_height_);
            ImGui::Spacing();
            ImGui::BeginDisabled(prophesee_camera_->IsStarted());
            ImGui::Text("Synchronization mode");
            ImGui::SameLine();
            if(ImGui::Combo(combo_modes_label_.c_str(), &mode_selected_, modes_, IM_ARRAYSIZE(modes_))) {
                prophesee_camera_->SetMode(modes_[mode_selected_]);
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Start or stop camera
            ImGui::BeginDisabled(prophesee_camera_->IsStarted() || !mode_selected_);
            if(ImGui::Button(button_start_label_.c_str())) {
                prophesee_camera_->Start();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(!prophesee_camera_->IsStarted() || prophesee_camera_->IsRecording());
            if(ImGui::Button(button_stop_label_.c_str())) {
                prophesee_camera_->Stop();
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            // Display general settings
            if(prophesee_camera_->IsStarted()) {
                if(ImGui::CollapsingHeader(collapsing_header_general_label_.c_str())) {
                    ImGui::Spacing();

                    // Visualization
                    ImGui::Text("visualize    ");
                    ImGui::SameLine();
                    ImGui::Checkbox(checkbox_viz_label_.c_str(), &show_visualization_);
                    ImGui::Spacing();

                    // Bias settings
                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    ImGui::Text("bias_diff_off");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_diff_off_label_.c_str(), &bias_diff_off_, bias_diff_off_range_.first, bias_diff_off_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_diff_off", bias_diff_off_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_diff_on ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_diff_on_label_.c_str(), &bias_diff_on_, bias_diff_on_range_.first, bias_diff_on_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_diff_on", bias_diff_on_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_fo      ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_fo_label_.c_str(), &bias_fo_, bias_fo_range_.first, bias_fo_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_fo", bias_fo_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_hpf     ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_hpf_label_.c_str(), &bias_hpf_, bias_hpf_range_.first, bias_hpf_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_hpf", bias_hpf_);
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_refr    ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt(slider_bias_refr_label_.c_str(), &bias_refr_, bias_refr_range_.first, bias_refr_range_.second)) {
                        prophesee_camera_->SetBiasValue("bias_refr", bias_refr_);
                    }
                    ImGui::Spacing();
                    ImGui::EndDisabled();

                    // Recording
                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    ImGui::Text("output_dir   ");
                    ImGui::SameLine();
                    ImGui::InputText(input_text_output_dir_label_.c_str(), input_text_output_dir_, BUF_SIZE);
                    ImGui::EndDisabled();
                    ImGui::Spacing();

                    ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                    if(ImGui::Button(button_start_recording_label_.c_str())) {
                        output_dir_ = std::string {input_text_output_dir_};
                        if(output_dir_.back() == '/') { output_dir_.pop_back(); }

                        output_dir_ += "/" + identifier_;
                        boost::filesystem::create_directory(output_dir_);
                        prophesee_camera_->StartRecording(output_dir_.c_str());
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!prophesee_camera_->IsRecording());
                    if(ImGui::Button(button_stop_recording_label_.c_str())) {
                        prophesee_camera_->StopRecording();
                        prophesee_camera_->AnalyzeRecording(output_dir_.c_str());
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                }

                // Event Signal Processing
                if(cams::prophesee::EventSignalProcessor* esp = prophesee_camera_->GetEventSignalProcessor(); esp) {
                    if(ImGui::CollapsingHeader(collapsing_header_esp_label_.c_str())) {
                        ImGui::BeginDisabled(prophesee_camera_->IsRecording());
                        ImGui::Spacing();

                        // Anti-Flicker (AFK)
                        ImGui::SeparatorText(separator_text_esp_afk_label_.c_str());
                        ImGui::Spacing();

                        ImGui::Text("enable_afk     ");
                        ImGui::SameLine();
                        if(ImGui::Checkbox(checkbox_esp_afk_is_enabled_label_.c_str(), &ESP_AFK_is_enabled_)) {
                            if(ESP_AFK_is_enabled_) {
                                esp->AFK_Enable();
                            } else {
                                esp->AFK_Disable();
                            }
                        }
                        ImGui::Spacing();

                        ImGui::BeginDisabled(ESP_AFK_is_enabled_);
                        ImGui::Text("filtering_mode ");
                        ImGui::SameLine();
                        if(ImGui::Combo(combo_esp_afk_modes_label_.c_str(), &esp_afk_mode_selected_, ESP_AFK_modes_, IM_ARRAYSIZE(ESP_AFK_modes_))) {
                            esp->AFK_SetFilteringMode(ESP_AFK_modes_[esp_afk_mode_selected_]);
                        }
                        ImGui::Spacing();

                        ImGui::Text("freq_band_low  ");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_afk_freq_band_lower_bound_label_.c_str(), &ESP_AFK_frequency_band_.first, ESP_AFK_max_frequency_band_.first, ESP_AFK_frequency_band_.second)) {
                            esp->AFK_SetFrequencyBand(ESP_AFK_frequency_band_);
                        }
                        ImGui::Spacing();

                        ImGui::Text("freq_band_upp  ");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_afk_freq_band_upper_bound_label_.c_str(), &ESP_AFK_frequency_band_.second, ESP_AFK_frequency_band_.first, ESP_AFK_max_frequency_band_.second)) {
                            esp->AFK_SetFrequencyBand(ESP_AFK_frequency_band_);
                        }
                        ImGui::Spacing();

                        ImGui::Text("start_threshold");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_afk_start_threshold_label_.c_str(), &ESP_AFK_start_threshold_, ESP_AFK_start_threshold_range_.first, ESP_AFK_start_threshold_range_.second)) {
                            esp->AFK_SetStartThreshold(ESP_AFK_start_threshold_);
                        }
                        ImGui::Spacing();

                        ImGui::Text("stop_threshold ");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_afk_stop_threshold_label_.c_str(), &ESP_AFK_stop_threshold_, ESP_AFK_stop_threshold_range_.first, ESP_AFK_stop_threshold_range_.second)) {
                            esp->AFK_SetStopThreshold(ESP_AFK_stop_threshold_);
                        }
                        ImGui::Spacing();

                        ImGui::Text("duty_cycle     ");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_afk_duty_cycle_label_.c_str(), &ESP_AFK_duty_cycle_, ESP_AFK_duty_cycle_range_.first, ESP_AFK_duty_cycle_range_.second)) {
                            esp->AFK_SetDutyCycle(ESP_AFK_duty_cycle_);
                        }
                        ImGui::Spacing();
                        ImGui::EndDisabled();

                        // Event Trail Filter (STC/Trail)
                        ImGui::SeparatorText(separator_text_esp_etf_label_.c_str());
                        ImGui::Spacing();

                        ImGui::Text("enable_etf    ");
                        ImGui::SameLine();
                        if(ImGui::Checkbox(checkbox_esp_etf_is_enabled_label_.c_str(), &ESP_ETF_is_enabled_)) {
                            if(ESP_ETF_is_enabled_) {
                                esp->ETF_Enable();
                            } else {
                                esp->ETF_Disable();
                            }
                        }
                        ImGui::Spacing();

                        ImGui::BeginDisabled(ESP_ETF_is_enabled_);
                        ImGui::Text("filter_type   ");
                        ImGui::SameLine();
                        if(ImGui::Combo(combo_esp_etf_filter_types_label_.c_str(), &esp_etf_filter_type_selected_,
                                        VectorOfStringsItemGetter, &ESP_ETF_filter_types_, ESP_ETF_filter_types_.size())) {
                            esp->ETF_SetFilterType(ESP_ETF_filter_types_[esp_etf_filter_type_selected_]);
                        }
                        ImGui::Spacing();

                        ImGui::Text("filter_thr_del");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_etf_filter_threshold_delay_label_.c_str(), &ESP_ETF_filter_threshold_delay_,
                                         ESP_ETF_filter_threshold_delay_range_.first, ESP_ETF_filter_threshold_delay_range_.second)) {
                            esp->ETF_SetFilterThresholdDelay(ESP_ETF_filter_threshold_delay_);
                        }
                        ImGui::Spacing();
                        ImGui::EndDisabled();

                        // Event Rate Controller (ERC)
                        ImGui::SeparatorText(separator_text_esp_erc_label_.c_str());
                        ImGui::Spacing();

                        ImGui::Text("enable_erc   ");
                        ImGui::SameLine();
                        if(ImGui::Checkbox(checkbox_esp_erc_is_enabled_label_.c_str(), &ESP_ERC_is_enabled_)) {
                            if(ESP_ERC_is_enabled_) {
                                esp->ERC_Enable();
                            } else {
                                esp->ERC_Disable();
                            }
                        }
                        ImGui::Spacing();

                        ImGui::Text("cd_event_rate");
                        ImGui::SameLine();
                        if(ImGui::SliderInt(slider_esp_erc_cd_event_rate_label_.c_str(), &ESP_ERC_cd_event_rate_, ESP_ERC_cd_event_rate_range_.first, ESP_ERC_cd_event_rate_range_.second)) {
                            esp->ERC_SetCDEventRate(ESP_ERC_cd_event_rate_);
                        }
                        ImGui::Spacing();

                        ImGui::EndDisabled();
                    }
                }

                // Debug
                if(ImGui::CollapsingHeader(collapsing_header_debug_label_.c_str())) {
                    ImGui::Spacing();

                    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
                    if(ImGui::BeginTable(table_debug_label_.c_str(), 3, table_flags)) {
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableSetupColumn("GUI");
                        ImGui::TableSetupColumn("Sensor");
                        ImGui::TableHeadersRow();

                        // Synchronization mode
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Synchronization mode");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", modes_[mode_selected_]);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", prophesee_camera_->GetMode().c_str());

                        // bias_diff_off
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("bias_diff_off");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", bias_diff_off_);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", prophesee_camera_->GetBiasValue("bias_diff_off"));

                        // bias_diff_on
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("bias_diff_on");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", bias_diff_on_);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", prophesee_camera_->GetBiasValue("bias_diff_on"));

                        // bias_fo
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("bias_fo");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", bias_fo_);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", prophesee_camera_->GetBiasValue("bias_fo"));

                        // bias_hpf
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("bias_hpf");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", bias_hpf_);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", prophesee_camera_->GetBiasValue("bias_hpf"));

                        // bias_refr
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("bias_refr");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", bias_refr_);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", prophesee_camera_->GetBiasValue("bias_refr"));

                        if(cams::prophesee::EventSignalProcessor* esp = prophesee_camera_->GetEventSignalProcessor(); esp) {
                            // enable_afk
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("enable_afk");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", ESP_AFK_is_enabled_ ? "true" : "false");
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", esp->AFK_IsEnabled() ? "true" : "false");

                            // filtering_mode
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("filtering_mode");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", ESP_AFK_modes_[esp_afk_mode_selected_]);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", esp->AFK_GetFilteringMode().c_str());

                            // freq_band_low
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("freq_band_low");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_AFK_frequency_band_.first);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->AFK_GetFrequencyBand().first);

                            // freq_band_upp
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("freq_band_upp");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_AFK_frequency_band_.second);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->AFK_GetFrequencyBand().second);

                            // start_threshold
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("start_threshold");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_AFK_start_threshold_);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->AFK_GetStartThreshold());

                            // stop_threshold
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("stop_threshold");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_AFK_stop_threshold_);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->AFK_GetStopThreshold());

                            // duty_cycle
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("duty_cycle");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_AFK_duty_cycle_);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->AFK_GetDutyCycle());

                            // enable_etf
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("enable_etf");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", ESP_ETF_is_enabled_ ? "true" : "false");
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", esp->ETF_IsEnabled() ? "true" : "false");

                            // filter_type
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("filter_type");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", ESP_ETF_filter_types_[esp_etf_filter_type_selected_].c_str());
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", esp->ETF_GetFilterType().c_str());

                            // filter_thr_del
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("filter_thr_del");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_ETF_filter_threshold_delay_);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->ETF_GetFilterThresholdDelay());

                            // enable_erc
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("enable_erc");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", ESP_ERC_is_enabled_ ? "true" : "false");
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", esp->ERC_IsEnabled() ? "true" : "false");

                            // cd_event_rate
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("cd_event_rate");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", ESP_ERC_cd_event_rate_);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%d", esp->ERC_GetCDEventRate());
                        }

                        ImGui::EndTable();
                    }

                    ImGui::Spacing();
                }
            }

            ImGui::End();
        }
    }

} // rcg::gui::prophesee

#endif //ENABLE_METAVISION_SDK