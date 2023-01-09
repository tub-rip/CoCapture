#pragma once

#include "Base.h"

namespace Gui {

    class PropheseeWrapper : public Base {
    public:
        PropheseeWrapper() {}
        ~PropheseeWrapper() {}

    public:
        void setupProphesee(Parameters appParams, std::string mode, int index) {
            cam = new camera::PropheseeCam(PropheseeParams(appParams, mode, index));
            setupCamera();
        }
    };

}