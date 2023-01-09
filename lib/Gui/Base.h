#pragma once

#include "../camera/basler_camera.h"
#include "../camera/prophesee_cam.h"

#include "Constants.h"

namespace Gui {

    class Base {
    public:
        void setupCamera();
        void updateCamera();

    public:
        camera::Base* getCam() { return cam; }

        cv::Mat getDisplay() { return cam->get_display(); }
        int getWidth() { return cam->get_width(); }
        int getHeight() { return cam->get_height(); }

    protected:
        camera::Base* cam;
    };

} // Gui