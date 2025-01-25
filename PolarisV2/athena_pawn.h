#ifndef ATHENA_PAWN_H
#define ATHENA_PAWN_H

#include "pawn.h"
#include <map>

namespace polaris::pawn::pawns
{
    //!  Player Pawn class for an AthenaPlate.
    class AthenaPawn : public Pawn
    {
    public:
        bool m_bTryingToDeployGlider = false;
        bool m_bPressed1 = false;
        bool m_bSprint = false; // naming it sprint because i have no idea what to name it stay mad
        bool m_bHasCycledWall = false;
        bool m_bHasCycledFloor = false;
        bool m_bHasCycledStair = false;
        bool m_bHasCycledRoof = false;
        bool m_bShouldRespawnOnDeath = true;

        bool m_bHasCycledWallOnce = false;
        bool m_bHasCycledFloorOnce = false;
        bool m_bHasCycledStairOnce = false;
        bool m_bHasCycledRoofOnce = false;
        SDK::FString m_sPendingMaterials = TEXT("WOOD");

        SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewRoof;
        SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewFloor;
        SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewStair;
        SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewWall;
        std::map<std::string, std::string> mPickaxeAsWid
        {
            {"DefaultPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"},
            {"BoltonPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_BoltOn_Athena_C_T01.WID_Harvest_Pickaxe_BoltOn_Athena_C_T01"},
            {"HalloweenScythe", "FortWeaponMeleeItemDefinition WID_Harvest_HalloweenScythe_Athena_C_T01.WID_Harvest_HalloweenScythe_Athena_C_T01"},
            {"HappyPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Smiley_Athena_C_T01.WID_Harvest_Pickaxe_Smiley_Athena_C_T01"},
            {"Pickaxe_Deathvalley", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Deathvalley_Athena_C_T01.WID_Harvest_Pickaxe_Deathvalley_Athena_C_T01"},
            {"Pickaxe_Flamingo", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Flamingo_Athena_C_T01.WID_Harvest_Pickaxe_Flamingo_Athena_C_T01"},
            {"Pickaxe_Lockjaw", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Lockjaw_Athena_C_T01.WID_Harvest_Pickaxe_Lockjaw_Athena_C_T01"},
            {"SickleBatPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SickleBat_Athena_C_T01.WID_Harvest_Pickaxe_SickleBat_Athena_C_T01"},
            {"SkiIcePickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_IcePick_Athena_C_T01.WID_Harvest_Pickaxe_IcePick_Athena_C_T01"},
            {"SpikyPickaxe", "FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Spikey_Athena_C_T01.WID_Harvest_Pickaxe_Spikey_Athena_C_T01"}
        };
        AthenaPawn(SDK::FVector pos, SDK::FRotator rot);
        ~AthenaPawn();

        void Update() override;
        void ApplyCustomizationLoadout(); //!< Applies skin, pickaxe and glider to pawn.
        void EquipWeapon(const char* cItemDef, int iGuid);
        void CreateBuildPreviews(); //!< Creates building previews.
    };
}

#endif // !ATHENA_PAWN_H
