#ifndef RIP_COCAPTURE_GUI_UTILS_H
#define RIP_COCAPTURE_GUI_UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>

#include "parameters.h"
#include "camera/base.h"
#include "camera/prophesee_cam.h"

namespace utils {

    namespace po = boost::program_options;

    bool parse_comman_line(int argc, const char *argv[], Parameters &params);

    void start_recording(std::vector<camera::Base*>& cameras);
    void stop_recording(std::vector<camera::Base*>& cameras);
    void set_roi(Metavision::Camera &cam);
}

#endif //RIP_COCAPTURE_GUI_UTILS_H
