#include "Base.h"

namespace Gui {

    void Base::setupCamera() {
        cam->setup_camera();
        cam->set_display(cv::Mat(cam->get_height(), cam->get_width(),
                                 OPENCV_MAT_TYPE, OPENCV_INIT_PIXEL_VALUE));
    }

    void Base::updateCamera() {
        cam->update_display_frame();
    }

} // Gui