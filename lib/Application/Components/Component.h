#pragma once

#include "imgui.h"

#include "../Gui.h"

namespace Gui {

    class Component {
    public:
        virtual void show() = 0;
    };

}
