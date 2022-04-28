//
// Created by friedhelm on 28.04.22.
//

#ifndef RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
#define RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H

#include <memory>

#include <metavision/hal/device/device.h>
#include <metavision/hal/utils/hal_exception.h>
#include <metavision/hal/device/device_discovery.h>
#include <metavision/hal/facilities/i_event_decoder.h>
#include <metavision/hal/facilities/i_geometry.h>
#include <metavision/hal/facilities/i_device_control.h>
#include <metavision/hal/facilities/i_events_stream.h>
#include <metavision/hal/facilities/i_decoder.h>


namespace prophesee {

    std::shared_ptr<Metavision::Device> make_device(std::string serial);
    void decoding_loop(std::shared_ptr<Metavision::Device> device);

} // prophesee

#endif //RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
