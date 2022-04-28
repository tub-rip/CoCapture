#include "prophesee_interface.h"

PropheseeInterface::PropheseeInterface() {
   open_camera();
}


bool PropheseeInterface::open_camera() {
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