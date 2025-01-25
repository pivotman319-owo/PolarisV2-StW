#include "pawn.h"

namespace polaris
{
    namespace pawn
    {
        void Pawn::ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
        {
        }
        void Pawn::Update()
        {
            if (m_pPawnActor == nullptr)
            {
                delete this;
                return;
            }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                if (m_bHasJumped == false)
                {
                    m_bHasJumped = true;
                    if (!m_pPawnActor->IsJumpProvidingForce()) {
                        SDK::FVector vec;
                        vec.X = 0.0f;
                        vec.Y = 0.0f;
                        vec.Z = 5000.0f;
                        m_pPawnActor->Jump();
                        //m_pPawnActor->LaunchCharacterJump(vec, false, false, true, true);
                    }
                }
            }
            else
                m_bHasJumped = false;
        }
    }
}