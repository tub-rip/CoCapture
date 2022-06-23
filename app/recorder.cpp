#include <iostream>

#include <pangolin/var/var.h>
#include <pangolin/gl/gl.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/display/widgets.h>
#include <pangolin/display/default_font.h>
#include <pangolin/handler/handler.h>
#include <pangolin/var/varextra.h>

#include <opencv2/core.hpp>

#include "../lib/prophesee_interface.h"
#include "../lib/event_visualizer.h"


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
        return 1;
    }

    std::shared_ptr<Metavision::Device> device = prophesee::make_device(v.front());
    auto i_events_stream = device->get_facility<Metavision::I_EventsStream>();

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

    auto i_geometry = device->get_facility<Metavision::I_Geometry>();
    event_visualizer.setup_display(i_geometry->get_width(), i_geometry->get_height());

    std::thread decoding_thread(prophesee::decoding_loop, device);

    // #########################################################################
    // GUI
    // #########################################################################
    pangolin::CreateWindowAndBind("Main",640,480);

    // Panel
    const int UI_WIDTH = 20 * pangolin::default_font().MaxWidth();
    pangolin::CreatePanel("ui")
            .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    pangolin::Var<bool> start_recording("ui.Start-Recording",false,false);
    pangolin::Var<bool> stop_recording("ui.Stop-Recording",false,false);
    bool is_recording = false;
    /*
    pangolin::Var<double> a_double("ui.A_Double",3,0,5);
    pangolin::Var<int> an_int("ui.An_Int",2,0,5);
    pangolin::Var<double> a_double_log("ui.Log_scale",3,1,1E4, true);
    pangolin::Var<bool> a_checkbox("ui.A_Checkbox",false,true);
    pangolin::Var<int> an_int_no_input("ui.An_Int_No_Input",2);
    pangolin::Var<std::string> recording_status("ui.recording_status", "Ready");
     */

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

        // GUI
        if( pangolin::Pushed(start_recording) ) {

            if (!is_recording) {
                std::string out_file_path("events.raw");
                i_events_stream->log_raw_data(out_file_path);
                is_recording = true;
                std::cout << "Start recording to:\n" << out_file_path << std::endl;
            } else {
                std::cout << "Recording is running already" << std::endl;
            }
        }
        if( pangolin::Pushed(stop_recording) ) {
            if (is_recording) {
                i_events_stream->stop_log_raw_data();
                is_recording = false;
                std::cout << "Stopped recording";
            }
            else {
                std::cout << "Cannot stop recording, because it's not running" << std::endl;
            }
        }




        // Necessary
        pangolin::FinishFrame();
    }

    decoding_thread.join();
    return 0;
}
