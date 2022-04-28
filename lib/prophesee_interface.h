#ifndef RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
#define RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H

#include <iostream>
#include <memory>

#include <metavision/hal/device/device.h>
#include <metavision/hal/utils/hal_exception.h>
#include <metavision/hal/device/device_discovery.h>
#include <metavision/hal/facilities/i_event_decoder.h>
#include <metavision/hal/facilities/i_geometry.h>
#include <metavision/hal/facilities/i_device_control.h>
#include <metavision/hal/facilities/i_events_stream.h>
#include <metavision/hal/facilities/i_decoder.h>

#include "event_visualizer.h"


class PropheseeInterface {
public:
    PropheseeInterface();
    ~PropheseeInterface() = default;

    int get_width() {
        auto *i_geometry = device_->get_facility<Metavision::I_Geometry>();
        return i_geometry->get_width();
    }
    int get_height() {
        auto *i_geometry = device_->get_facility<Metavision::I_Geometry>();
        return i_geometry->get_height();
    }

    void get_display_frame(cv::Mat &display) {
        event_visualizer_.get_display_frame(display);
    }



    void decoding_loop();
    std::unique_ptr<Metavision::Device> device_;
    Metavision::I_EventsStream *i_events_stream_;
    Metavision::I_DeviceControl *i_device_control_;
    Metavision::I_Decoder *i_decoder_;
private:
    bool open_camera_();
    bool sign_up_cd_callback_();
    EventVisualizer event_visualizer_;




};


#endif //RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
