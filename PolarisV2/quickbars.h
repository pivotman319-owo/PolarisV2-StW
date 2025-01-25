#ifndef QUICKBARS_H
#define QUICKBARS_H

#include "peh_plate.h"
#include "sdk_utils.h"
#include "athena_pawn.h"

namespace polaris::inventory
{
    class quickbars
    {
    private:
        int foo = 0;
    public:
        SDK::FGuid* m_pgPickaxe;
        SDK::FGuid* m_pgGoingCommando;
        SDK::FGuid* m_pgSentryDef;
        SDK::FGuid* m_pgWallBuild;
        SDK::FGuid* m_pgFloorBuild;
        SDK::FGuid* m_pgStairBuild;
        SDK::FGuid* m_pgRoofBuild;

        void SetupQuickbars(); //!< SetupQuickbars initializes the quickbars.
    };
}

#endif // !QUICKBARS_H
