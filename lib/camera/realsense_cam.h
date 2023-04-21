#pragma once

#include "base.h"
#include <librealsense2/rs.hpp>
#include <fstream>

namespace camera {

    class RealSenseCam : public Base {
    public:
        RealSenseCam(int idx, rs2::context& ctx):
                record_(false),
                seq_(0),
                path_(""),
                save_ir_(true),
                save_color_(true){
            dvc_ = ctx.query_devices()[idx];

            rs2::config cfg;

            cfg.enable_device(dvc_.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
//            cfg.enable_all_streams();
            cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
            cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_ANY, 30);
            cfg.enable_stream(RS2_STREAM_INFRARED, 640, 480, RS2_FORMAT_ANY, 30);

            pipe_.start(cfg);
            //TODO: pass Realsense config through user input (via GUI) or config file
        }

        ~RealSenseCam() {}

    public:
        virtual void setup_camera();
        virtual void get_display_frame(cv::Mat &display);
        virtual void start_recording_to_path(std::string path);
        virtual void stop_recording();

    public:
        void metadata_to_csv(const rs2::frame& frm, const std::string& filename);

    private:
        rs2::device dvc_;
        rs2::pipeline pipe_;
        rs2::colorizer color_map_;

        bool record_;
        int seq_;
        std::string path_;
        bool save_ir_;
        bool save_color_;
    };

}
