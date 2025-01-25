#include "athena_plate.h"
#include "globals.h"
#include "error_utils.h"
#include "sdk_utils.h"
#include "inventory.h"
#include "quickbars.h"
#include "program.h"
#include <MinHook.h>
#include <vector>
#include <map>
#include <iostream>

/*
PVOID(*CollectGarbageInternal)(uint32_t, bool) = nullptr;
PVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
{
    return NULL;
}
*/

namespace polaris::tables::plates
{
    void AthenaPlate::ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
    {
        /*
        * Made irrelevant by quickbar code from Wiktor
        if (pFunction->GetName().find("ServerAddItemInternal") != std::string::npos)
        {
            auto params = reinterpret_cast<SDK::AFortQuickBars_ServerAddItemInternal_Params*>(pParams);
            int slot = params->Slot;
            SDK::EFortQuickBars quickbar = params->InQuickBar;
            SDK::FGuid guid = params->Item;
            if (slot != -1 && quickbar == SDK::EFortQuickBars::Secondary)
            {
                for (auto it = m_pInventory->m_mItems.begin(); it != m_pInventory->m_mItems.end(); it++)
                {
                    if (m_pEquip->AreGuidsTheSame((*it->first), guid))
                    {
                        SDK::FLinearColor color{ 100,100,100,100 };
                        if (!hud)
                        {
                            hud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
                        }
                        if (hud != nullptr)
                        {
                            SDK::FSlateBrush brush = it->second->GetSmallPreviewImageBrush();
                            if (&brush)
                            {
                                hud->QuickbarSecondary->QuickbarSlots[params->Slot]->Empty->SetBrush(brush);
                                hud->QuickbarSecondary->QuickbarSlots[params->Slot]->Empty->SetColorAndOpacity(color);
                            }
                        }
                    }
                }
            }
        }
        */
        if (m_pPlayerPawn != nullptr)
            m_pPlayerPawn->ProcessEventHook(pObject, pFunction, pParams);

        if (m_pEquip != nullptr)
            m_pEquip->ProcessEventHook(pObject, pFunction, pParams);

        if (pFunction->GetName().find("Tick") != std::string::npos)
        {
            if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000)
            {
                SDK::FVector Pos = m_pPlayerPawn->m_pPawnActor->K2_GetActorLocation();
                Pos.X = Pos.X + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().X;
                Pos.Y = Pos.Y + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().Y;
                Pos.Z = Pos.Z + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().Z;
                new pawn::pawns::BotPawn(Pos, m_pPlayerPawn->m_pPawnActor->K2_GetActorRotation(), SDK::EFortTeam::HumanPvP_Team2, nullptr);
            }
            if (GetAsyncKeyState(VK_END) & 0x8000 && !m_bGameOver)
            {
                m_bGameOver = true;
                static_cast<SDK::AFortPlayerControllerAthena*>(globals::gpPlayerController)->ClientNotifyWon();
                static_cast<SDK::AFortPlayerControllerAthena*>(globals::gpPlayerController)->PlayWinEffects();
            }
        }

        // Victory Royale screen
        if (pFunction->GetName().find("BndEvt__LeaveButton_K2Node_ComponentBoundEvent_76_CommonButtonClicked__DelegateSignature") != std::string::npos)
        {
            if (!uiContext)
            {
                uiContext = SDK::UObject::FindObject<SDK::UFortGlobalUIContext>("FortGlobalUIContext Transient.FortEngine_1.FortLocalPlayer_1.FortGlobalUIContext_1");
            }
            // the below function just straight-up exits the engine without any confirmation.
            // UFortGlobalUIContext::AllowQuit() does jack shit
            // POSSIBLE ANTI-CRASH SOLUTION: invoke a MessageBox asking to exit the game instead?
            uiContext->QuitGame();
        }

        if (pFunction->GetName().find("ReadyToStartMatch") != std::string::npos && m_bIsInitialized == false)
            Initialize();

        if (pFunction->GetName().find("ServerAttemptAircraftJump") != std::string::npos && static_cast<SDK::AFortPlayerControllerAthena*>(globals::gpPlayerController)->IsInAircraft())
        {
            m_pPlayerPawn = new pawn::pawns::AthenaPawn(static_cast<SDK::AAthena_GameState_C*>((*globals::gpWorld)->GameState)->GetAircraft()->K2_GetActorLocation(),
                reinterpret_cast<SDK::AFortPlayerControllerAthena_ServerAttemptAircraftJump_Params*>(pParams)->ClientRotation);
            m_pEquip->m_pPlayerPawn = m_pPlayerPawn;
        }
        if (pFunction->GetName().find("AircraftExitedDropZone") != std::string::npos && static_cast<SDK::AFortPlayerControllerAthena*>(globals::gpPlayerController)->IsInAircraft())
        {
            m_pPlayerPawn = new pawn::pawns::AthenaPawn(static_cast<SDK::AAthena_GameState_C*>((*globals::gpWorld)->GameState)->GetAircraft()->K2_GetActorLocation(),
                SDK::FRotator{ 0,0,0 });
            m_pEquip->m_pPlayerPawn = m_pPlayerPawn;
        }
        /*
        if (pFunction->GetName().find("ServerLoadingScreenDropped") != std::string::npos)
            m_pQuickbars->SetupQuickbars();
        */
    }
    void AthenaPlate::Update()
    {
        if (m_pPlayerPawn != nullptr)
            m_pPlayerPawn->Update();
    }

    void AthenaPlate::OnEnabled()
    {
        utilities::SDKUtils::InitSdk();
        utilities::SDKUtils::InitGlobals();

        globals::gpPlayerController->SwitchLevel(TEXT("Athena_Terrain"));
        auto kislib = SDK::UKismetSystemLibrary::StaticClass()->CreateDefaultObject<SDK::UKismetSystemLibrary>();
        //kislib->STATIC_ExecuteConsoleCommand((*globals::gpWorld), TEXT("open 25.57.168.149"), globals::gpPlayerController);
    }

    void AthenaPlate::Initialize()
    {
        //Initialize the SDK again.
        m_bIsInitialized = true;
        utilities::SDKUtils::InitSdk();
        utilities::SDKUtils::InitGlobals();
        utilities::SDKUtils::InitPatches();
        //Load Husks Into Memory.
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn.HuskPawn_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Fire.HuskPawn_Fire_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Ice.HuskPawn_Ice_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Lightning.HuskPawn_Lightning_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Beehive.HuskPawn_Beehive_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell.HuskPawn_Bombshell_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell_Poison.HuskPawn_Bombshell_Poison_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf.HuskPawn_Dwarf_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Fire.HuskPawn_Dwarf_Fire_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Ice.HuskPawn_Dwarf_Ice_C");
        utilities::SDKUtils::FindOrLoadObject("/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf_Lightning.HuskPawn_Dwarf_Lightning_C");

        /*
        // Disable garbage collection.
        auto pCollectGarbageInternalAddress = utilities::SDKUtils::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");
        if (!pCollectGarbageInternalAddress)
            utilities::ErrorUtils::ThrowException(L"Finding pattern for CollectGarbageInternal has failed. Please relaunch Fortnite and try again!");

        MH_CreateHook(static_cast<LPVOID>(pCollectGarbageInternalAddress), CollectGarbageInternalHook, reinterpret_cast<LPVOID*>(&CollectGarbageInternal));
        MH_EnableHook(static_cast<LPVOID>(pCollectGarbageInternalAddress));
        */

        // Spawn a Player Pawn and setup inventory.
        m_pPlayerPawn = new pawn::pawns::AthenaPawn(globals::gpPlayerController->SpectatorPawn->K2_GetActorLocation(),
            globals::gpPlayerController->SpectatorPawn->K2_GetActorRotation());

        m_pInventory = new inventory::Inventory;
        m_pInventory->m_pPickaxeDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>(m_pPlayerPawn->mPickaxeAsWid[static_cast<SDK::AFortPlayerPawnAthena*>(m_pPlayerPawn->m_pPawnActor)->CustomizationLoadout.Character->GetName()].c_str());
        m_pQuickbars = new inventory::quickbars;
        m_pInventory->SetupInventory();

        m_pQuickbars->m_pgPickaxe = m_pInventory->m_pgPickaxe;
        m_pQuickbars->m_pgWallBuild = m_pInventory->m_pgWallBuild;
        m_pQuickbars->m_pgFloorBuild = m_pInventory->m_pgFloorBuild;
        m_pQuickbars->m_pgStairBuild = m_pInventory->m_pgStairBuild;
        m_pQuickbars->m_pgRoofBuild = m_pInventory->m_pgRoofBuild;

        m_pEquip = new inventory::equip;
        m_pEquip->m_pInventory = m_pInventory;
        m_pEquip->m_pPlayerPawn = m_pPlayerPawn;
        
        // Tell the client that we are ready to start the match, this allows the loading screen to drop.
        static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController)->ServerReadyToStartMatch();
        static_cast<SDK::AGameMode*>((*globals::gpWorld)->AuthorityGameMode)->StartMatch();
        m_bHasLoadingScreenDropped = true;
    }
}