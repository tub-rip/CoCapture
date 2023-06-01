#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_GUI_H
#define RIP_COCAPTURE_GUI_REALSENSE_GUI_H

#include "camera_gui.h"
#include "realsense_camera.h"

namespace rcg::gui::realsense {

    class RealSenseGui : ICameraGui {
    public:
        RealSenseGui(const char* serial_number, int id);
        ~RealSenseGui();

    public:
        void Show(bool* show) override;
        void StartRecording(const char* output_dir) override;
        void StopRecording() override;

    private:
        // Camera
        std::unique_ptr<cams::realsense::RealSenseCamera> realsense_camera_;

        // Information
        char identifier_[BUF_SIZE];
        char serial_number_[BUF_SIZE];

        // Frame
        cv::Mat color_frame_;
        cv::Mat depth_frame_;
        cv::Mat infrared_frame_;

        std::pair<int, int> color_frame_dimensions_;
        std::pair<int, int> depth_frame_dimensions_;
        std::pair<int, int> infrared_frame_dimensions_;

        // GUI
        GLuint color_frame_tid_;
        GLuint depth_frame_tid_;
        GLuint infrared_frame_tid_;

        bool show_viz_color_;
        bool show_viz_depth_;
        bool show_viz_infrared_;

        char window_main_label_[BUF_SIZE];
        char collapsing_header_general_label_[BUF_SIZE];

        char window_viz_color_label_[BUF_SIZE];
        char window_viz_depth_label_[BUF_SIZE];
        char window_viz_infrared_label_[BUF_SIZE];

        char checkbox_viz_color_label_[BUF_SIZE];
        char checkbox_viz_depth_label_[BUF_SIZE];
        char checkbox_viz_infrared_label_[BUF_SIZE];
    };

} // rcg::gui::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_GUI_H
#endif //ENABLE_REALSENSE_SDK
