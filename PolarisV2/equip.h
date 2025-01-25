#ifndef EQUIP_H
#define EQUIP_H

#include "peh_plate.h"
#include "sdk_utils.h"
#include "athena_pawn.h"
#include "inventory.h"

namespace polaris::inventory
{
    class equip
    {
    private:
        struct AFortAsBuildPreview //spain without the s
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
        int foo = 0;
        SDK::UAthenaHUD_C* hud;
    public:
        inventory::Inventory* m_pInventory;
        pawn::pawns::AthenaPawn* m_pPlayerPawn;

        virtual void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams);

        SDK::FGuid m_gTempSlot;
        SDK::FGuid m_gSlots[5];
        int m_iCurrentBuildPiece = 0;
        SDK::UClass* m_pLastBuildClassForWall = SDK::APBWA_W1_Solid_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForFloor = SDK::APBWA_W1_Floor_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForStair = SDK::APBWA_W1_StairW_C::StaticClass();
        SDK::UClass* m_pLastBuildClassForRoof = SDK::APBWA_W1_RoofC_C::StaticClass();

        bool AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB); //!< AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    };
}

#endif // !EQUIP_H
