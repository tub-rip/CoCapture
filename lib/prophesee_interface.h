#ifndef RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
#define RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H

#include <iostream>
#include <memory>

#include <metavision/hal/device/device.h>
#include <metavision/hal/utils/hal_exception.h>
#include <metavision/hal/device/device_discovery.h>


class PropheseeInterface {
public:
    PropheseeInterface();
    ~PropheseeInterface() = default;

    bool open_camera();
private:
    std::unique_ptr<Metavision::Device> device_;
};


#endif //RIP_COCAPTURE_GUI_PROPHESEE_INTERFACE_H
