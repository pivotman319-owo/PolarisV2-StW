#ifndef ATHENA_PLATE_H
#define ATHENA_PLATE_H

#include "peh_plate.h"
#include "athena_pawn.h"
#include "bot_pawn.h"
#include "inventory.h"
#include "quickbars.h"
#include "equip.h"

namespace polaris::tables::plates
{
    //!  Manages Athena's custom behavior.
    /*!
     * This Plate is in charge of essential Athena stuff, like spawning a pawn or dropping the loading screen.
     */
    class AthenaPlate : public PehPlate
    {
    protected:
        pawn::pawns::AthenaPawn* m_pPlayerPawn;
        inventory::Inventory* m_pInventory;
        inventory::quickbars* m_pQuickbars;
        inventory::equip* m_pEquip;
        SDK::UAthenaHUD_C* hud;
        SDK::UFortGlobalUIContext* uiContext;
        bool m_bIsInitialized;
    public:
        bool m_bHasLoadingScreenDropped = false;
        bool m_bGameOver = false;
        int m_iCurrentBuildPiece = 0;
        SDK::UClass* m_pLastBuildClassForWall = SDK::APBWA_W1_Solid_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForFloor = SDK::APBWA_W1_Floor_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForStair = SDK::APBWA_W1_StairW_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForRoof = SDK::APBWA_W1_RoofC_C::StaticClass();

        virtual void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams) override;
        virtual void Update() override;

        virtual void OnEnabled() override;

        virtual void Initialize(); //!< Initialize spawns a pawn and drops the loading screen.

        bool AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB); //!< AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    };
}

#endif // !ATHENA_PLATE_H
