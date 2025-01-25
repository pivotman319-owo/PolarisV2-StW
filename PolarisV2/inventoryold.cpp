#include "inventory.h"
#include "globals.h"
#include "error_utils.h"
#include "sdk_utils.h"
#include "program.h"
#include <MinHook.h>
#include <vector>
#include <map>
#include <iostream>

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
            auto pWood = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");
            auto pMetal = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition MetalItemData.MetalItemData");
            auto pStone = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition StoneItemData.StoneItemData");
            auto pRockets = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortAmmoItemDefinition AmmoDataRockets.AmmoDataRockets");
            m_pgEditToolDef = utilities::SDKUtils::GenerateGuidPtr();
            m_pgPickaxe = utilities::SDKUtils::GenerateGuidPtr();
            m_pgWallBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgFloorBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgStairBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_pgRoofBuild = utilities::SDKUtils::GenerateGuidPtr();
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pWood);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pMetal);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pStone);
            m_mItems.insert_or_assign(utilities::SDKUtils::GenerateGuidPtr(), pRockets);
            m_mItems.insert_or_assign(m_pgPickaxe, m_pPickaxeDef);
            m_mItems.insert_or_assign(m_pgWallBuild, m_pWallBuildDef);
            m_mItems.insert_or_assign(m_pgFloorBuild, m_pFloorBuildDef);
            m_mItems.insert_or_assign(m_pgStairBuild, m_pStairBuildDef);
            m_mItems.insert_or_assign(m_pgRoofBuild, m_pRoofBuildDef);
            m_mItems.insert_or_assign(m_pgEditToolDef, m_pEditToolDef);
            for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
            {
                auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
                if (pObject != nullptr && pObject->GetFullName().find("FortniteGame") == std::string::npos)
                {
                    if (pObject->GetFullName().rfind("FortAmmoItemDefinition", 0) == 0)
                    {
                        if (pObject->GetFullName().find("Athena") != std::string::npos)
                            m_mItems[utilities::SDKUtils::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortWeaponRangedItemDefinition", 0) == 0)
                    {
                        if (pObject->GetFullName().find("Athena") != std::string::npos || pObject->GetFullName().find("Test") != std::string::npos)
                            m_mItems[utilities::SDKUtils::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                    }
                    if (pObject->GetFullName().rfind("FortTrapItemDefinition", 0) == 0)
                    {
                        if (pObject->GetFullName().find("Athena") != std::string::npos) {
                            auto guid = utilities::SDKUtils::GenerateGuidPtr();
                            m_mTraps[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                            m_mItems[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                        }
                    }
                }
            }
            auto pInventory1 = static_cast<SDK::AFortInventory*>(polaris::utilities::SDKUtils::FindActor(SDK::AFortInventory::StaticClass()));
            auto pInventory2 = static_cast<SDK::AFortInventory*>(polaris::utilities::SDKUtils::FindActor(SDK::AFortInventory::StaticClass(), 1));
            SDK::AFortInventory* pActualInv = nullptr;

            // joe nuts
            if (pInventory1 && pInventory1->InventoryType == SDK::EFortInventoryType::World)
            {
                pActualInv = pInventory1;
            }

            if (pInventory2 && pInventory2->InventoryType == SDK::EFortInventoryType::World)
            {
                pActualInv = pInventory2;
            }

            if (pActualInv)
            {
                //gaming rgb inventory
                auto myInventory = pActualInv;
                SDK::FFortItemList* inv = &myInventory->Inventory;
                SDK::TArray<class SDK::UFortWorldItem*>* pItemInsts = &inv->ItemInstances;
                pItemInsts->Count = m_mItems.size();
                pItemInsts->Max = m_mItems.size();
                pItemInsts->Data = (class SDK::UFortWorldItem**)::malloc(pItemInsts->Count * sizeof(SDK::UFortWorldItem*));
                for (auto it = m_mItems.begin(); it != m_mItems.end(); it++) {
                    auto pItemEntry = new SDK::FFortItemEntry;
                    pItemEntry->Count = 100;
                    pItemEntry->ItemDefinition = it->second;
                    pItemEntry->Durability = 1000;
                    pItemEntry->Level = 1;
                    pItemEntry->ItemGuid = (*it->first);
                    pItemEntry->bIsDirty = false;
                    pItemEntry->LoadedAmmo = 0;
                    //pItemEntry->ParentInventory.ObjectIndex = myInventory->InternalIndex;
                    auto pWorldItem = reinterpret_cast<SDK::UFortWorldItem*>(polaris::globals::StaticConstructObject_Internal(SDK::UFortWorldItem::StaticClass(), myInventory, SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                    pWorldItem->OwnerInventory = myInventory;
                    pWorldItem->ItemEntry = *pItemEntry;
                    pItemInsts->operator[](iInventoryIteration) = pWorldItem;
                    iInventoryIteration++;
                }
            }
        }
    }
}
