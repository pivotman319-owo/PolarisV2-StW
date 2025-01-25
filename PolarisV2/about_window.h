#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include "ui_window.h"

namespace polaris::ui::window::windows
{
    //!  Simple window including credits and build id.
    class AboutWindow : public UIWindow
    {
    public:
        void Draw() override;
    };
}

#endif // !ABOUT_WINDOW_H
