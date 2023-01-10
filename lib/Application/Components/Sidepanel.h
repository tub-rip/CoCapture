#pragma once

#include "Component.h"
#include "../Camera/BaslerWrapper.h"
#include "../Camera/PropheseeWrapper.h"

namespace Gui {

    class Sidepanel : public Component {
    public:
        Sidepanel(std::vector<Base*> actualCams) {
            this->actualCams = actualCams;
        }

        ~Sidepanel() {}

    public:
        void show() override;

    public:
        void demoSidepanel();

    private:
        std::vector<Base*> actualCams;
    };

}