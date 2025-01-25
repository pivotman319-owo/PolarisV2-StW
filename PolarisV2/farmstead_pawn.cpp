#include "farmstead_pawn.h"
#include "globals.h"
#include "sdk_utils.h"

struct AFortAsQuickBars
{
public:
    unsigned char                                      UnknownData00[0x1A88];
    class SDK::AFortQuickBars* QuickBars;
};
struct AFortAsBuildPreviewMID
{
public:
    unsigned char UnknownData00[0x1928];
    class SDK::UMaterialInstanceDynamic* BuildPreviewMarkerMID;
};
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

namespace polaris
{
    namespace pawn
    {
        namespace pawns
        {
            static SDK::UCustomCharacterPart* pCharacterPartBody;
            static SDK::UCustomCharacterPart* pCharacterPartHead;
            static SDK::UCustomCharacterPart* pCharacterPartHat;

            template<typename T>
            T* FindObject(const std::string& sClassName, const std::string& sQuery)
            {
                for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
                {
                    auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
                    if (pObject != nullptr && pObject->GetFullName().find("F_Med_Head1") == std::string::npos)
                    {
                        if (pObject->GetFullName().rfind(sClassName, 0) == 0 && pObject->GetFullName().find(sQuery) != std::string::npos)
                            return static_cast<T*>(pObject);
                    }
                }

                return nullptr;
            }

            FarmsteadPawn::FarmsteadPawn()
            {
                globals::gpPlayerController->CheatManager->Summon(TEXT("PlayerPawn_Commando_C"));
                m_pPawnActor = static_cast<SDK::AFortPlayerPawn*>(utilities::SDKUtils::FindActor(SDK::AFortPlayerPawn::StaticClass()));
                globals::gpPlayerController->Possess(m_pPawnActor);

                // Load Ramirez onto the pawn and replicate character parts.
                auto playerState = static_cast<SDK::AFortPlayerStateZone*>(globals::gpPlayerController->PlayerState);
                playerState->OnRep_CharacterParts();
                m_pPawnActor->OnCharacterPartsReinitialized();

                //Reset the pawn's actor rotation.
                SDK::FRotator actorRotation = m_pPawnActor->K2_GetActorRotation();
                actorRotation.Pitch = 0;
                actorRotation.Roll = 0;

                m_pPawnActor->K2_SetActorLocationAndRotation(m_pPawnActor->K2_GetActorLocation(), actorRotation, false, true, new SDK::FHitResult());

                // Give the player a pickaxe.
                EquipWeapon("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SR_T05.WID_Harvest_Pickaxe_SR_T05", 0);
                CreateBuildPreviews();
                m_bHasCycledWall = false;
                m_bHasCycledFloor = false;
                m_bHasCycledStair = false;
                m_bHasCycledRoof = false;
                m_bHasCycledWallOnce = false;
                m_bHasCycledFloorOnce = false;
                m_bHasCycledStairOnce = false;
                m_bHasCycledRoofOnce = false;
            }
            FarmsteadPawn::~FarmsteadPawn()
            {
                if (m_pPawnActor != nullptr)
                    m_pPawnActor->K2_DestroyActor();
            }

            void FarmsteadPawn::Update()
            {
                Pawn::Update();

                // Sprinting keybind
                bool wantsToSprint = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController)->bWantsToSprint;
                if (m_bSprint == false)
                {
                    m_bSprint = true;
                    if (m_pPawnActor->CurrentWeapon && !m_pPawnActor->CurrentWeapon->IsReloading() && m_pPawnActor->CurrentWeapon->bIsTargeting == false)
                    {
                        m_pPawnActor->CurrentMovementStyle = wantsToSprint ? SDK::EFortMovementStyle::Sprinting : SDK::EFortMovementStyle::Running;
                    }
                }
                else
                    m_bSprint = false;
            }
            void FarmsteadPawn::EquipWeapon(const char* cItemDef, int iGuid)
            {
                SDK::FGuid guid;
                guid.A = iGuid;
                guid.B = iGuid;
                guid.C = iGuid;
                guid.D = iGuid;

                m_pPawnActor->EquipWeaponDefinition(SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>(cItemDef), guid)->SetOwner(m_pPawnActor);
            }
            void FarmsteadPawn::CreateBuildPreviews()
            {
                SDK::AFortPlayerController* playerController = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
                playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
                m_pBuildPreviewRoof = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass()));
                auto pBuildingEditSupportRoof = reinterpret_cast<SDK::UBuildingEditModeSupport_Roof*>(globals::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Roof::StaticClass(), (*globals::gpWorld), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                pBuildingEditSupportRoof->Outer = m_pBuildPreviewRoof;
                m_pBuildPreviewRoof->EditModeSupport = pBuildingEditSupportRoof;
                auto pComponent = m_pBuildPreviewRoof->GetBuildingMeshComponent();
                m_pStaticRoof = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_RoofC.PBW_W1_RoofC");
                pComponent->SetStaticMesh(m_pStaticRoof);
                pComponent->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(globals::gpPlayerController)->BuildPreviewMarkerMID);
                m_pBuildPreviewRoof->BuildingType = SDK::EFortBuildingType::Roof;
                m_pMetadataRoof = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Roof>("BuildingEditModeMetadata_Roof EMP_Roof_RoofC.EMP_Roof_RoofC");
                m_pBuildPreviewRoof->EditModePatternData = m_pMetadataRoof;
                m_pBuildPreviewRoof->EditModeSupportClass = SDK::UBuildingEditModeSupport_Roof::StaticClass();
                m_pBuildPreviewRoof->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);

                playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
                m_pBuildPreviewStair = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 1));
                auto pBuildingEditSupportStair = reinterpret_cast<SDK::UBuildingEditModeSupport_Stair*>(globals::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Stair::StaticClass(), (*globals::gpWorld), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                pBuildingEditSupportStair->Outer = m_pBuildPreviewStair;
                m_pBuildPreviewStair->EditModeSupport = pBuildingEditSupportStair;
                auto pComponent1 = m_pBuildPreviewStair->GetBuildingMeshComponent();
                m_pStaticStair = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_StairW.PBW_W1_StairW");
                pComponent1->SetStaticMesh(m_pStaticStair);
                pComponent1->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(globals::gpPlayerController)->BuildPreviewMarkerMID);
                m_pBuildPreviewStair->BuildingType = SDK::EFortBuildingType::Stairs;
                m_pMetadataStair = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Stair>("BuildingEditModeMetadata_Stair EMP_Stair_StairW.EMP_Stair_StairW");
                m_pBuildPreviewStair->EditModePatternData = m_pMetadataStair;
                m_pBuildPreviewStair->EditModeSupportClass = SDK::UBuildingEditModeSupport_Stair::StaticClass();
                m_pBuildPreviewStair->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);

                playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
                m_pBuildPreviewFloor = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 2));
                auto pBuildingEditSupportFloor = reinterpret_cast<SDK::UBuildingEditModeSupport_Floor*>(globals::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Floor::StaticClass(), (*globals::gpWorld), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                pBuildingEditSupportFloor->Outer = reinterpret_cast<AFortAsBuildPreview*>(globals::gpPlayerController)->BuildPreviewMarker;
                m_pBuildPreviewFloor->EditModeSupport = pBuildingEditSupportFloor;
                auto pComponent2 = m_pBuildPreviewFloor->GetBuildingMeshComponent();
                m_pStaticFloor = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_Floor.PBW_W1_Floor");
                pComponent2->SetStaticMesh(m_pStaticFloor);
                pComponent2->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(globals::gpPlayerController)->BuildPreviewMarkerMID);
                m_pBuildPreviewFloor->BuildingType = SDK::EFortBuildingType::Floor;
                m_pMetadataFloor = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Floor>("BuildingEditModeMetadata_Floor EMP_Floor_Floor.EMP_Floor_Floor");
                m_pBuildPreviewFloor->EditModePatternData = m_pMetadataFloor;
                m_pBuildPreviewFloor->EditModeSupportClass = SDK::UBuildingEditModeSupport_Floor::StaticClass();
                m_pBuildPreviewFloor->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);

                playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
                m_pBuildPreviewWall = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 3));
                auto pBuildingEditSupportWall = reinterpret_cast<SDK::UBuildingEditModeSupport_Wall*>(globals::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Wall::StaticClass(), (*globals::gpWorld), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                pBuildingEditSupportWall->Outer = m_pBuildPreviewWall;
                m_pBuildPreviewWall->EditModeSupport = pBuildingEditSupportWall;
                auto pComponent3 = m_pBuildPreviewWall->GetBuildingMeshComponent();
                m_pStaticWall = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_Solid.PBW_W1_Solid");
                pComponent3->SetStaticMesh(m_pStaticWall);
                pComponent3->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(globals::gpPlayerController)->BuildPreviewMarkerMID);
                m_pBuildPreviewWall->BuildingType = SDK::EFortBuildingType::Wall;
                m_pMetadataWall = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Wall>("BuildingEditModeMetadata_Wall EMP_Wall_Solid.EMP_Wall_Solid");
                m_pBuildPreviewWall->EditModePatternData = m_pMetadataWall;
                m_pBuildPreviewWall->EditModeSupportClass = SDK::UBuildingEditModeSupport_Wall::StaticClass();
                m_pBuildPreviewWall->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);

                m_pBuildPreviewWall->SetActorHiddenInGame(true);
                m_pBuildPreviewFloor->SetActorHiddenInGame(true);
                m_pBuildPreviewStair->SetActorHiddenInGame(true);
                m_pBuildPreviewRoof->SetActorHiddenInGame(true);
            }
        }
    }
}