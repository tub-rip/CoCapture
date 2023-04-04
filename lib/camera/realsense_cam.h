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
                path_("") {
            dvc_ = ctx.query_devices()[idx];

            rs2::config cfg;
            cfg.enable_device(dvc_.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

            pipe_.start(cfg);
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
    };

}
