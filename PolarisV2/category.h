#ifndef CATEGORY_H
#define CATEGORY_H

#include <vector>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

namespace polaris::ui::window::windows::mainwindow
{
    class Category
    {
    public:
        std::vector<Category*> m_vCategories;
        const char* m_cCategoryName;

        virtual void DrawButtons();
        void Draw();
    };
}

#endif // !SUB_WINDOW_H
