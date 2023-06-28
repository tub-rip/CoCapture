#ifndef RIP_COCAPTURE_GUI_CAMERA_GUI_H
#define RIP_COCAPTURE_GUI_CAMERA_GUI_H

#include <memory>

#include <SDL_opengl.h>

namespace rcg::gui {

    const size_t BUF_SIZE = 128;

    class ICameraGui {
    public:
        virtual void Show(bool* show = nullptr) = 0;
    };

} // rcg::gui

#endif //RIP_COCAPTURE_GUI_CAMERA_GUI_H