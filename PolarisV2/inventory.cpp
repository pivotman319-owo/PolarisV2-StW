#include "inventory.h"
#include "globals.h"
#include "error_utils.h"
#include "inventory_offset_fixes.h"
#include "sdk_utils.h"
#include "program.h"
#include <MinHook.h>
#include <vector>
#include <map>
#include <iostream>

struct AFortAsQuickBars
{
public:
    unsigned char                                      UnknownData00[0x1A88];
    class SDK::AFortQuickBars* QuickBars;
};

namespace polaris
{
    namespace inventory
    {
        void Inventory::SetupInventory()
        {
            // Defining Item Definitions
            m_pEditToolDef = SDK::UObject::FindObject<SDK::UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");
            m_pWallBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
            m_pFloorBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");
            m_pStairBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
            m_pRoofBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");
            auto m_pGoinCommandoDef = SDK::UObject::FindObject<SDK::UFortGadgetItemDefinition>("FortGadgetItemDefinition G_Commando_GoinCommando.G_Commando_GoinCommando");
            auto m_pSentryDef = SDK::UObject::FindObject<SDK::UFortGadgetItemDefinition>("FortDecoItemDefinition D_SentryGun.D_SentryGun");
            auto pWood = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");
            auto pMetal = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition MetalItemData.MetalItemData");
            auto pStone = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition StoneItemData.StoneItemData");
            auto pRockets = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortAmmoItemDefinition AmmoDataRockets.AmmoDataRockets");
            m_pgEditToolDef = utilities::SDKUtils::GenerateGuidPtr();
            m_pgPickaxe = utilities::SDKUtils::GenerateGuidPtr();
            m_pgWallBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgFloorBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgStairBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgRoofBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgSentryDef = utilities::SDKUtils::GenerateGuidPtr();
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pWood);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pMetal);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pStone);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pRockets);

            m_mItems.insert_or_assign(m_pgPickaxe, m_pPickaxeDef);
            m_mItems[m_pgPickaxe] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(m_pPickaxeDef);
            m_mItems.insert_or_assign(m_pgWallBuild, m_pWallBuildDef);
            m_mItems.insert_or_assign(m_pgFloorBuild, m_pFloorBuildDef);
            m_mItems.insert_or_assign(m_pgStairBuild, m_pStairBuildDef);
            m_mItems.insert_or_assign(m_pgRoofBuild, m_pRoofBuildDef);
            m_mItems.insert_or_assign(m_pgEditToolDef, m_pEditToolDef);

            m_mEssentials.insert_or_assign(m_pgPickaxe, m_pPickaxeDef);
            m_mEssentials.insert_or_assign(m_pgWallBuild, m_pWallBuildDef);
            m_mEssentials.insert_or_assign(m_pgFloorBuild, m_pFloorBuildDef);
            m_mEssentials.insert_or_assign(m_pgStairBuild, m_pStairBuildDef);
            m_mEssentials.insert_or_assign(m_pgRoofBuild, m_pRoofBuildDef);
            m_mEssentials.insert_or_assign(m_pgEditToolDef, m_pEditToolDef);
            m_gCommandoGuid = utilities::SDKUtils::GenerateGuidPtr();
            m_mGadgets[m_gCommandoGuid] = reinterpret_cast<SDK::UFortGadgetItemDefinition*>(m_pGoinCommandoDef);
            m_mItems[m_gCommandoGuid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(m_pGoinCommandoDef);
            m_mDecos[m_pgSentryDef] = reinterpret_cast<SDK::UFortDecoItemDefinition*>(m_pSentryDef);
            m_mItems[m_pgSentryDef] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(m_pSentryDef);

            for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
            {
                auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
                if (pObject != nullptr && pObject->GetFullName().find("FortniteGame") == std::string::npos)
                {
                    auto guid = utilities::SDKUtils::GenerateGuidPtr();

                    if (pObject->GetFullName().rfind("FortAmmoItemDefinition", 0) == 0)
                    {
                        //if (pObject->GetFullName().find("Athena") != std::string::npos)
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortWeaponRangedItemDefinition", 0) == 0)
                    {
                        //if (pObject->GetFullName().find("Athena") != std::string::npos || pObject->GetFullName().find("Test") != std::string::npos)
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortWeaponMeleeItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortGadgetItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mGadgets[guid] = reinterpret_cast<SDK::UFortGadgetItemDefinition*>(pObject);
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortIngredientItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortNeverPersistItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortBadgeItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortBackpackItemDefinition", 0) == 0)
                    {
                        //reinterpret_cast<AsInvFilter*>(pObject)->FilterOverride = SDK::EFortInventoryFilter::WeaponRanged;
                        m_mItems[guid] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortTrapItemDefinition", 0) == 0)
                    {
                        //if (pObject->GetFullName().find("Athena") != std::string::npos) {
                        m_mTraps[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                        m_mItems[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                        //}
                    }
                    if (pObject->GetFullName().rfind("FortDecoItemDefinition", 0) == 0)
                    {
                        //if (pObject->GetFullName().find("Athena") != std::string::npos) {
                        m_mDecos[guid] = reinterpret_cast<SDK::UFortDecoItemDefinition*>(pObject);
                        m_mItems[guid] = reinterpret_cast<SDK::UFortDecoItemDefinition*>(pObject);
                        //}
                    }
                }
            }
            auto controller = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
            auto winventory = reinterpret_cast<polaris::models::offsetfixes::WorldInventory*>(controller)->WorldInventory;
            if (winventory)
            {
                SDK::FFortItemList* inv = &winventory->Inventory;
                pItemInsts = &inv->ItemInstances;

                SDK::TArray<struct SDK::FFortItemEntry>* pRepEntries = &inv->ReplicatedEntries;

                for (auto it = m_mItems.begin(); it != m_mItems.end(); it++)
                {
                    int Count = 100;

                    if (it->second != nullptr && it->second->IsA(SDK::UFortWorldItemDefinition::StaticClass()))
                    {
                        auto pWorldItem = static_cast<SDK::UFortWorldItem*>(it->second->CreateTemporaryItemInstanceBP(999, 50));

                        pWorldItem->ItemEntry.bIsReplicatedCopy = true;
                        pWorldItem->ItemEntry.ItemGuid = (*it->first);
                        pWorldItem->bTemporaryItemOwningController = true;
                        pWorldItem->SetOwningControllerForTemporaryItem(controller);
                        reinterpret_cast<polaris::models::offsetfixes::OwnerInventory*>(pWorldItem)->OwnerInventory = winventory;

                        (*pRepEntries).Add(pWorldItem->ItemEntry);
                        (*pItemInsts).Add(pWorldItem);
                    }

                    iInventoryIteration++;
                }

                winventory->HandleInventoryLocalUpdate();
                controller->HandleWorldInventoryLocalUpdate();
            }
        }
        void Inventory::CraftItem(SDK::FString* ItemId, int PostCraftSlot, bool bIsQuickCraft)
        {
            auto controller = static_cast<SDK::AFortPlayerController*>(globals::gpPlayerController);
            auto winventory = reinterpret_cast<polaris::models::offsetfixes::WorldInventory*>(controller)->WorldInventory;
            auto itemInsts = winventory->Inventory.ItemInstances;
            for (int i = 0; i < itemInsts.Num(); ++i)
            {
                auto item = itemInsts[i];
                if (item)
                {
                    std::cout << "Item: " << item->ItemEntry.ItemDefinition->GetFullName();
                }
            }
        }
    }
}
