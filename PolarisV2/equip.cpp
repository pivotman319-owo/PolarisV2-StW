#include "equip.h"
#include "globals.h"
#include "error_utils.h"
#include "sdk_utils.h"
#include "program.h"
#include <MinHook.h>
#include <vector>
#include <map>
#include <iostream>

namespace polaris::inventory
{
    void inventory::equip::ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
    {
        if (pFunction->GetName().find("HandleBuildingMaterialChanged") != std::string::npos)
        {
            auto controller = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
            auto buildtool = reinterpret_cast<SDK::AFortWeap_BuildingTool*>(controller->MyFortPawn->CurrentWeapon);
            auto cba = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
            switch (buildtool->LastResourceType)
            {
            case SDK::EFortResourceType::Wood: //stone
                m_pPlayerPawn->m_sPendingMaterials = TEXT("Stone");
                break;
            case SDK::EFortResourceType::Stone: //metal
                m_pPlayerPawn->m_sPendingMaterials = TEXT("Metal");
                break;
            case SDK::EFortResourceType::Metal: //wood
                m_pPlayerPawn->m_sPendingMaterials = TEXT("Wood");
                break;
            }
            m_pPlayerPawn->m_bHasCycledWall = false;
            m_pPlayerPawn->m_bHasCycledFloor = false;
            m_pPlayerPawn->m_bHasCycledStair = false;
            m_pPlayerPawn->m_bHasCycledRoof = false;
        }
        if (pFunction->GetName().find("ServerEditBuildingActor") != std::string::npos)
        {
            auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
            if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                m_iCurrentBuildPiece = 0;
        }
        if (pFunction->GetName().find("OnSuccessfulMatchInteractComplete") != std::string::npos)
        {
            auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
            if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                m_iCurrentBuildPiece = 0;
        }
        if (pFunction->GetName().find("ServerEndEditingBuildingActor") != std::string::npos)
        {
            auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
            if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                m_iCurrentBuildPiece = 0;
        }
        if (pFunction->GetName().find("ServerHandleMissionEvent_ToggledEditMode") != std::string::npos)
        {
            auto params = reinterpret_cast<SDK::AFortPlayerController_ServerHandleMissionEvent_ToggledEditMode_Params*>(pParams);
            auto cba = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
            auto lba = reinterpret_cast<AFortAsLastBuildable*>(globals::gpPlayerController)->PreviousBuildableClass;
            auto controller = reinterpret_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
            auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
            if (params->EditableActor == reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
            {
                switch (m_iCurrentBuildPiece)
                {
                case 1:
                    m_pLastBuildClassForWall = cba;
                    lba = m_pLastBuildClassForWall;
                    break;
                case 2:
                    m_pLastBuildClassForFloor = cba;
                    lba = m_pLastBuildClassForFloor;
                    break;
                case 3:
                    m_pLastBuildClassForStair = cba;
                    lba = m_pLastBuildClassForStair;
                    break;
                case 4:
                    m_pLastBuildClassForRoof = cba;
                    lba = m_pLastBuildClassForRoof;
                    break;
                }
            }
        }
        if (pFunction->GetName().find("ServerExecuteInventoryItem") != std::string::npos)
        {
            SDK::FGuid* paramGuid = reinterpret_cast<SDK::FGuid*>(pParams);
            for (auto it = m_pInventory->m_mItems.begin(); it != m_pInventory->m_mItems.end(); it++)
            {
                if (AreGuidsTheSame((*it->first), (*paramGuid)) && !static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController)->IsInBuildMode())
                {
                    m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(it->second, (*it->first));
                    m_iCurrentBuildPiece = 0;
                }
            }
            for (auto it = m_pInventory->m_mTraps.begin(); it != m_pInventory->m_mTraps.end(); it++)
            {
                if (AreGuidsTheSame((*it->first), (*paramGuid)))
                {
                    if (!m_pInventory->bTrapDone)
                    {
                        m_pInventory->m_pTrapC = nullptr;
                        m_pInventory->bTrapDone = true;
                    }
                    m_pPlayerPawn->m_pPawnActor->PickUpActor(m_pInventory->m_pTrapC, it->second);
                    m_pPlayerPawn->m_pPawnActor->CurrentWeapon->ItemEntryGuid = (*it->first);
                    m_iCurrentBuildPiece = 0;
                }
            }
            if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgWallBuild)))
            {
                if (m_iCurrentBuildPiece != 1)
                {
                    m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pWallBuildDef, (*paramGuid));
                    reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForWall;
                    reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewWall;
                    m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(false);
                    m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                    // building position fix
                    auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                    if (!m_pPlayerPawn->m_bHasCycledWallOnce)
                    {
                        cheatman->BuildWith(TEXT("Wood"));
                        cheatman->BuildWith(TEXT("Stone"));
                        cheatman->BuildWith(TEXT("Wood"));
                        m_pPlayerPawn->m_bHasCycledWallOnce = true;
                    }
                    if (m_pPlayerPawn->m_bHasCycledWall != true)
                    {
                        cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                        m_pPlayerPawn->m_bHasCycledWall = true;
                    }
                    m_iCurrentBuildPiece = 1;
                    m_pLastBuildClassForWall = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                }
            }
            if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgFloorBuild)))
            {
                if (m_iCurrentBuildPiece != 2)
                {
                    m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pFloorBuildDef, (*paramGuid));
                    reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForFloor;
                    reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewFloor;
                    m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(false);
                    m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                    // building position fix
                    auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                    if (!m_pPlayerPawn->m_bHasCycledFloorOnce)
                    {
                        cheatman->BuildWith(TEXT("Wood"));
                        cheatman->BuildWith(TEXT("Stone"));
                        cheatman->BuildWith(TEXT("Wood"));
                        m_pPlayerPawn->m_bHasCycledFloorOnce = true;
                    }
                    if (m_pPlayerPawn->m_bHasCycledFloor != true)
                    {
                        cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                        m_pPlayerPawn->m_bHasCycledFloor = true;
                    }
                    m_iCurrentBuildPiece = 2;
                    m_pLastBuildClassForFloor = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                }
            }
            if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgStairBuild)))
            {
                if (m_iCurrentBuildPiece != 3)
                {
                    m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pStairBuildDef, (*paramGuid));
                    reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForStair;
                    reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewStair;
                    m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(false);
                    m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                    // building position fix
                    auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                    if (!m_pPlayerPawn->m_bHasCycledStairOnce)
                    {
                        cheatman->BuildWith(TEXT("Wood"));
                        cheatman->BuildWith(TEXT("Stone"));
                        cheatman->BuildWith(TEXT("Wood"));
                        m_pPlayerPawn->m_bHasCycledStairOnce = true;
                    }
                    if (m_pPlayerPawn->m_bHasCycledStair != true)
                    {
                        cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                        m_pPlayerPawn->m_bHasCycledStair = true;
                    }
                    m_iCurrentBuildPiece = 3;
                    m_pLastBuildClassForStair = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                }
            }
            if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgRoofBuild)))
            {
                if (m_iCurrentBuildPiece != 4)
                {
                    m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pRoofBuildDef, (*paramGuid));
                    reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForRoof;
                    reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewRoof;
                    m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                    m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(false);

                    // building position fix
                    auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                    if (!m_pPlayerPawn->m_bHasCycledRoofOnce)
                    {
                        cheatman->BuildWith(TEXT("Wood"));
                        cheatman->BuildWith(TEXT("Stone"));
                        cheatman->BuildWith(TEXT("Wood"));
                        m_pPlayerPawn->m_bHasCycledRoofOnce = true;
                    }
                    if (m_pPlayerPawn->m_bHasCycledRoof != true)
                    {
                        cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                        m_pPlayerPawn->m_bHasCycledRoof = true;
                    }
                    m_iCurrentBuildPiece = 4;
                    m_pLastBuildClassForRoof = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                }
            }
        }
    }

    bool inventory::equip::AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB)
    {
        if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
            return true;
        else
            return false;
    }
}
