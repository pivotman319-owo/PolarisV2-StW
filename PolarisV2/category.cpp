#include "category.h"

#include <string>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <iostream>

namespace polaris::ui::window::windows::mainwindow
{
    void Category::DrawButtons()
    {
    }

    void Category::Draw()
    {
        if (ImGui::BeginMenu(m_cCategoryName))
        {
            for (auto& category : m_vCategories)
            {
                category->Draw();
            }
            DrawButtons();
            ImGui::EndMenu();
        }
    }
}