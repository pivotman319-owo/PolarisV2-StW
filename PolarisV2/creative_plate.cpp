#include "creative_plate.h"
#include "trap_build.h"
#include "json.hpp"

#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using trapbuild = polaris::models::offsetfixes::TrapBuild;
using json = nlohmann::json;

namespace polaris::tables::plates
{
    std::map<std::string, SDK::UBlueprintGeneratedClass*> mpClassCache;
    std::map<std::string, SDK::UFortTrapItemDefinition*> mpTrapCache;
    std::map<SDK::FGuid*, SDK::ABuildingSMActor*> mpBuildActors;

    void CreativePlate::Initialize()
    {
        AthenaPlate::Initialize();

        auto cheatManager = static_cast<SDK::UFortCheatManager*>(globals::gpPlayerController->CheatManager);
        cheatManager->AllowRespawn();
        cheatManager->AthenaEndlessGame();
        cheatManager->SetSafeZoneRadius(999999.0f);

        cheatManager->DestroyAll(SDK::AFortAthenaMapInfo::StaticClass());
    }

    void CreativePlate::DestroyAllBuilds()
    {
        for (int i = 0; i < globals::gpLevel->Actors.Count; i++)
        {
            auto actor = globals::gpLevel->Actors[i];
            if (actor && (actor->IsA(SDK::ABuildingSMActor::StaticClass())) && (actor->GetFullName().find("PBWA") != std::string::npos))
            {
                actor->K2_DestroyActor();
            }
        }
    }

    void CreativePlate::LoadCreation(std::filesystem::path path)
    {
        // Clear out any old actors.
        DestroyAllBuilds();

        // Load the creation data into memory.
        std::ifstream streamData(path);
        json jsonData;
        streamData >> jsonData;

        // Load the builds.
        for (int i = 0; i < jsonData["buildPieces"].size(); i++)
        {
            SDK::UBlueprintGeneratedClass* buildClass = nullptr; // The build piece's class
            SDK::ABuildingSMActor* buildPiece = nullptr; // The build piece actor
            SDK::FVector location; // Location of the build piece
            SDK::FRotator rotation; // Rotation of the build piece

            // Place the build in the correct location.
            location.X = jsonData["buildPieces"][i]["location"]["x"];
            location.Y = jsonData["buildPieces"][i]["location"]["y"];
            location.Z = jsonData["buildPieces"][i]["location"]["z"];

            // Rotate the build.
            rotation.Pitch = jsonData["buildPieces"][i]["rotation"]["pitch"];
            rotation.Yaw = jsonData["buildPieces"][i]["rotation"]["yaw"];
            rotation.Roll = jsonData["buildPieces"][i]["rotation"]["roll"];

            // Cache the actor class or cache it now.
            if (mpClassCache.find(jsonData["buildPieces"][i]["name"]) == mpClassCache.end())
            {
                buildClass = SDK::UObject::FindObject<SDK::UBlueprintGeneratedClass>("BlueprintGeneratedClass " + std::string(jsonData["buildPieces"][i]["name"]) + "." + std::string(jsonData["buildPieces"][i]["name"]) + "_C");
                mpClassCache.insert({ jsonData["buildPieces"][i]["name"], buildClass });
            }
            else buildClass = mpClassCache[jsonData["buildPieces"][i]["name"]];

            // Summon the build piece at the correct location and rotation.
            buildPiece = static_cast<SDK::ABuildingSMActor*>(utilities::SDKUtils::SpawnActor(buildClass, &location, &rotation));

            // Apply its health property from storage.
            buildPiece->ForceBuildingHealth(jsonData["buildPieces"][i]["health"]);

            // Apply GUID
            buildPiece->MyGuid.A = jsonData["buildPieces"][i]["guid"]["a"];
            buildPiece->MyGuid.B = jsonData["buildPieces"][i]["guid"]["b"];
            buildPiece->MyGuid.C = jsonData["buildPieces"][i]["guid"]["c"];
            buildPiece->MyGuid.D = jsonData["buildPieces"][i]["guid"]["d"];

            // Insert the build actor to the list.
            mpBuildActors.insert({ &buildPiece->MyGuid, buildPiece });
        }

        // Load the traps.
        for (int i = 0; i < jsonData["traps"].size(); i++)
        {
            SDK::UBlueprintGeneratedClass* trapClass = nullptr; // The trap's class
            SDK::ABuildingTrap* trapActor = nullptr; // The trap actor
            SDK::FVector location; // The trap's location
            SDK::FRotator rotation; // The trap's rotation

            // Place the trap in the correct location
            location.X = jsonData["traps"][i]["location"]["x"];
            location.Y = jsonData["traps"][i]["location"]["y"];
            location.Z = jsonData["traps"][i]["location"]["z"];

            // Rotate the trap properly
            rotation.Pitch = jsonData["traps"][i]["rotation"]["pitch"];
            rotation.Yaw = jsonData["traps"][i]["rotation"]["yaw"];
            rotation.Roll = jsonData["traps"][i]["rotation"]["roll"];

            // Grab the actor class from cache or cache it now.
            if (mpClassCache.find(jsonData["traps"][i]["name"]) == mpClassCache.end())
            {
                trapClass = SDK::UObject::FindObject<SDK::UBlueprintGeneratedClass>("BlueprintGeneratedClass " + std::string(jsonData["traps"][i]["name"]) + "." + std::string(jsonData["traps"][i]["name"]) + "_C");
                mpClassCache.insert({ jsonData["traps"][i]["name"], trapClass });
            }
            else trapClass = mpClassCache[jsonData["traps"][i]["name"]];

            // Summon the trap actor at the correct location and rotation.
            trapActor = static_cast<SDK::ABuildingTrap*>(utilities::SDKUtils::SpawnActor(trapClass, &location, &rotation));

            // Load the trap's build GUID.
            SDK::FGuid* buildGuid = new SDK::FGuid;
            buildGuid->A = jsonData["traps"][i]["buildGuid"]["a"];
            buildGuid->B = jsonData["traps"][i]["buildGuid"]["b"];
            buildGuid->C = jsonData["traps"][i]["buildGuid"]["c"];
            buildGuid->D = jsonData["traps"][i]["buildGuid"]["d"];

            // Attach the trap to it's corresponding build piece.
            trapActor->AttachedTo = mpBuildActors[buildGuid];

            // Grab the trap weapon definition from cache or cache it now.
            auto fixedTrap = reinterpret_cast<models::offsetfixes::TrapBuild*>(trapActor);
            if (mpTrapCache.find(jsonData["traps"][i]["tid"]) == mpTrapCache.end())
            {
                auto result = SDK::UObject::FindObject<SDK::UFortTrapItemDefinition>(jsonData["traps"][i]["tid"]);
                fixedTrap->TrapData = result;

                mpTrapCache.insert({ jsonData["traps"][i]["tid"], result });
            }
            else fixedTrap->TrapData = mpTrapCache[jsonData["traps"][i]["tid"]];

            // Initialize the trap actor.
            trapActor->InitializeKismetSpawnedBuildingActor(mpBuildActors[buildGuid], static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController));
        }

        SDK::AFortPlayerPawn* pawnActor; // The player pawn
        SDK::FVector location; // The pawn's intended location
        SDK::FRotator rotation; // The pawn's intended rotation

        // Load the pawn's location.
        location.X = jsonData["player"]["location"]["x"];
        location.Y = jsonData["player"]["location"]["y"];
        location.Z = jsonData["player"]["location"]["z"];

        // Load the pawn's rotation.
        rotation.Pitch = jsonData["player"]["rotation"]["pitch"];
        rotation.Yaw = jsonData["player"]["rotation"]["yaw"];
        rotation.Roll = jsonData["player"]["rotation"]["roll"];

        // Find the pawn and move it to the intended position.
        pawnActor = static_cast<SDK::AFortPlayerPawn*>(utilities::SDKUtils::FindActor(SDK::AFortPlayerPawn::StaticClass()));
        pawnActor->K2_SetActorLocation(location, false, true, new SDK::FHitResult);
        pawnActor->K2_SetActorRotation(rotation, true);

        // Dispose of the creation data.
        streamData.close();
    }
    void CreativePlate::SaveCreation(std::string name)
    {
        int buildActorIndex = 0; // Amount of builds saved
        int trapActorIndex = 0; // Amount of traps saved
        json creationJson; // Create a json object.

        for (int i = 0; i < globals::gpLevel->Actors.Count; i++)
        {
            auto actor = globals::gpLevel->Actors[i];
            if (actor && (actor->IsA(SDK::ABuildingSMActor::StaticClass())) && (actor->GetFullName().find("PBWA") != std::string::npos))
            {
                auto name = actor->GetFullName();
                auto formattedName = name.substr(0, name.find(" ")).substr(0, name.length() - 1).substr(0, name.find("_C"));
                auto smActor = static_cast<SDK::ABuildingSMActor*>(actor);

                // Save the build piece's properties.
                creationJson["buildPieces"][buildActorIndex]["name"] = formattedName;
                creationJson["buildPieces"][buildActorIndex]["health"] = static_cast<SDK::ABuildingSMActor*>(actor)->GetHealth();
                creationJson["buildPieces"][buildActorIndex]["guid"] =
                {
                    { "a", smActor->MyGuid.A },
                    { "b", smActor->MyGuid.B },
                    { "c", smActor->MyGuid.C },
                    { "d", smActor->MyGuid.D }
                };

                // Save the build piece's location and rotation.
                creationJson["buildPieces"][buildActorIndex]["location"] =
                {
                    { "x", actor->K2_GetActorLocation().X },
                    { "y", actor->K2_GetActorLocation().Y },
                    { "z", actor->K2_GetActorLocation().Z }
                };
                creationJson["buildPieces"][buildActorIndex]["rotation"] =
                {
                    { "pitch", actor->K2_GetActorRotation().Pitch },
                    { "yaw", actor->K2_GetActorRotation().Yaw },
                    { "roll", actor->K2_GetActorRotation().Roll }
                };

                buildActorIndex++;
            }
            if (actor && (actor->IsA(SDK::ABuildingTrap::StaticClass())) && (actor->GetFullName().find("Trap") != std::string::npos))
            {
                auto name = actor->GetFullName();
                auto formattedName = name.substr(0, name.find(" ")).substr(0, name.length() - 1).substr(0, name.find("_C"));
                auto smActor = static_cast<SDK::ABuildingSMActor*>(actor);
                auto trapActor = reinterpret_cast<models::offsetfixes::TrapBuild*>(actor);

                // Save the trap's properties.
                creationJson["traps"][trapActorIndex]["name"] = formattedName;
                creationJson["traps"][trapActorIndex]["tid"] = trapActor->TrapData->GetFullName();
                creationJson["traps"][trapActorIndex]["buildGuid"] =
                {
                    { "a", smActor->MyGuid.A },
                    { "b", smActor->MyGuid.B },
                    { "c", smActor->MyGuid.C },
                    { "d", smActor->MyGuid.D }
                };

                // Save the trap's location and rotation.
                creationJson["traps"][trapActorIndex]["location"] =
                {
                    { "x", actor->K2_GetActorLocation().X },
                    { "y", actor->K2_GetActorLocation().Y },
                    { "z", actor->K2_GetActorLocation().Z }
                };
                creationJson["traps"][trapActorIndex]["rotation"] =
                {
                    { "pitch", actor->K2_GetActorRotation().Pitch },
                    { "yaw", actor->K2_GetActorRotation().Yaw },
                    { "roll", actor->K2_GetActorRotation().Roll }
                };

                trapActorIndex++;
            }
        }

        // Save the name of this creation and the player's location when saved.
        creationJson["info"]["name"] = name;
        creationJson["player"]["location"] =
        {
            { "x", m_pPlayerPawn->m_pPawnActor->K2_GetActorLocation().X },
            { "y", m_pPlayerPawn->m_pPawnActor->K2_GetActorLocation().Y },
            { "z", m_pPlayerPawn->m_pPawnActor->K2_GetActorLocation().Z }
        };
        creationJson["player"]["rotation"] =
        {
            { "pitch", m_pPlayerPawn->m_pPawnActor->K2_GetActorRotation().Pitch },
            { "yaw", m_pPlayerPawn->m_pPawnActor->K2_GetActorRotation().Yaw },
            { "roll", m_pPlayerPawn->m_pPawnActor->K2_GetActorRotation().Roll }
        };

        // Save our JSON object to a Polaris Creation File (pcf).
        std::ofstream o("creations\\" + name + ".pcf");
        o << creationJson.dump(4) << std::endl;
        o.close();
    }
}