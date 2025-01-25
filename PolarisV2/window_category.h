#ifndef WINDOW_CATEGORY_H
#define WINDOW_CATEGORY_H

#include "category.h"
#include "world_inspector.h"

namespace polaris::ui::window::windows::mainwindow::category::categories
{
    class WindowCategory : public Category
    {
    public:
        WorldInspector* m_pWorldInspector;

        WindowCategory();

        void DrawButtons() override;
    };
}

#endif // !WINDOW_CATEGORY_H
