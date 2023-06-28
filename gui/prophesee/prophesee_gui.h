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
        void Show(bool* show) override;

    private:
        // Camera
        cams::prophesee::PropheseeCamera* prophesee_camera_;

        // Information
        char identifier_[BUF_SIZE];
        char serial_number_[BUF_SIZE];
        char generation_[BUF_SIZE];
        char plugin_name_[BUF_SIZE];
        char mode_[BUF_SIZE];

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
        const char* modes_[4] {"", "Standalone", "Master", "Slave"};
        GLuint cd_frame_texture_id_;
        bool show_visualization_;
        int mode_selected_;

        char window_main_label_[BUF_SIZE];
        char window_viz_label_[BUF_SIZE];
        char checkbox_viz_label_[BUF_SIZE];
        char combo_modes_label_[BUF_SIZE];
        char button_start_label_[BUF_SIZE];
        char button_stop_label_[BUF_SIZE];
        char button_start_recording_label_[BUF_SIZE];
        char button_stop_recording_label_[BUF_SIZE];
        char collapsing_header_general_label_[BUF_SIZE];
        char slider_bias_diff_off_label_[BUF_SIZE];
        char slider_bias_diff_on_label_[BUF_SIZE];
        char slider_bias_fo_label_[BUF_SIZE];
        char slider_bias_hpf_label_[BUF_SIZE];
        char slider_bias_refr_label_[BUF_SIZE];

        char input_text_output_dir_[BUF_SIZE];
        char input_text_output_dir_label_[BUF_SIZE];
    };

} // rcg::gui::prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_GUI_H
#endif //ENABLE_METAVISION_SDK