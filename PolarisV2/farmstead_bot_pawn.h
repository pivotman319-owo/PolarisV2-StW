#ifndef FARMSTEAD_BOT_PAWN_H
#define FARMSTEAD_BOT_PAWN_H

#include "pawn.h"
#include <map>

namespace polaris
{
    namespace pawn
    {
        namespace pawns
        {
            //!  Player Pawn class for an Bot pawn.
            class FarmsteadBotPawn : public Pawn
            {
            public:
                FarmsteadBotPawn(SDK::FVector pos, SDK::FRotator rot, SDK::UFortWeaponItemDefinition* wep);
                ~FarmsteadBotPawn();

                SDK::AFortPlayerPawn* m_pBotPawnActor;
                SDK::AFortPlayerControllerZone* m_pBotPawnController;
                SDK::AFortPlayerStateZone* m_pPlayerState;
                void EquipWeapon(const char* cItemDef, int iGuid);
                void ApplyCustomizationLoadout();
            };
        }
    }
}

#endif // !BOT_PAWN_H
