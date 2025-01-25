#include "main_window.h"
#include "program.h"
#include "sdk_utils.h"
#include "globals.h"
#include "ui_renderer.h"
#include "changelog_window.h"
#include "player_category.h"
#include "window_category.h"

polaris::ui::window::windows::MainWindow* gpMainWindow;

namespace polaris::ui::window::windows
{
    MainWindow::MainWindow()
    {
        gpMainWindow = this;
        m_pAboutWindow = new AboutWindow;
        m_pWatermark = new Watermark;

        m_pAboutWindow->m_bIsOpen = false;

        m_vCategories.push_back(new mainwindow::category::categories::PlayerCategory);
        m_vCategories.push_back(new mainwindow::category::categories::WindowCategory);
    }

    void MainWindow::Draw()
    {
        ImGui::BeginMainMenuBar();
        {
            for (auto& category : m_vCategories)
            {
                category->Draw();
            }

            // Help remains hardcoded, because we always want it to appear last.
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About"))
                {
                    m_pAboutWindow->m_bIsOpen = !m_pAboutWindow->m_bIsOpen;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    void MainWindow::Update()
    {
        m_bIsOpen = gpRenderer->m_bLockFortInput;
    }
}