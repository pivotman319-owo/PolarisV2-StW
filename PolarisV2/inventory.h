#ifndef INVENTORY_H
#define INVENTORY_H

#include "peh_plate.h"
#include "sdk_utils.h"
#include "athena_pawn.h"
#include "quickbars.h"

namespace polaris::inventory
{
    class Inventory
    {
    private:
        int foo = 0;
    public:
        SDK::FGuid* m_pgEditToolDef;
        SDK::FGuid* m_pgPickaxe;
        SDK::FGuid* m_pgWallBuild;
        SDK::FGuid* m_pgFloorBuild;
        SDK::FGuid* m_pgStairBuild;
        SDK::FGuid* m_pgRoofBuild;
        SDK::FGuid* m_pgSentryDef;
        SDK::FGuid* m_gCommandoGuid;
        SDK::UFortEditToolItemDefinition* m_pEditToolDef;
        SDK::UFortWeaponMeleeItemDefinition* m_pPickaxeDef;
        SDK::UFortBuildingItemDefinition* m_pWallBuildDef;
        SDK::UFortBuildingItemDefinition* m_pFloorBuildDef;
        SDK::UFortBuildingItemDefinition* m_pStairBuildDef;
        SDK::UFortBuildingItemDefinition* m_pRoofBuildDef;
        SDK::UFortDecoItemDefinition* m_pSentryDef;
        SDK::ABuildingActor* m_pTrapC;
        bool bTrapDone = false;
        std::map<SDK::FGuid*, SDK::UFortWeaponItemDefinition*> m_mItems;
        std::map<SDK::FGuid*, SDK::UFortGadgetItemDefinition*> m_mGadgets;
        std::map<SDK::FGuid*, SDK::UFortDecoItemDefinition*> m_mDecos;
        std::map<SDK::FGuid*, SDK::UFortItemDefinition*> m_mEssentials;
        std::map<SDK::FGuid*, SDK::UFortTrapItemDefinition*> m_mTraps;
        int iInventoryIteration = 0;
        SDK::TArray<class SDK::UFortWorldItem*>* pItemInsts;
        inventory::quickbars* m_pQuickbars;

        void SetupInventory(); //!< SetupInventory initializes the inventory.
        void CraftItem(SDK::FString* ItemId, int PostCraftSlot, bool bIsQuickCraft);
    };
}
#endif // INVENTORY_H