#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "ui_window.h"

#include <vector>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

namespace polaris::ui
{
    //!  Renders D3D11 on top of the Fortnite window.
    /*!
     * In charge of rendering ImGui windows on top of the Fortnite window using D3D11.
    */
    class UIRenderer
    {
    public:
        // D3D11 Devices
        ID3D11Device* m_pCurrentDevice = nullptr;
        ID3D11DeviceContext* m_pCurrentContext = nullptr;
        ID3D11RenderTargetView* m_pCurrentView = nullptr;

        // Fortnite Window
        HWND m_hWnd = 0;
        WNDPROC m_lpPrevWndFunc;

        // ImGui Window Properties
        std::vector<window::UIWindow*> m_vpWindows;
        bool m_bLockFortInput;

        UIRenderer();

        void SetTheme(); //!< Applies the custom Polaris theme to ImGui.
    };
}

#endif // !UI_RENDERER_H

extern polaris::ui::UIRenderer* gpRenderer;