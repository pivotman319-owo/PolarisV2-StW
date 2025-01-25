#include "athena_plate.h"
#include "farmstead_plate.h"
#include "farmstead_bot_pawn.h"
#include "globals.h"
#include "error_utils.h"
#include "inventory.h"
#include "quickbars.h"
#include "sdk_utils.h"
#include "program.h"
#include <MinHook.h>
#include <vector>
#include <map>
#include <iostream>

/*
PVOID(*CollectStupidityInternal)(uint32_t, bool) = nullptr;
PVOID CollectStupidityInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
{
    return NULL;
}
*/
BOOL(*CanCreateInCurrentContext)(SDK::UObject*) = nullptr;
BOOL CanCreateInCurrentContextHook(SDK::UObject* pTemplate)
{
    return TRUE;
}


struct AFortAsBuildPreview
{
public:
    unsigned char UnknownData00[0x1788];
    class SDK::ABuildingPlayerPrimitivePreview* BuildPreviewMarker;
};
struct AFortAsCurrentBuildable
{
public:
    unsigned char UnknownData00[0x1940];
    class SDK::UClass* CurrentBuildableClass;
};
struct AFortAsLastBuildable
{
public:
    unsigned char UnknownData00[0x1948];
    class SDK::UClass* PreviousBuildableClass;
};
struct AFortAsEditActor
{
public:
    unsigned char UnknownData00[0x1A48];
    class SDK::ABuildingSMActor* EditBuildingActor;
};
struct AFortAsQuickBars
{
public:
    unsigned char                                      UnknownData00[0x1A88];
    class SDK::AFortQuickBars* QuickBars;
};
struct paramstruct
{
public:
    class SDK::AFortPlayerController* PlayerThatToggledEditMode;
    class SDK::ABuildingSMActor* EditableActor;
    bool bOpened;
};
namespace polaris
{
    namespace tables
    {
        namespace plates
        {
            void FarmsteadPlate::ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
            {
                if (m_pPlayerPawn != nullptr)
                    m_pPlayerPawn->ProcessEventHook(pObject, pFunction, pParams);

                if (pFunction->GetName().find("ReadyToStartMatch") != std::string::npos && m_bIsInitialized == false)
                    Initialize();
                if (pFunction->GetName().find("HandleBuildingMaterialChanged") != std::string::npos) {
                    auto controller = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
                    auto buildtool = reinterpret_cast<SDK::AFortWeap_BuildingTool*>(controller->MyFortPawn->CurrentWeapon);
                    auto cba = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                    switch (buildtool->LastResourceType) {
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
                if (pFunction->GetName().find("ServerEditBuildingActor") != std::string::npos) {
                    auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
                    if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                        m_iCurrentBuildPiece = 0;
                }
                if (pFunction->GetName().find("OnSuccessfulMatchInteractComplete") != std::string::npos) {
                    auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
                    if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                        m_iCurrentBuildPiece = 0;
                }
                if (pFunction->GetName().find("ServerEndEditingBuildingActor") != std::string::npos) {
                    auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
                    if (editactor->EditBuildingActor != reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker)
                        m_iCurrentBuildPiece = 0;
                }
                if (pFunction->GetName().find("ServerHandleMissionEvent_ToggledEditMode") != std::string::npos) {
                    auto params = reinterpret_cast<paramstruct*>(pParams);
                    auto cba = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                    auto lba = reinterpret_cast<AFortAsLastBuildable*>(globals::gpPlayerController)->PreviousBuildableClass;
                    auto controller = reinterpret_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
                    auto editactor = reinterpret_cast<AFortAsEditActor*>(globals::gpPlayerController);
                    if (params->EditableActor == reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker) {
                        switch (m_iCurrentBuildPiece) {
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
                if (pFunction->GetName().find("ServerLoadingScreenDropped") != std::string::npos)
                    m_pQuickbars->SetupQuickbars();
                if (pFunction->GetName().find("ServerCraftSchematic") != std::string::npos)
                {
                    auto params = reinterpret_cast<SDK::AFortPlayerController_ServerCraftSchematic_Params*>(pParams);
                    SDK::FString ItemId = params->ItemId;
                    int slot = params->PostCraftSlot;
                    bool qcraft = params->bIsQuickCrafted;
                    printf("ItemId: %ls\n", ItemId.c_str());
                    printf("PostCraftSlot: %i\n", slot);
                    printf("bIsQuickCrafted: %s\n", qcraft ? "true" : "false");
                    SDK::UFortInventoryContext* pInvContext = SDK::UObject::FindObject<SDK::UFortInventoryContext>("FortInventoryContext Transient.FortEngine_1.FortLocalPlayer_1.FortInventoryContext_1");
                    SDK::TArray<class SDK::UFortAccountItem*> Items;
                    pInvContext->GetAccountItemsByType(SDK::EFortItemType::Schematic, &Items);
                    auto kislib = SDK::UKismetGuidLibrary::StaticClass()->CreateDefaultObject<SDK::UKismetGuidLibrary>();

                    // print out items list that was generated by UFortInventoryContext::GetAccountItemsByType()
                    for (int i = 0; i < Items.Num(); ++i)
                    {
                        auto item = Items[i];
                        SDK::FGuid itemguid = item->GetItemGuid();
                        SDK::FString itemguidconv = kislib->STATIC_Conv_GuidToString(itemguid);
                        std::cout << "Item: " << item->GetFullName() << " at index: " << i << std::endl;
                        std::cout << "Name: " << item->GetName() << " at index: " << i << std::endl;
                        std::cout << "Item " << i << " GUID: " << itemguidconv.c_str() << std::endl;
                    }
                }
                // UFortInventoryContext::CraftSchematic does not get called upon craft //
                /*
                if (pFunction->GetName().find("FortInventoryContext.CraftSchematic") != std::string::npos)
                {
                    auto params = reinterpret_cast<SDK::UFortInventoryContext_CraftSchematic_Params*>(pParams);
                    SDK::UFortSchematicItem* SchematicItem = params->SchematicItem;
                    SDK::UFortAccountItemDefinition* itemdef = params->SchematicItem->ItemDefinition;
                    SDK::EFortCraftFailCause FailCause = params->FailCause;
                    if (itemdef != nullptr && SchematicItem != nullptr)
                    {
                        std::string AccFullName = itemdef->GetFullName();
                        std::string AccName = itemdef->GetName();
                        std::string SchemFullName = itemdef->GetFullName();
                        std::string SchemName = itemdef->GetName();
                        printf("Account Item Full Name: %s", AccFullName.c_str());
                        printf("Account Item Name: %s", AccFullName.c_str());
                        printf("Schematic Item Full Name: %s", SchemFullName.c_str());
                        printf("Schematic Item Name: %s", SchemFullName.c_str());
                    }
                }
                */
                if (pFunction->GetName().find("ServerExecuteInventoryItem") != std::string::npos) {
                    SDK::FGuid* paramGuid = reinterpret_cast<SDK::FGuid*>(pParams);
                    for (auto it = m_pInventory->m_mItems.begin(); it != m_pInventory->m_mItems.end(); it++) {
                        if (AreGuidsTheSame((*it->first), (*paramGuid)) && !static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController)->IsInBuildMode()) {
                            m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(it->second, (*it->first));
                            m_iCurrentBuildPiece = 0;
                        }
                    }
                    for (auto it = m_pInventory->m_mTraps.begin(); it != m_pInventory->m_mTraps.end(); it++) {
                        if (AreGuidsTheSame((*it->first), (*paramGuid))) {
                            if (m_pInventory->bTrapDone == false) {
                                m_pInventory->m_pTrapC = SDK::UObject::FindObject<SDK::ABuildingActor>("Trap_Floor_Player_Jump_Free_Direction_Pad_C Zone_Onboarding_FarmsteadFort.Zone_Onboarding_FarmsteadFort.PersistentLevel.Trap_Floor_Player_Jump_Free_Direction_Pad_C_1");
                                m_pInventory->bTrapDone = true;
                            }
                            m_pPlayerPawn->m_pPawnActor->PickUpActor(m_pInventory->m_pTrapC, it->second);
                            m_pPlayerPawn->m_pPawnActor->CurrentWeapon->ItemEntryGuid = (*it->first);
                            m_iCurrentBuildPiece = 0;
                        }
                    }
                    if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgWallBuild))) {
                        if (m_iCurrentBuildPiece != 1) {
                            m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pWallBuildDef, (*paramGuid));
                            reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForWall;
                            reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewWall;
                            m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(false);
                            m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                            // building position fix
                            auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                            if (!m_pPlayerPawn->m_bHasCycledWallOnce) {
                                cheatman->BuildWith(TEXT("Wood"));
                                cheatman->BuildWith(TEXT("Stone"));
                                cheatman->BuildWith(TEXT("Metal"));
                                cheatman->BuildWith(TEXT("Wood"));
                                m_pPlayerPawn->m_bHasCycledWallOnce = true;
                            }
                            if (m_pPlayerPawn->m_bHasCycledWall != true) {
                                cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                                m_pPlayerPawn->m_bHasCycledWall = true;
                            }
                            m_iCurrentBuildPiece = 1;
                            m_pLastBuildClassForWall = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                        }
                    }
                    if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgFloorBuild))) {
                        if (m_iCurrentBuildPiece != 2) {
                            m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pFloorBuildDef, (*paramGuid));
                            reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForFloor;
                            reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewFloor;
                            m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(false);
                            m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                            // building position fix
                            auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                            if (!m_pPlayerPawn->m_bHasCycledFloorOnce) {
                                cheatman->BuildWith(TEXT("Wood"));
                                cheatman->BuildWith(TEXT("Stone"));
                                cheatman->BuildWith(TEXT("Metal"));
                                cheatman->BuildWith(TEXT("Wood"));
                                m_pPlayerPawn->m_bHasCycledFloorOnce = true;
                            }
                            if (m_pPlayerPawn->m_bHasCycledFloor != true) {
                                cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                                m_pPlayerPawn->m_bHasCycledFloor = true;
                            }
                            m_iCurrentBuildPiece = 2;
                            m_pLastBuildClassForFloor = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                        }
                    }
                    if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgStairBuild))) {
                        if (m_iCurrentBuildPiece != 3) {
                            m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pStairBuildDef, (*paramGuid));
                            reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForStair;
                            reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewStair;
                            m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(false);
                            m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(true);

                            // building position fix
                            auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                            if (!m_pPlayerPawn->m_bHasCycledStairOnce) {
                                cheatman->BuildWith(TEXT("Wood"));
                                cheatman->BuildWith(TEXT("Stone"));
                                cheatman->BuildWith(TEXT("Metal"));
                                cheatman->BuildWith(TEXT("Wood"));
                                m_pPlayerPawn->m_bHasCycledStairOnce = true;
                            }
                            if (m_pPlayerPawn->m_bHasCycledStair != true) {
                                cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                                m_pPlayerPawn->m_bHasCycledStair = true;
                            }
                            m_iCurrentBuildPiece = 3;
                            m_pLastBuildClassForStair = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                        }
                    }
                    if (AreGuidsTheSame((*paramGuid), (*m_pInventory->m_pgRoofBuild))) {
                        if (m_iCurrentBuildPiece != 4) {
                            m_pPlayerPawn->m_pPawnActor->EquipWeaponDefinition(m_pInventory->m_pRoofBuildDef, (*paramGuid));
                            reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass = m_pLastBuildClassForRoof;
                            reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker = m_pPlayerPawn->m_pBuildPreviewRoof;
                            m_pPlayerPawn->m_pBuildPreviewWall->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewStair->SetActorHiddenInGame(true);
                            m_pPlayerPawn->m_pBuildPreviewRoof->SetActorHiddenInGame(false);

                            // building position fix
                            auto cheatman = static_cast<SDK::UFortCheatManager*>(polaris::globals::gpPlayerController->CheatManager);
                            if (!m_pPlayerPawn->m_bHasCycledRoofOnce) {
                                cheatman->BuildWith(TEXT("Wood"));
                                cheatman->BuildWith(TEXT("Stone"));
                                cheatman->BuildWith(TEXT("Metal"));
                                cheatman->BuildWith(TEXT("Wood"));
                                m_pPlayerPawn->m_bHasCycledRoofOnce = true;
                            }
                            if (m_pPlayerPawn->m_bHasCycledRoof != true) {
                                cheatman->BuildWith(m_pPlayerPawn->m_sPendingMaterials);
                                m_pPlayerPawn->m_bHasCycledRoof = true;
                            }
                            m_iCurrentBuildPiece = 4;
                            m_pLastBuildClassForRoof = reinterpret_cast<AFortAsCurrentBuildable*>(globals::gpPlayerController)->CurrentBuildableClass;
                        }
                    }
                }
                if (pFunction->GetName().find("Tick") != std::string::npos)
                {
                    if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000 && (globals::gpLevel->URL.Map.ToString() != "/Game/Maps/Zones/TheFarmstead/Zone_Onboarding_FarmsteadFort"))
                    {
                        SDK::FVector Pos = m_pPlayerPawn->m_pPawnActor->K2_GetActorLocation();
                        Pos.X = Pos.X + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().X;
                        Pos.Y = Pos.Y + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().Y;
                        Pos.Z = Pos.Z + m_pPlayerPawn->m_pPawnActor->GetActorForwardVector().Z;
                        new pawn::pawns::FarmsteadBotPawn(Pos, m_pPlayerPawn->m_pPawnActor->K2_GetActorRotation(), nullptr);
                    }
                    else if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000 && (globals::gpLevel->URL.Map.ToString() == "/Game/Maps/Zones/TheFarmstead/Zone_Onboarding_FarmsteadFort"))
                        MessageBox(0, L"You cannot spawn bots during the introductory mission.", L"Cannot Spawn Yet", MB_ICONEXCLAMATION);

                    if (GetAsyncKeyState(VK_END) & 0x8000 && !m_bGameOver)
                    {
                        m_bGameOver = true;
                        static_cast<SDK::AFortPlayerControllerZone*>(globals::gpPlayerController)->PostHeroStatMissionCompleted(1);
                        static_cast<SDK::AFortPlayerControllerZone*>(globals::gpPlayerController)->ServerHandleMissionEvent_StartLeavingZone(m_pPlayerPawn->m_pPawnActor);
                    }
                }
            }
            void FarmsteadPlate::Update()
            {
                if (m_pPlayerPawn != nullptr)
                    m_pPlayerPawn->Update();

                // TEMP: Go back to Frontend.
                //if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000)
                //    gpProgram->m_pMainTable->PopPlate();
            }

            void FarmsteadPlate::OnEnabled()
            {
                utilities::SDKUtils::InitSdk();
                utilities::SDKUtils::InitGlobals();

                globals::gpPlayerController->SwitchLevel(TEXT("Zone_Onboarding_FarmsteadFort"));
                //globals::gpPlayerController->SwitchLevel(TEXT("/Game/VehicleAdvBP/Maps/VehicleAdvExampleMap"));
            }

            void FarmsteadPlate::Initialize()
            {
                // Initialize the SDK again.
                m_bIsInitialized = true;
                utilities::SDKUtils::InitSdk();
                utilities::SDKUtils::InitGlobals();
                utilities::SDKUtils::InitPatches();

                // Load SpawnGroups first:
                utilities::SDKUtils::FindOrLoadObject("/Game/AIDirector/SpawnGroups/PvE/BaseHusk.BaseHusk");

                // Load husks into memory.
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

                // forces boolean function CanCreateInCurrentContext to return true
                auto pCanCreateInCurrentContextOffset = utilities::SDKUtils::FindPattern("\xE8\x00\x00\x00\x00\x84\xC0\x75\x7B\x80\x3D\x00\x00\x00\x00\x00\x72\x6B", "x????xxxxxx?????xx");
                if (!pCanCreateInCurrentContextOffset)
                    utilities::ErrorUtils::ThrowException(L"Finding pattern for CanCreateInCurrentContext has failed. Please relaunch Fortnite and try again!");
                auto pCanCreateInCurrentContextAddress = pCanCreateInCurrentContextOffset + 5 + *reinterpret_cast<int32_t*>(pCanCreateInCurrentContextOffset + 1);

                MH_CreateHook(static_cast<LPVOID>(pCanCreateInCurrentContextAddress), CanCreateInCurrentContextHook, reinterpret_cast<LPVOID*>(&CanCreateInCurrentContext));
                MH_EnableHook(static_cast<LPVOID>(pCanCreateInCurrentContextAddress));

                globals::gpPlayerController->CheatManager->Summon(L"FortAIDirector");
                globals::gpPlayerController->CheatManager->Summon(L"FortAIGoalManager");
                static_cast<SDK::UFortCheatManager*>(globals::gpPlayerController->CheatManager)->CraftFree();

                /*
                // Disable garbage collection.
                auto pCollectGarbageInternalAddress = utilities::SDKUtils::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");
                if (!pCollectGarbageInternalAddress)
                    utilities::ErrorUtils::ThrowException(L"Finding pattern for CollectGarbageInternal has failed. Please relaunch Fortnite and try again!");

                MH_CreateHook(static_cast<LPVOID>(pCollectGarbageInternalAddress), CollectStupidityInternalHook, reinterpret_cast<LPVOID*>(&CollectStupidityInternal));
                MH_EnableHook(static_cast<LPVOID>(pCollectGarbageInternalAddress));
                */

                // Spawn a Player Pawn and setup inventory.
                m_pPlayerPawn = new pawn::pawns::FarmsteadPawn;

                m_pInventory = new inventory::Inventory;
                m_pInventory->m_pPickaxeDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SR_T05.WID_Harvest_Pickaxe_SR_T05");
                m_pInventory->m_pSentryDef = SDK::UObject::FindObject<SDK::UFortDecoItemDefinition>("FortDecoItemDefinition D_SentryGun.D_SentryGun");
                m_pQuickbars = new inventory::quickbars;
                m_pInventory->SetupInventory();
                m_pQuickbars->m_pgPickaxe = m_pInventory->m_pgPickaxe;
                m_pQuickbars->m_pgGoingCommando = m_pInventory->m_gCommandoGuid;
                m_pQuickbars->m_pgSentryDef = m_pInventory->m_pgSentryDef;
                m_pQuickbars->m_pgWallBuild = m_pInventory->m_pgWallBuild;
                m_pQuickbars->m_pgFloorBuild = m_pInventory->m_pgFloorBuild;
                m_pQuickbars->m_pgStairBuild = m_pInventory->m_pgStairBuild;
                m_pQuickbars->m_pgRoofBuild = m_pInventory->m_pgRoofBuild;

                /*
                auto asfortquickbars = reinterpret_cast<AFortAsQuickBars*>(polaris::globals::gpPlayerController);
                asfortquickbars->QuickBars = static_cast<SDK::AFortQuickBars*>(utilities::SDKUtils::FindActor(SDK::AFortQuickBars::StaticClass()));
                asfortquickbars->QuickBars->ServerAddItemInternal(*m_pQuickbars->m_pgPickaxe, SDK::EFortQuickBars::Primary, 0);
                */

                // Tell the client that we are ready to start the match, this allows the loading screen to drop.
                static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController)->ServerReadyToStartMatch();
                static_cast<SDK::AGameMode*>((*globals::gpWorld)->AuthorityGameMode)->StartMatch();

                auto fortplayerstate = static_cast<SDK::AFortPlayerStateZone*>(globals::gpPlayerController->PlayerState);
                auto fortplayercontroller = static_cast<SDK::AFortPlayerControllerZone*>(globals::gpPlayerController);

                fortplayerstate->bHasFinishedLoading = true;
                fortplayerstate->bHasStartedPlaying = true;
                fortplayerstate->OnRep_bHasStartedPlaying();

                fortplayercontroller->ServerSetClientHasFinishedLoading(true);

                fortplayercontroller->bHasInitiallySpawned = true;
                fortplayercontroller->bAssignedStartSpawn = true;
                fortplayercontroller->bReadyToStartMatch = true;
                fortplayercontroller->bClientPawnIsLoaded = true;
                fortplayercontroller->bHasClientFinishedLoading = true;
                fortplayercontroller->bHasServerFinishedLoading = true;

                FarmsteadMapCheck();
            }
            void FarmsteadPlate::FarmsteadMapCheck()
            {
                if (globals::gpLevel->URL.Map.ToString() == "/Game/Maps/Zones/TheFarmstead/Zone_Onboarding_FarmsteadFort")
                {
                    globals::gpPlayerController->CheatManager->Summon(L"FortMissionManager");
                    globals::gpPlayerController->CheatManager->Summon(L"Mission_FarmsteadFort_C");
                    globals::gpPlayerController->CheatManager->Summon(L"OBJ_HuskProblem_C");

                    // try to get missions somewhat working
                    auto missionmanager = SDK::UObject::FindObject<SDK::AFortMission>("FortMissionManager Zone_Onboarding_FarmsteadFort");
                    auto farmsteadmission = SDK::UObject::FindObject<SDK::AFortMission>("Mission_FarmsteadFort_C Zone_Onboarding_FarmsteadFort");
                    auto huskobj = SDK::UObject::FindObject<SDK::AFortObjectiveBase>("OBJ_HuskProblem_C Zone_Onboarding_FarmsteadFort");
                    
                    // add our controller to the mission
                    farmsteadmission->AddParticipantAccount(static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController));

                    // do stuff
                    farmsteadmission->bIsMissionVisible = true;
                    farmsteadmission->bIsMissionVisibleOverride = true;
                    farmsteadmission->OnWorldReady();
                    farmsteadmission->OnAllPlayersLoadedIn();
                    farmsteadmission->OnRep_UIIndex();
                    farmsteadmission->OnRep_MissionStatus();
                    farmsteadmission->OnRep_ActiveObjectives();
                    huskobj->bIsObjectiveVisible = true;
                    huskobj->OnRep_ObjectiveVisibilityOverride();
                    huskobj->OnRep_ObjectiveStatus();
                    missionmanager->OnRep_CurrentFocusDisplayData();
                    farmsteadmission->UpdateUI();
                }
            }
            bool FarmsteadPlate::AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB)
            {
                if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
                    return true;
                else
                    return false;
            }
        }
    }
}