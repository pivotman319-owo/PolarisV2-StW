#ifndef WATERMARK_H
#define WATERMARK_H

#include "ui_window.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

namespace polaris::ui::window::windows
{
    class Watermark : public UIWindow
    {
    public:
        const ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoNav;

        const int CORNER = 2; /*
                                0 = Top left
                                1 = Top right
                                2 = Bottom left
                                3 = Bottom right
                              */

        const float DISTANCE_TO_CORNER = 15.0f;

        Watermark();
        void Draw() override;
    };
}

#endif // WATERMARK_H