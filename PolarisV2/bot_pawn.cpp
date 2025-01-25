#include "bot_pawn.h"
#include "globals.h"
#include "sdk_utils.h"

#include <iostream>
#include <fstream>

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

    BotPawn::BotPawn(SDK::FVector pos, SDK::FRotator rot, SDK::TEnumAsByte<SDK::EFortTeam> Team, SDK::UFortWeaponItemDefinition* wep)
    {
        m_pBotPawnController = static_cast<SDK::AFortPlayerControllerAthena*>(utilities::SDKUtils::SpawnActor(SDK::AAthena_PlayerController_C::StaticClass(), &pos, &rot));
        m_pBotPawnActor = static_cast<SDK::AFortPlayerPawn*>(utilities::SDKUtils::SpawnActor(SDK::APlayerPawn_Generic_C::StaticClass(), &pos, &rot));
        m_pBotPawnController->Possess(m_pBotPawnActor);
        m_pBotPawnActor->PlayerState = static_cast<SDK::AFortPlayerStateAthena*>(utilities::SDKUtils::SpawnActor(SDK::AFortPlayerStateAthena::StaticClass(), &pos, &rot));
        m_pBotPawnActor->OnRep_PlayerState();
        m_pBotPawnController->PlayerState = m_pBotPawnActor->PlayerState;
        m_pBotPawnController->OnRep_PlayerState();
        m_pBotPawnController->ServerChangeName(TEXT("FUCKING PAWN SEX PLAYER FUNNY"));
        m_pPlayerState = static_cast<SDK::AFortPlayerStateAthena*>(m_pBotPawnController->PlayerState);
        m_pPlayerState->bHasFinishedLoading = true;
        m_pPlayerState->bHasStartedPlaying = true;
        m_pPlayerState->OnRep_bHasStartedPlaying();
        m_pBotPawnController->ServerSetClientHasFinishedLoading(true);
        m_pBotPawnController->ServerLoadingScreenDropped();
        m_pBotPawnController->bHasInitiallySpawned = true;
        m_pBotPawnController->bAssignedStartSpawn = true;
        m_pBotPawnController->bReadyToStartMatch = true;
        m_pBotPawnController->bClientPawnIsLoaded = true;
        m_pBotPawnController->bHasClientFinishedLoading = true;
        m_pBotPawnController->bHasServerFinishedLoading = true;
        //Reset the pawn's actor rotation.
        SDK::FRotator actorRotation = m_pBotPawnActor->K2_GetActorRotation();
        actorRotation.Pitch = 0;
        actorRotation.Roll = 0;

        m_pBotPawnActor->K2_SetActorLocationAndRotation(m_pBotPawnActor->K2_GetActorLocation(), actorRotation, false, true, new SDK::FHitResult());

        auto playerState = static_cast<SDK::AFortPlayerStateAthena*>(m_pBotPawnController->PlayerState);
        playerState->TeamIndex = Team;
        playerState->OnRep_TeamIndex();

        //auto pawn = static_cast<SDK::AFortPlayerPawnAthena*>(m_pBotPawnActor);
        //pawn->OnRep_CustomizationLoadout();

        // Give the player a pickaxe.
        if (wep != nullptr)
        {
            m_pBotPawnActor->EquipWeaponDefinition(wep, SDK::FGuid());
        }
        // Apply customization loadout.
        ApplyCustomizationLoadout();
    }
    BotPawn::~BotPawn()
    {
        if (m_pBotPawnActor != nullptr)
            m_pBotPawnActor->K2_DestroyActor();
    }
    void BotPawn::ApplyCustomizationLoadout()
    {
        auto playerState = static_cast<SDK::AFortPlayerStateAthena*>(m_pBotPawnController->PlayerState);

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
    void BotPawn::EquipWeapon(const char* cItemDef, int iGuid)
    {
        SDK::FGuid guid;
        guid.A = iGuid;
        guid.B = iGuid;
        guid.C = iGuid;
        guid.D = iGuid;

        m_pBotPawnActor->EquipWeaponDefinition(SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>(cItemDef), guid)->SetOwner(m_pBotPawnActor);
    }
}