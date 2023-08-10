#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_GUI_H
#define RIP_COCAPTURE_GUI_REALSENSE_GUI_H

#include "camera_gui.h"
#include "realsense_camera.h"
#include "l5.h"

namespace rcg::gui::realsense {

    class RealSenseGui : public ICameraGui {
    public:
        RealSenseGui(cams::realsense::RealSenseCamera* realsense_camera, int id);
        ~RealSenseGui();

    public:
        void Show(bool* show = nullptr) override;

    private:
        // Camera
        cams::realsense::RealSenseCamera* realsense_camera_;

        // Information
        std::string identifier_;
        std::string name_;
        std::string serial_number_;
        std::string product_line_;

        // Display Frame
        cv::Mat display_frame_;
        int display_frame_width_;
        int display_frame_height_;

        // GUI
        GLuint display_frame_texture_id_;
        bool show_visualization_;

        std::string window_main_label_;
        std::string window_viz_label_;
        std::string checkbox_viz_label_;
        std::string button_start_label_;
        std::string button_stop_label_;
        std::string button_start_recording_label_;
        std::string button_stop_recording_label_;
        std::string collapsing_header_general_label_;
        std::string input_text_output_dir_label_;

        char input_text_output_dir_[BUF_SIZE];
        std::string output_dir_;
    };

} // rcg::gui::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_GUI_H
#endif //ENABLE_REALSENSE_SDK