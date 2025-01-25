#include "window_category.h"
#include "globals.h"
#include "program.h"
#include "sdk_utils.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    WindowCategory::WindowCategory()
    {
        m_cCategoryName = "Window";

        m_pWorldInspector = new WorldInspector;
        m_pWorldInspector->m_bIsOpen = false;
    }

    void WindowCategory::DrawButtons()
    {
        if (ImGui::MenuItem("World Inspector"))
        {
            m_pWorldInspector->m_bIsOpen = !m_pWorldInspector->m_bIsOpen;
        }
    }
}