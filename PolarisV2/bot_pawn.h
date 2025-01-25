#ifndef BOT_PAWN_H
#define BOT_PAWN_H

#include "pawn.h"
#include <map>

namespace polaris
{
    namespace pawn
    {
        namespace pawns
        {
            //!  Player Pawn class for an Bot pawn.
            class BotPawn : public Pawn
            {
            public:
                BotPawn(SDK::FVector pos, SDK::FRotator rot, SDK::TEnumAsByte<SDK::EFortTeam> Team, SDK::UFortWeaponItemDefinition* wep);
                ~BotPawn();

                SDK::AFortPlayerPawn* m_pBotPawnActor;
                SDK::AFortPlayerControllerAthena* m_pBotPawnController;
                SDK::AFortPlayerStateAthena* m_pPlayerState;
                void EquipWeapon(const char* cItemDef, int iGuid);
                void ApplyCustomizationLoadout();
            };
        }
    }
}

#endif // !BOT_PAWN_H
