#ifdef ENABLE_METAVISION_SDK
#ifndef RIP_COCAPTURE_GUI_PROPHESEE_GUI_H
#define RIP_COCAPTURE_GUI_PROPHESEE_GUI_H

#include "camera_gui.h"
#include "prophesee_camera.h"

namespace rcg::gui::prophesee {

    class PropheseeGui : public ICameraGui {
    public:
        PropheseeGui(cams::prophesee::PropheseeCamera* prophesee_camera, int id);
        ~PropheseeGui();

    public:
        void Show(bool* show = nullptr) override;

    private:
        // Camera
        cams::prophesee::PropheseeCamera* prophesee_camera_;

        // Information
        std::string identifier_;
        std::string serial_number_;
        std::string generation_;
        std::string plugin_name_;

        // CD Frame
        cv::Mat cd_frame_;
        int cd_frame_width_;
        int cd_frame_height_;

        // Biases
        int bias_diff_off_;
        int bias_diff_on_;
        int bias_fo_;
        int bias_hpf_;
        int bias_refr_;

        std::pair<int, int> bias_diff_off_range_;
        std::pair<int, int> bias_diff_on_range_;
        std::pair<int, int> bias_fo_range_;
        std::pair<int, int> bias_hpf_range_;
        std::pair<int, int> bias_refr_range_;

        // GUI
        const char* modes_[4] {"Select a mode", "Standalone", "Master", "Slave"};
        GLuint cd_frame_texture_id_;
        bool show_visualization_;
        int mode_selected_;

        std::string window_main_label_;
        std::string window_viz_label_;
        std::string checkbox_viz_label_;
        std::string combo_modes_label_;
        std::string button_start_label_;
        std::string button_stop_label_;
        std::string button_start_recording_label_;
        std::string button_stop_recording_label_;
        std::string collapsing_header_general_label_;
        std::string slider_bias_diff_off_label_;
        std::string slider_bias_diff_on_label_;
        std::string slider_bias_fo_label_;
        std::string slider_bias_hpf_label_;
        std::string slider_bias_refr_label_;
        std::string input_text_output_dir_label_;

        char input_text_output_dir_[BUF_SIZE];
        std::string output_dir_;

        // Event Signal Processor
        std::string collapsing_header_esp_label_;

        // Anti-Flicker (AFK)
        bool ESP_AFK_is_enabled_;
        std::pair<int, int> ESP_AFK_frequency_band_;
        std::pair<int, int> ESP_AFK_max_frequency_band_;
        int ESP_AFK_duty_cycle_;
        std::pair<int, int> ESP_AFK_duty_cycle_range_;
        int ESP_AFK_start_threshold_;
        std::pair<int, int> ESP_AFK_start_threshold_range_;
        int ESP_AFK_stop_threshold_;
        std::pair<int, int> ESP_AFK_stop_threshold_range_;

        const char* ESP_AFK_modes_[2] = {"Band Pass", "Band Stop"};
        int esp_afk_mode_selected_;
        std::string separator_text_esp_afk_label_;
        std::string combo_esp_afk_modes_label_;
        std::string checkbox_esp_afk_is_enabled_label_;
        std::string slider_esp_afk_freq_band_lower_bound_label_;
        std::string slider_esp_afk_freq_band_upper_bound_label_;
        std::string slider_esp_afk_duty_cycle_label_;
        std::string slider_esp_afk_start_threshold_label_;
        std::string slider_esp_afk_stop_threshold_label_;

        // Event Trail Filter (STC/Trail)
        bool ESP_ETF_is_enabled_;
        std::vector<std::string> ESP_ETF_filter_types_;
        int ESP_ETF_filter_threshold_delay_;
        std::pair<int, int> ESP_ETF_filter_threshold_delay_range_;

        int esp_etf_filter_type_selected_;
        std::string separator_text_esp_etf_label_;
        std::string checkbox_esp_etf_is_enabled_label_;
        std::string combo_esp_etf_filter_types_label_;
        std::string slider_esp_etf_filter_threshold_delay_label_;

        // Event Rate Controller (ERC)
        bool ESP_ERC_is_enabled_;
        int ESP_ERC_cd_event_rate_;
        std::pair<int, int> ESP_ERC_cd_event_rate_range_;

        std::string separator_text_esp_erc_label_;
        std::string checkbox_esp_erc_is_enabled_label_;
        std::string slider_esp_erc_cd_event_rate_label_;

        // Debug
        std::string collapsing_header_debug_label_;
        std::string table_debug_label_;
    };

} // rcg::gui::prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_GUI_H
#endif //ENABLE_METAVISION_SDK