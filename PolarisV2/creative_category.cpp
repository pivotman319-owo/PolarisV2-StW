#include "creative_category.h"
#include "globals.h"
#include "program.h"
#include "sdk_utils.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    CreativeCategory::CreativeCategory(tables::plates::CreativePlate* creativePlate)
    {
        m_cCategoryName = "Creative";
        m_pCreativeMenu = new CreativeMenu(creativePlate);
    }

    void CreativeCategory::DrawButtons()
    {
        if (ImGui::MenuItem("Creations Hub"))
        {
            m_pCreativeMenu->m_bIsOpen = !m_pCreativeMenu->m_bIsOpen;
        }
    }
}