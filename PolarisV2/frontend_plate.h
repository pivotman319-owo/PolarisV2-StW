#ifndef FRONTEND_PLATE_H
#define FRONTEND_PLATE_H

#include "peh_plate.h"

namespace polaris::tables::plates
{
    //!  Manages Frontend's custom behavior.
    /*!
     * In charge of managing basic interaction like entering a match.
     */
    class FrontendPlate : public PehPlate
    {
    public:
        bool m_bCreativeMode;

        void OnEnabled() override;

        void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams) override;
    };
}

#endif // !FRONTEND_PLATE_H
