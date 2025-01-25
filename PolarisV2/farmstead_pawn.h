#ifndef FARMSTEAD_PAWN_H
#define FARMSTEAD_PAWN_H

#include "pawn.h"

namespace polaris
{
    namespace pawn
    {
        namespace pawns
        {
            //!  Player Pawn class for a FarmsteadPlate.
            class FarmsteadPawn : public Pawn
            {
            public:
                bool m_bPressedF1 = false;
                bool m_bPressedF2 = false;
                bool m_bPressedF3 = false;
                bool m_bPressedF4 = false;
                bool m_bPressed1 = false;
                bool m_bSprint = false; // naming it sprint because i have no idea what to name it stay mad
                bool m_bHasCycledWall = false;
                bool m_bHasCycledFloor = false;
                bool m_bHasCycledStair = false;
                bool m_bHasCycledRoof = false;

                bool m_bHasCycledWallOnce = false;
                bool m_bHasCycledFloorOnce = false;
                bool m_bHasCycledStairOnce = false;
                bool m_bHasCycledRoofOnce = false;
                SDK::FString m_sPendingMaterials = TEXT("WOOD");


                SDK::UStaticMesh* m_pStaticRoof;
                SDK::UStaticMesh* m_pStaticWall;
                SDK::UStaticMesh* m_pStaticFloor;
                SDK::UBuildingEditModeMetadata_Wall* m_pMetadataWall;
                SDK::UBuildingEditModeMetadata_Roof* m_pMetadataRoof;
                SDK::UBuildingEditModeMetadata_Stair* m_pMetadataStair;
                SDK::UBuildingEditModeMetadata_Floor* m_pMetadataFloor;
                SDK::UStaticMesh* m_pStaticStair;
                SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewRoof;
                SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewFloor;
                SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewStair;
                SDK::ABuildingPlayerPrimitivePreview* m_pBuildPreviewWall;
                FarmsteadPawn();
                ~FarmsteadPawn();

                void Update() override;
                void EquipWeapon(const char* cItemDef, int iGuid);
                void CreateBuildPreviews(); //!< Creates building previews.
            };
        }
    }
}

#endif // !FARMSTEAD_PAWN_H