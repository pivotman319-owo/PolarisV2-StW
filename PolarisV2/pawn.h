#ifndef PAWN_H
#define PAWN_H

#include "SDK.hpp"

#include <windows.h>

namespace polaris
{
    namespace pawn
    {
        //!  Pawn class for a PehPlate.
        /*!
        * Used to separate Pawn logic from the a PehPlate's update loops. Call a pawn's Update and ProcessEventHook functions in a PehPlate.
        */
        class Pawn
        {
        public:
            SDK::AFortPlayerPawn* m_pPawnActor = nullptr; //!< The Pawn's actor.
            bool m_bHasJumped = false; //!< True when space is being held down and the pawn has already finished a jump.

            virtual void ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams); //!< ProcessEventHook is used to hook behavior to ProcessEvent calls.
            virtual void Update(); //!< Update should be called in a PehPlate's Update function.
        };
    }
}

#endif // !PAWN_H
