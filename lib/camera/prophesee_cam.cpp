#include "prophesee_cam.h"


namespace camera {
    void PropheseeCam::setup_camera() {
        cam_ = Metavision::Camera::from_first_available();
        std::cout << "Opening Prophesee Camera: " << cam_.get_camera_configuration().serial_number << std::endl;
        auto &geometry = cam_.geometry();
        width_ = geometry.width();
        height_ = geometry.height();
        visualizer_.setup_display(width_, height_, false);

        cam_.add_runtime_error_callback([](const Metavision::CameraException &e) {
            MV_LOG_ERROR() << e.what();
            throw;
        });

        cam_.cd().add_callback([this](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
            visualizer_.process_events(begin, end);
        });

        cam_.cd().add_callback([this](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
            visualizer_.estimate_snr(begin, end);
        });

        if (params_.set_rois) {
            utils::set_roi(cam_);
        }

        cam_.start();
    }

    void PropheseeCam::get_display_frame(cv::Mat &display) {
        visualizer_.get_display_frame(display);
    }

    void PropheseeCam::get_display_frame(cv::Mat &display, const cv::Mat &canvas) {
        visualizer_.get_display_frame(display, canvas);
    }

    void PropheseeCam::start_camera() { cam_.start(); }

    void PropheseeCam::stop_camera() { cam_.stop(); }

    void PropheseeCam::set_mode_master() {
        Metavision::Device &device = cam_.get_device();
        auto *i_device_control = device.get_facility<Metavision::I_DeviceControl>();
        if (i_device_control->set_mode_master()) {
            std::cout << "Set mode Master successful."
                         "Remember to start the slave first." << std::endl;
        }
        else {
            std::cerr << "Could not set Master mode. Master/slave might"
                         "not be supported by your camera" << std::endl;
        }
    }

    void PropheseeCam::set_mode_slave() {
        Metavision::Device &device = cam_.get_device();
        auto *i_device_control = device.get_facility<Metavision::I_DeviceControl>();
        if (i_device_control->set_mode_slave()) {
            std::cout << "Set mode Slave successful."
                         "Remember to start the slave first." << std::endl;
        }
        else {
            std::cerr << "Could not set Slave mode. Master/slave might"
                         "not be supported by your camera" << std::endl;
        }
    }


} // camera