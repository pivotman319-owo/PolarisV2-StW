#include "world_inspector.h"
#include "ui_renderer.h"
#include "globals.h"
#include "imgui_text_extension.h"
#include "sdk_utils.h"
#include "json.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace polaris::ui::window::windows
{
    WorldInspector::WorldInspector()
    {
    }

    void WorldInspector::Draw()
    {
        auto world = (*globals::gpWorld);
        ImGui::SetNextWindowSize(ImVec2(820, 440), ImGuiCond_Appearing);
        ImGui::Begin("World Inspector", &m_bIsOpen);
        {
            if (world == nullptr || (world && world->Levels.Count == 0))
            {
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("There's no levels loaded!").x / 1.15f);
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2);
                ImGui::Header("There's no levels loaded!");

                ImGui::End();
                return;
            }

            static int selectedLevel = 0;
            static int selectedActor = 0;
            {
                ImGui::BeginChild("Hierarchy", ImVec2(300, 0), true);
                {
                    for (int i = 0; i < world->Levels.Count; i++)
                    {
                        if (world->Levels.IsValidIndex(i) && world->Levels[i])
                        {
                            auto level = world->Levels[i];
                            auto name = level->GetFullName();
                            auto formattedName = name.substr(0, name.find(".")).substr(5, name.length() - 1);

                            if (ImGui::TreeNode(formattedName.c_str()))
                            {
                                for (int j = 0; j < level->Actors.Count; j++)
                                {
                                    if (level->Actors.IsValidIndex(j) && level->Actors[j])
                                    {
                                        auto actor = level->Actors[j];

                                        if (ImGui::Selectable(actor->GetName().c_str(), selectedActor == j && selectedLevel == i))
                                        {
                                            selectedLevel = i;
                                            selectedActor = j;
                                        }
                                    }
                                }

                                ImGui::TreePop();
                            }
                        }
                    }
                    ImGui::EndChild();
                }
            }

            ImGui::SameLine();

            auto level = world->Levels[selectedLevel];
            auto actor = level->Actors[selectedActor];
            if (actor)
            {
                bool actorHasAuthority = actor->HasAuthority();

                ImGui::SameLine();

                ImGui::BeginGroup();
                ImGui::BeginChild("Inspector", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
                {
                    ImGui::Header2(actor->GetName().c_str());

                    ImGui::Separator();
                    {
                        ImGui::TextWrapped("ID: %s", actor->GetFullName().c_str());

                        auto location = actor->K2_GetActorLocation();
                        ImGui::TextWrapped("Position: (%d, %d, %d)", location.X, location.Y, location.Z);

                        auto rotation = actor->K2_GetActorRotation();
                        ImGui::TextWrapped("Rotation: (%d, %d, %d)", rotation.Pitch, rotation.Roll, rotation.Yaw);

                        auto scale = actor->GetActorScale3D();
                        ImGui::TextWrapped("Scale: (%d, %d, %d)", scale.X, scale.Y, scale.Z);

                        ImGui::Separator();
                    }
                    {
                        auto ownerLabel = actor->Owner != nullptr ? actor->Owner->GetName().c_str() : "None";
                        ImGui::TextWrapped("Owner: %s", ownerLabel);

                        auto authorityLabel = actor->HasAuthority() ? "Yes" : "No";
                        ImGui::TextWrapped("Has Authority: %s", authorityLabel);
                    }

                    ImGui::EndChild();
                }

                if (ImGui::Button("Destroy"))
                    actor->K2_DestroyActor();

                ImGui::SameLine();

                if (ImGui::Button("Toggle Authority"))
                    actor->Role = actorHasAuthority ? SDK::ENetRole::ROLE_None : SDK::ENetRole::ROLE_Authority;

                ImGui::EndGroup();
            }

            ImGui::End();
        }
    }
}