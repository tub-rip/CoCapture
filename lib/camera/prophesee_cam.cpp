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
        trigger_event_saver_ = std::make_unique<TriggerEventSaver>();
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

        int channel_id = 0; // for EVK4, EVK3
        cam_.get_device().get_facility<Metavision::I_TriggerIn>()->enable(channel_id);

        auto ext_trigger_evts_cb = std::bind(&PropheseeCam::increment_ext_trigger_evts, this);
        ext_trigger_evts_cb_id_ = cam_.ext_trigger().add_callback(ext_trigger_evts_cb);

        cam_.ext_trigger().add_callback(
                [this](const Metavision::EventExtTrigger *begin,
                                       const Metavision::EventExtTrigger *end) {
                    trigger_event_saver_->save_events(begin, end);
                });

        cam_.start();
    }

    void PropheseeCam::get_display_frame(cv::Mat &display) {
        if (!cd_frame_.empty()) {
            cd_frame_.copyTo(display);
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

    int PropheseeCam::get_bias_value(std::string bias_name) { return cam_.biases().get_facility()->get(bias_name); }

    void PropheseeCam::set_bias_value(std::string bias_name, int bias_value) {
        cam_.biases().get_facility()->set(bias_name, bias_value);
    }

    void PropheseeCam::start_recording_to_path(std::string path) {
        is_recording_ = true;
        trigger_event_saver_->set_path(path);
        cam_.start_recording(path);
    }

    void PropheseeCam::stop_recording() {
        is_recording_ = false;
        trigger_event_saver_->stop_writing();
        cam_.stop_recording();
    }

    void PropheseeCam::increment_ext_trigger_evts() { if (is_recording_) { ext_trigger_evts_++; }}

    void PropheseeCam::reset_ext_trigger_evts() { ext_trigger_evts_ = 0; }

    int PropheseeCam::get_ext_trigger_evts() { return ext_trigger_evts_; }

} // camera
