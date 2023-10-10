#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_GUI_H
#define RIP_COCAPTURE_GUI_FLIR_GUI_H

#include "camera_gui.h"
#include "flir_camera.h"

namespace rcg::gui::flir {

    class FlirGui : public ICameraGui {
    public:
        FlirGui(cams::flir::FlirCamera* flir_camera, int id);
        ~FlirGui();

    public:
        void Show(bool* show = nullptr) override;

    private:
        // Camera
        cams::flir::FlirCamera* flir_camera_;

        // Information
        std::string identifier_;
        std::string serial_number_;
        std::string model_name_;
        std::string vendor_name_;
        int max_exposure_time_;

        // Image Frame
        cv::Mat image_frame_;
        int image_frame_width_;
        int image_frame_height_;

        // Settings
        int exposure_time_;
        bool trigger_mode_;
        bool reverse_x_;

        // GUI
        GLuint image_frame_texture_id_;
        bool show_visualization_;

        std::string window_main_label_;
        std::string window_viz_label_;
        std::string checkbox_viz_label_;
        std::string button_start_label_;
        std::string button_stop_label_;
        std::string button_start_recording_label_;
        std::string button_stop_recording_label_;
        std::string collapsing_header_general_label_;
        std::string slider_exposure_time_label_;
        std::string checkbox_trigger_mode_label_;
        std::string checkbox_reverse_x_label_;
        std::string input_text_output_dir_label_;

        char input_text_output_dir_[BUF_SIZE];
        std::string output_dir_;
    };

} // rcg::gui::flir

#endif //RIP_COCAPTURE_GUI_FLIR_GUI_H
#endif //ENABLE_SPINNAKER_SDK