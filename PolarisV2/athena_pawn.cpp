#include "athena_pawn.h"
#include "bot_pawn.h"
#include "globals.h"
#include "sdk_utils.h"

#include <iostream>
#include <fstream>

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
namespace polaris::pawn::pawns
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

    AthenaPawn::AthenaPawn(SDK::FVector pos, SDK::FRotator rot)
    {
        m_pPawnActor = static_cast<SDK::AFortPlayerPawn*>(utilities::SDKUtils::SpawnActor(SDK::APlayerPawn_Athena_C::StaticClass(), &pos, &rot));
        globals::gpPlayerController->Possess(m_pPawnActor);

        //Reset the pawn's actor rotation.
        SDK::FRotator actorRotation = m_pPawnActor->K2_GetActorRotation();
        actorRotation.Pitch = 0;
        actorRotation.Roll = 0;

        m_pPawnActor->K2_SetActorLocationAndRotation(m_pPawnActor->K2_GetActorLocation(), actorRotation, false, true, new SDK::FHitResult());

        auto playerState = static_cast<SDK::AFortPlayerStateAthena*>(globals::gpPlayerController->PlayerState);
        playerState->TeamIndex = SDK::EFortTeam::HumanPvP_Team69; // funny number go brrrrrr
        playerState->OnRep_TeamIndex();

        auto pawn = static_cast<SDK::AFortPlayerPawnAthena*>(m_pPawnActor);
        pawn->OnRep_CustomizationLoadout();

        // Give the player a pickaxe.
        EquipWeapon(mPickaxeAsWid[pawn->CustomizationLoadout.Character->GetName()].c_str(), 0);
        CreateBuildPreviews();
        m_bHasCycledWall = false;
        m_bHasCycledFloor = false;
        m_bHasCycledStair = false;
        m_bHasCycledRoof = false;
        m_bHasCycledWallOnce = false;
        m_bHasCycledFloorOnce = false;
        m_bHasCycledStairOnce = false;
        m_bHasCycledRoofOnce = false;

        // Apply customization loadout.
        ApplyCustomizationLoadout();
    }
    AthenaPawn::~AthenaPawn()
    {
        if (m_pPawnActor != nullptr)
            m_pPawnActor->K2_DestroyActor();
    }

    void AthenaPawn::Update()
    {
        Pawn::Update();

        // Sprinting keybind
        bool wantsToSprint = static_cast<SDK::AFortPlayerControllerAthena*>(globals::gpPlayerController)->bWantsToSprint;
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
        // Glider redeploy keybind
        auto athenaPawn = static_cast<SDK::AFortPlayerPawnAthena*>(m_pPawnActor);
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            // We do this hacky bool check to avoid the holding button issue.
            if (m_bTryingToDeployGlider == false)
            {
                m_bTryingToDeployGlider = true;
                if (athenaPawn->IsSkydiving() && !athenaPawn->IsParachuteOpen() && !athenaPawn->IsParachuteForcedOpen())
                    athenaPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 2U);
                else if (athenaPawn->IsParachuteOpen() && !athenaPawn->IsParachuteForcedOpen())
                    athenaPawn->CharacterMovement->SetMovementMode(SDK::EMovementMode::MOVE_Custom, 3U);

                athenaPawn->OnRep_IsParachuteOpen(athenaPawn->IsParachuteOpen());
            }
        }
        else
            m_bTryingToDeployGlider = false;
    }
    void AthenaPawn::ApplyCustomizationLoadout()
    {
        auto playerState = static_cast<SDK::AFortPlayerStateAthena*>(globals::gpPlayerController->PlayerState);

        if (!pCharacterPartHead)
            pCharacterPartHead = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Head");
        if (!pCharacterPartBody)
            pCharacterPartBody = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Body");
        if (!pCharacterPartHat)
            pCharacterPartHat = FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart", "Hat_");

        // Assign custom character parts to the player.
        playerState->CharacterParts[0] = pCharacterPartHead;
        playerState->CharacterParts[1] = pCharacterPartBody;
        playerState->CharacterParts[3] = pCharacterPartHat;

        // If no head was found, force Ramirez's head.
        if (!playerState->CharacterParts[0])
            playerState->CharacterParts[0] = SDK::UObject::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart F_Med_Head1.F_Med_Head1");

        // If no body was found, force Ramirez's body.
        if (!playerState->CharacterParts[1])
            playerState->CharacterParts[1] = SDK::UObject::FindObject<SDK::UCustomCharacterPart>("CustomCharacterPart F_Med_Soldier_01.F_Med_Soldier_01");

        playerState->OnRep_CharacterParts();
    }
    void AthenaPawn::EquipWeapon(const char* cItemDef, int iGuid)
    {
        SDK::FGuid guid;
        guid.A = iGuid;
        guid.B = iGuid;
        guid.C = iGuid;
        guid.D = iGuid;

        m_pPawnActor->EquipWeaponDefinition(SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>(cItemDef), guid)->SetOwner(m_pPawnActor);
    }
    void AthenaPawn::CreateBuildPreviews()
    {
        SDK::AFortPlayerController* playerController = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);

        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        m_pBuildPreviewRoof = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass()));

        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        m_pBuildPreviewStair = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 1));

        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        m_pBuildPreviewFloor = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 2));

        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        m_pBuildPreviewWall = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(polaris::utilities::SDKUtils::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 3));

        m_pBuildPreviewWall->SetActorHiddenInGame(true);
        m_pBuildPreviewFloor->SetActorHiddenInGame(true);
        m_pBuildPreviewStair->SetActorHiddenInGame(true);
        m_pBuildPreviewRoof->SetActorHiddenInGame(true);
    }
}