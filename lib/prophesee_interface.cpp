#include "prophesee_interface.h"


PropheseeInterface::PropheseeInterface()
{
    open_camera_();

    auto *i_geometry = device_->get_facility<Metavision::I_Geometry>();
    if (!i_geometry) {
        std::cerr << "Could not retrieve geometry." << std::endl;
    }

    event_visualizer_.setup_display(i_geometry->get_width(), i_geometry->get_height());

    sign_up_cd_callback_();

    i_device_control_ = device_->get_facility<Metavision::I_DeviceControl>();
    if (!i_device_control_) {
        std::cerr << "Could not get Device Control facility." << std::endl;
    }

    i_events_stream_ = device_->get_facility<Metavision::I_EventsStream>();
    if (!i_events_stream_) {
        std::cerr << "Could not initialize events stream." << std::endl;
    }

    auto *i_decoder = device_->get_facility<Metavision::I_Decoder>();

    i_device_control_->set_mode_master();
    i_device_control_->start();
    i_events_stream_->start();

}


bool PropheseeInterface::open_camera_() {
    /// Retrieve serial numbers
    auto v = Metavision::DeviceDiscovery::list();

    if (v.empty()) {
        std::cout << "No Prophesee Camera detected" << std::endl;
        return false;
    }

    std::cout << "Opening Prophesee Camera.." << std::endl;
    try {
        device_ = Metavision::DeviceDiscovery::open(v.front());
    } catch (Metavision::HalException &e) { std::cout << "Error exception: " << e.what() << std::endl; }

    if (!device_) {
        std::cerr << "Camera opening failed." << std::endl;
        return false;
    }
    std::cout << "Camera open." << std::endl;
    return true;
}


bool PropheseeInterface::sign_up_cd_callback_() {
    auto *i_cddecoder =
            device_->get_facility<Metavision::I_EventDecoder<Metavision::EventCD>>();

    if (i_cddecoder) {
        // Register a lambda function to be called on every CD events
        i_cddecoder->add_event_buffer_callback(
                [&](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                    this->event_visualizer_.process_events(begin, end);
                });
    }

    return true;
}


void PropheseeInterface::decoding_loop() {
    while (true) {
        short ret = i_events_stream_->poll_buffer();
        if (ret < 0) {
            std::cout << "End of file" << std::endl;
            i_events_stream_->stop();
            i_events_stream_->stop_log_raw_data();
            i_device_control_->stop();
            std::cout << "Camera stopped." << std::endl;
        }
        /// [buffer]
        // Here we polled data, so we can launch decoding
        long n_bytes;
        uint8_t *raw_data = i_events_stream_->get_latest_raw_data(n_bytes);

        // This will trigger callbacks set on decoders: in our case EventAnalyzer.process_events
        i_decoder_->decode(raw_data, raw_data + n_bytes);
        /// [buffer]
    }
}