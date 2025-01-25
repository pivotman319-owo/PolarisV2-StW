#ifndef WORLD_INSPECTOR_H
#define WORLD_INSPECTOR_H

#include "ui_window.h"

namespace polaris::ui::window::windows
{
    //!  Simple tool to view the world's actors.
    class WorldInspector : public UIWindow
    {
    public:
        WorldInspector();

        void Draw() override;
    };
}

#endif // !WORLD_INSPECTOR_H
