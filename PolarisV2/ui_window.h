#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

namespace polaris::ui::window
{
    //!  Base class for an ImGui window.
    /*!
     * Contains functions like Draw, Resize and Update, called by the UIRenderer.
     */
    class UIWindow
    {
    public:
        bool m_bIsOpen;

        UIWindow();

        virtual void Draw(); //!< Draw is called each tick while the window is open.
        virtual void Resize(); //!< Resize is called when the Fortnite window is resized, regardless of the window being open or closed.
        virtual void Update(); //!< Update is called each tick, regardless of the window being open or closed.
    };
}

#endif // !UI_WINDOW_H
