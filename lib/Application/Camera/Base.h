#pragma once

#include "../../camera/basler_camera.h"
#include "../../camera/prophesee_cam.h"
#include "../../camera/realsense_cam.h"

#include "../Constants.h"

namespace Gui {

    class Base {
    public:
        void setupCamera();
        void updateCamera();

        virtual void updateValues() {}
        virtual void startRecording(std::string path) {}
        virtual void stopRecording() {}

    public:
        camera::Base* getActual() { return cam; }

        cv::Mat getDisplay() { return cam->get_display(); }
        int getWidth() { return cam->get_width(); }
        int getHeight() { return cam->get_height(); }
        std::string getType() { return type; }

        std::string getString() { return (type + "_" + std::to_string(id)); }

    protected:
        camera::Base* cam;
        std::string type;
        int id;
    };

}