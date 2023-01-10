#pragma once

#include "../../camera/basler_camera.h"
#include "../../camera/prophesee_cam.h"

#include "../Constants.h"

namespace Gui {

    class Base {
    public:
        void setupCamera();
        void updateCamera();

        virtual void updateValues() {}

    public:
        camera::Base* getActual() { return cam; }

        cv::Mat getDisplay() { return cam->get_display(); }
        int getWidth() { return cam->get_width(); }
        int getHeight() { return cam->get_height(); }
        std::string getType() { return type; }

    protected:
        camera::Base* cam;
        std::string type;
    };

}