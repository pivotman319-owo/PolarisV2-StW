#ifndef FARMSTEAD_PLATE_H
#define FARMSTEAD_PLATE_H

#include "peh_plate.h"
#include "farmstead_pawn.h"
#include "inventory.h"
#include "quickbars.h"

namespace polaris
{
    namespace tables
    {
        namespace plates
        {
            //!  Manages Farmstead's custom behavior.
            /*!
            * ez pirate StW with this plate.
            * Not included in prod or beta builds.
            */
            class FarmsteadPlate : public PehPlate
            {
            private:
                pawn::pawns::FarmsteadPawn* m_pPlayerPawn;
                inventory::Inventory* m_pInventory;
                inventory::quickbars* m_pQuickbars;
                SDK::AFortQuickBars* m_pQB;
                bool m_bIsInitialized;
                bool m_bGameOver;
            public:
                int m_iCurrentBuildPiece = 0;
                SDK::UClass* m_pLastBuildClassForWall = SDK::APBWA_W1_Solid_C::StaticClass();
                SDK::UClass* m_pLastBuildClassForFloor = SDK::APBWA_W1_Floor_C::StaticClass();
                SDK::UClass* m_pLastBuildClassForStair = SDK::APBWA_W1_StairW_C::StaticClass();
                SDK::UClass* m_pLastBuildClassForRoof = SDK::APBWA_W1_RoofC_C::StaticClass();

                void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams) override;
                void Update() override;

                void OnEnabled() override;

                void Initialize(); //!< Initialize spawns a pawn gives it a pickaxe.

                void FarmsteadMapCheck();

                bool AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB); //!< AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            };
        }
    }
}

#endif // !FARMSTEAD_PLATE_H