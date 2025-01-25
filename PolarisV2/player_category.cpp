#include "player_category.h"
#include "globals.h"
#include "program.h"
#include "sdk_utils.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    PlayerCategory::PlayerCategory()
    {
        m_cCategoryName = "Player";
    }

    void PlayerCategory::DrawButtons()
    {
        if (!globals::gpLocalPlayer->ViewportClient->ViewportConsole)
        {
            if (ImGui::MenuItem("Enable Console"))
            {
                utilities::SDKUtils::ToggleUnrealConsole(true);
            }
        }
        else if (globals::gpLocalPlayer->ViewportClient->ViewportConsole)
        {
            if (ImGui::MenuItem("Disable Console"))
            {
                utilities::SDKUtils::ToggleUnrealConsole(false);
            }
        }

        if (ImGui::MenuItem("Exit"))
        {
            ExitProcess(0);
        }
    }
}