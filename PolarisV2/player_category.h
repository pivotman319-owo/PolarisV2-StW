#ifndef PLAYER_CATEGORY_H
#define PLAYER_CATEGORY_H

#include "category.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    class PlayerCategory : public Category
    {
    public:
        PlayerCategory();

        void DrawButtons() override;
    };
}

#endif // !PLAYER_CATEGORY_H
