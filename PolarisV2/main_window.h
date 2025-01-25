#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_window.h"
#include "about_window.h"
#include "world_inspector.h"
#include "watermark.h"
#include "creative_menu.h"
#include "category.h"

#include <vector>

namespace polaris::ui::window::windows
{
    //!  The main menu bar.
    /*!
     * In charge of giving users access to the Polaris tools.
     */
    class MainWindow : public UIWindow
    {
    public:
        std::vector<mainwindow::Category*> m_vCategories;
        AboutWindow* m_pAboutWindow;
        Watermark* m_pWatermark;

        MainWindow();

        void Draw() override;
        void Update() override;
    };
}

extern polaris::ui::window::windows::MainWindow* gpMainWindow;

#endif // !MAIN_WINDOW_H
