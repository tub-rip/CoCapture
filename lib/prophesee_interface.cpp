//
// Created by friedhelm on 28.04.22.
//

#include "prophesee_interface.h"

namespace prophesee {
    std::shared_ptr<Metavision::Device> make_device(const std::string serial) {
        std::cout << "Opening Prophesee Camera" << serial << std::endl;
        std::shared_ptr<Metavision::Device> device;
        try {
            device = Metavision::DeviceDiscovery::open(serial);
        } catch (Metavision::HalException &e) { std::cout << "Error exception: " << e.what() << std::endl; }

        if (!device) {
            std::cerr << "Camera opening failed." << std::endl;
        }
        std::cout << "Camera open." << std::endl;

        device->get_facility<Metavision::I_DeviceControl>()->start();
        device->get_facility<Metavision::I_EventsStream>()->start();
        return device;
    }

    void decoding_loop(std::shared_ptr<Metavision::Device> device) {
        auto i_device_control = device->get_facility<Metavision::I_DeviceControl>();
        auto i_events_stream = device->get_facility<Metavision::I_EventsStream>();
        auto i_decoder = device->get_facility<Metavision::I_Decoder>();

        using namespace std::chrono;
        milliseconds last_update_monitoring = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        while (true) {
            short ret = i_events_stream->poll_buffer();
            if (ret < 0) {
                std::cout << "End of file" << std::endl;
                i_events_stream->stop();
                i_events_stream->stop_log_raw_data();
                i_device_control->stop();
                std::cout << "Camera stopped." << std::endl;
            }

            /// [buffer]
            // Here we polled data, so we can launch decoding
            long n_bytes;
            uint8_t *raw_data = i_events_stream->get_latest_raw_data(n_bytes);

            // This will trigger callbacks set on decoders: in our case EventAnalyzer.process_events
            i_decoder->decode(raw_data, raw_data + n_bytes);
            /// [buffer]
        }
    }
} // prophesee