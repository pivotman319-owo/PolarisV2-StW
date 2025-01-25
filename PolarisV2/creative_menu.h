#ifndef CREATIVE_MENU
#define CREATIVE_MENU

#include "ui_window.h"
#include "creative_plate.h"

#include <string>
#include <filesystem>

namespace polaris::ui::window::windows
{
    class CreativeMenu : public UIWindow
    {
    private:
        tables::plates::CreativePlate* m_pCreativePlate;
    public:
        bool m_bCreatingNewProject;

        CreativeMenu(tables::plates::CreativePlate* creativePlate);

        void Draw() override;

        void CreateNew();
    };
}

#endif // !CREATIVE_MENU
