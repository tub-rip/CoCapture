#pragma once

#include "Base.h"

namespace Gui {

    class BaslerWrapper : public Base {
    public:
        BaslerWrapper() {}
        ~BaslerWrapper() {}

    public:
        void setupBasler(Parameters appParams) {
            cam = new camera::BaslerCamera(BaslerParams(appParams));
            setupCamera();
        }
    };

} // Gui