#ifndef INVENTORY_H
#define INVENTORY_H

#include "peh_plate.h"
#include "sdk_utils.h"
#include "athena_pawn.h"

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
        SDK::UFortEditToolItemDefinition* m_pEditToolDef;
        SDK::UFortWeaponMeleeItemDefinition* m_pPickaxeDef;
        SDK::UFortBuildingItemDefinition* m_pWallBuildDef;
        SDK::UFortBuildingItemDefinition* m_pFloorBuildDef;
        SDK::UFortBuildingItemDefinition* m_pStairBuildDef;
        SDK::UFortBuildingItemDefinition* m_pRoofBuildDef;
        SDK::ABuildingActor* m_pTrapC;
        bool bTrapDone = false;
        std::map<SDK::FGuid*, SDK::UFortWeaponItemDefinition*> m_mItems;
        std::map<SDK::FGuid*, SDK::UFortTrapItemDefinition*> m_mTraps;
        int iInventoryIteration = 0;

        void SetupInventory(); //!< SetupInventory initializes the inventory.
    };
}
#endif // !INVENTORY_H
