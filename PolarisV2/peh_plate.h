#ifndef PEH_PLATE_H
#define PEH_PLATE_H

#include "plate.h"
#include "SDK.hpp"

#include <windows.h>

namespace polaris::tables::plates
{
    //!  The base class for a PehPlate.
    /*!
     * A PehPlate is a type of Plate which can subscribe to a PehTable (ProcessEventHook Table).
     */
    class PehPlate : public Plate
    {
    public:
        virtual void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams); //!< ProcessEventHook is called every time an event is triggered while the plate is primary on the stack.
    };
}

#endif // !PEH_PLATE_H
