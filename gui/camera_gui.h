#ifndef RIP_COCAPTURE_GUI_CAMERA_GUI_H
#define RIP_COCAPTURE_GUI_CAMERA_GUI_H

#include <memory>

#include <boost/filesystem.hpp>
#include <SDL_opengl.h>

namespace rcg::gui {

    const size_t BUF_SIZE = 128;

    class ICameraGui {
    public:
        virtual void Show(bool* show) = 0;
        virtual void StartRecording(const char* output_dir) = 0;
        virtual void StopRecording()  = 0;
    };

} // rcg::gui

#endif //RIP_COCAPTURE_GUI_CAMERA_GUI_H