#include <iostream>

#include <pangolin/var/var.h>
#include <pangolin/gl/gl.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/display/widgets.h>
#include <pangolin/display/default_font.h>

#include <opencv2/core.hpp>

#include "../lib/prophesee_interface.h"


int main() {

    // #########################################################################
    // PROPHESEE
    // #########################################################################

    EventVisualizer event_visualizer;
    cv::Mat display;

    // Opening Camera
    auto v = Metavision::DeviceDiscovery::list();

    if (v.empty()) {
        std::cout << "No Prophesee Camera detected" << std::endl;
        return false;
    }

    std::cout << "Opening Prophesee Camera.." << std::endl;
    std::unique_ptr<Metavision::Device> device;
    try {
        device = Metavision::DeviceDiscovery::open(v.front());
    } catch (Metavision::HalException &e) { std::cout << "Error exception: " << e.what() << std::endl; }

    if (!device) {
        std::cerr << "Camera opening failed." << std::endl;
        return false;
    }
    std::cout << "Camera open." << std::endl;

    // Register callback function
    auto *i_cddecoder =
            device->get_facility<Metavision::I_EventDecoder<Metavision::EventCD>>();

    if (i_cddecoder) {
        // Register a lambda function to be called on every CD events
        i_cddecoder->add_event_buffer_callback(
                [&event_visualizer](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                    event_visualizer.process_events(begin, end);
                });
    }

    // Decode in additional thread
    auto i_device_control = device->get_facility<Metavision::I_DeviceControl>();
    if (!i_device_control) {
        std::cerr << "Could not get Device Control facility." << std::endl;
    }

    auto i_events_stream = device->get_facility<Metavision::I_EventsStream>();
    if (!i_events_stream) {
        std::cerr << "Could not initialize events stream." << std::endl;
    }

    auto *i_geometry = device->get_facility<Metavision::I_Geometry>();
    if (!i_geometry) {
        std::cerr << "Could not retrieve geometry." << std::endl;
    }

    event_visualizer.setup_display(i_geometry->get_width(), i_geometry->get_height());

    i_device_control->start();
    i_events_stream->start();

    auto *i_decoder = device->get_facility<Metavision::I_Decoder>();
    bool stop_decoding = false;
    bool stop_application = false;
    i_events_stream->start();
    std::thread decoding_loop([&]() {
        using namespace std::chrono;
        milliseconds last_update_monitoring = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        while (!stop_decoding) {
            short ret = i_events_stream->poll_buffer();
            if (ret < 0) {
                std::cout << "End of file" << std::endl;
                i_events_stream->stop();
                i_events_stream->stop_log_raw_data();
                i_device_control->stop();
                std::cout << "Camera stopped." << std::endl;
                stop_decoding = true;
                stop_application = true;
            }

            /// [buffer]
            // Here we polled data, so we can launch decoding
            long n_bytes;
            uint8_t *raw_data = i_events_stream->get_latest_raw_data(n_bytes);

            // This will trigger callbacks set on decoders: in our case EventAnalyzer.process_events
            i_decoder->decode(raw_data, raw_data + n_bytes);
            /// [buffer]
        }
    });

    // #########################################################################
    // PANGOLIN
    // #########################################################################
    pangolin::CreateWindowAndBind("Main",640,480);

    // Panel
    const int UI_WIDTH = 20* pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> a_button("ui.A_Button",false,false);
    pangolin::Var<double> a_double("ui.A_Double",3,0,5);
    pangolin::Var<int> an_int("ui.An_Int",2,0,5);
    pangolin::Var<double> a_double_log("ui.Log_scale",3,1,1E4, true);
    pangolin::Var<bool> a_checkbox("ui.A_Checkbox",false,true);
    pangolin::Var<int> an_int_no_input("ui.An_Int_No_Input",2);
    pangolin::Var<std::string> a_string("ui.A_String", "Edit ME!");

    // Image
    pangolin::View& d_image = pangolin::Display("image")
            .SetAspect(640.0f/480.0f);

    pangolin::Display("multi")
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0)
            .SetLayout(pangolin::LayoutEqual)
            .AddDisplay(d_image);

    pangolin::GlTexture imageTexture(i_geometry->get_width(),
                                     i_geometry->get_height(),
                                     GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);

    while(!pangolin::ShouldQuit()) {
        // Get image from Prophesee camera
        event_visualizer.get_display_frame(display);
        cv::flip(display, display, 0);

        // Image
        if (!display.empty()) {
            imageTexture.Upload(display.data,GL_RGB,GL_UNSIGNED_BYTE);
            d_image.Activate();
            glColor3f(1.0,1.0,1.0);
            imageTexture.RenderToViewport();
        }

        // Necessary
        pangolin::FinishFrame();
    }

    return 0;
}
