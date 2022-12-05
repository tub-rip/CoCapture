#include "prophesee_cam.h"


namespace camera {

    void PropheseeCam::setup_camera() {
        cam_ = Metavision::Camera::from_first_available();
        cam_.erc_module().enable(false);
        std::cout << "Opening Prophesee Camera: " << cam_.get_camera_configuration().serial_number << std::endl;
        auto &geometry = cam_.geometry();
        width_ = geometry.width();
        height_ = geometry.height();

        cam_.add_runtime_error_callback([](const Metavision::CameraException &e) {
            MV_LOG_ERROR() << e.what();
            throw;
        });
        cd_frame_generator = std::make_unique<Metavision::CDFrameGenerator>(width_, height_);
        cd_frame_generator->set_display_accumulation_time_us(params_.ev_acc_t_ms);
        cd_frame_generator->set_colors(color_bg_, color_on_, color_off_, true);
        cam_.cd().add_callback([this](const Metavision::EventCD *ev_begin,
                                      const Metavision::EventCD *ev_end) {
            cd_frame_generator->add_events(ev_begin, ev_end);
        });
        cd_frame_generator->start(params_.fps, [this](const Metavision::timestamp &ts, const cv::Mat &frame) {
            frame.copyTo(cd_frame_);
        });

        if (params_.mode == "master") {
            this->set_mode_master();
        } else if (params_.mode == "slave") {
            this->set_mode_slave();
        }

        if (params_.set_rois) {
            utils::set_roi(cam_);
        }

        if (params_.record_from_startup) {
            std::stringstream ss;
            ss << "./out_" << params_.id << ".raw";
            cam_.start_recording(ss.str());
        }
        cam_.start();
    }

    void PropheseeCam::get_display_frame(cv::Mat &display) {
        if (!cd_frame_.empty()) {
            cv::flip(cd_frame_, display, 0);
        }
    }

    void PropheseeCam::start_camera() { cam_.start(); }

    void PropheseeCam::stop_camera() { cam_.stop(); }

    void PropheseeCam::set_mode_master() {
        Metavision::Device &device = cam_.get_device();
        auto *i_device_control = device.get_facility<Metavision::I_DeviceControl>();
        if (i_device_control->set_mode_master()) {
            std::cout << "Set mode Master successful."
                         "Remember to start the slave first." << std::endl;
        } else {
            std::cerr << "Could not set Master mode. Master/slave might"
                         "not be supported by your camera" << std::endl;
        }
    }

    void PropheseeCam::set_mode_slave() {
        Metavision::Device &device = cam_.get_device();
        auto *i_device_control = device.get_facility<Metavision::I_DeviceControl>();
        if (i_device_control->set_mode_slave()) {
            std::cout << "Set mode Slave successful." << std::endl;
        } else {
            std::cerr << "Could not set Slave mode. Master/slave might"
                         "not be supported by your camera" << std::endl;
        }
    }

} // camera
