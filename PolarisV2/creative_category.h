#ifndef CREATIVE_CATEGORY_H
#define CREATIVE_CATEGORY_H

#include "category.h"
#include "creative_menu.h"
#include "creative_plate.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    class CreativeCategory : public Category
    {
    public:
        CreativeMenu* m_pCreativeMenu;

        CreativeCategory(tables::plates::CreativePlate* creativePlate);

        void DrawButtons() override;
    };
}

#endif // !CREATIVE_CATEGORY_H
