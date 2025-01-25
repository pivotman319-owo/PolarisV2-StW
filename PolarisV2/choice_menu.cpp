#include "choice_menu.h"
#include "frontend_plate.h"
#include "program.h"
#include "ui_renderer.h"
#include "imgui_text_extension.h"

namespace polaris::ui::window::windows
{
    ChoiceMenu::ChoiceMenu()
    {
        m_cChoosingMessage = "Choose a game mode.";
    }

    void ChoiceMenu::Draw()
    {
        gpRenderer->m_bLockFortInput = true;
        ImGui::OpenPopup("Choice Menu");

        auto io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::BeginPopupModal("Choice Menu", &m_bIsOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
        {
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2) - (ImGui::CalcTextSize("Choose a game mode").x * 0.87f), (ImGui::GetWindowSize().y / 2) - 100));
            ImGui::Header("Choose a game mode");

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2) - (55 * 2), (ImGui::GetWindowSize().y / 2) - 30));
            if (ImGui::Button("Classic", ImVec2(90, 90)))
            {
                m_bIsOpen = false;

                gpProgram->m_pMainTable->PushPlate(new tables::plates::FrontendPlate);
                gpRenderer->m_bLockFortInput = false;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            {
                m_cChoosingMessage = "Classic Polaris experience, with no creative functionality.";
            }
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetWindowSize().y / 2));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.3f), "or");
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), (ImGui::GetWindowSize().y / 2) - 30));
            if (ImGui::Button("Creative", ImVec2(90, 90)))
            {
                m_bIsOpen = false;
                tables::plates::FrontendPlate* frontendPlate = new tables::plates::FrontendPlate;;
                frontendPlate->m_bCreativeMode = true;

                gpProgram->m_pMainTable->PushPlate(frontendPlate);
                gpRenderer->m_bLockFortInput = false;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            {
                m_cChoosingMessage = "Build your own creations and share it with your friends!";
            }

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2) - (ImGui::CalcTextSize(m_cChoosingMessage).x / 2), ImGui::GetCursorPosY()));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), m_cChoosingMessage);
            ImGui::End();
        }
    }
}