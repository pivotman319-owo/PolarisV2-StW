#include "frontend_plate.h"
#include "globals.h"
#include "program.h"
#include "athena_plate.h"
#include "farmstead_plate.h"
#include "creative_plate.h"
#include "main_window.h"
#include "creative_category.h"

#include <iostream>

namespace polaris::tables::plates
{
    void FrontendPlate::OnEnabled()
    {
        std::cout << "We're now in the Frontend State." << std::endl;

        if (globals::gpLevel->URL.Map.ToString() != "/Game/Maps/FortniteEntry")
            globals::gpPlayerController->SwitchLevel(TEXT("Frontend"));
    }

    void FrontendPlate::ProcessEventHook(SDK::UObject* pObject, SDK::UFunction* pFunction, PVOID pParams)
    {
        if (pFunction->GetName().find("BP_PlayButton") != std::string::npos)
        {
            if (m_bCreativeMode == true)
            {
                CreativePlate* creativePlate = new CreativePlate;
                auto shit = new ui::window::windows::mainwindow::category::categories::CreativeCategory(creativePlate);

                gpMainWindow->m_vCategories.push_back(shit);
                gpProgram->m_pMainTable->PushPlate(creativePlate);
            }
            else
                gpProgram->m_pMainTable->PushPlate(new AthenaPlate);
        }
        else if (pFunction->GetName().find("BndEvt__CampaignBtn_K2Node_ComponentBoundEvent_107_CommonButtonClicked__DelegateSignature") != std::string::npos)
        {
            gpProgram->m_pMainTable->PushPlate(new FarmsteadPlate);
        }
    }
}