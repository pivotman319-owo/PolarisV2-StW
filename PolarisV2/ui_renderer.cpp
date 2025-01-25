#include "ui_renderer.h"
#include "error_utils.h"
#include "sdk_utils.h"
#include "directory_utils.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <MinHook.h>
#pragma comment(lib, "libMinHook.x64.lib")

polaris::ui::UIRenderer* gpRenderer;

static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_KEYUP && (wParam == VK_HOME || (wParam == VK_ESCAPE && gpRenderer->m_bLockFortInput)))
	{
		gpRenderer->m_bLockFortInput = !gpRenderer->m_bLockFortInput;
		ImGui::GetIO().MouseDrawCursor = gpRenderer->m_bLockFortInput;
	}

	if (gpRenderer->m_bLockFortInput)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

		return TRUE;
	}

	return CallWindowProc(gpRenderer->m_lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

// Rendering hook used to draw on top of the Fortnite window.
HRESULT(*Present)(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags) = nullptr;
__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags)
{
	static float fWidth = 0;
	static float fHeight = 0;

	// Poll and handle messages (inputs, window resize, etc)
	static MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	if (::PeekMessage(&msg, gpRenderer->m_hWnd, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (!gpRenderer->m_pCurrentDevice)
	{
		pInstance->GetDevice(__uuidof(gpRenderer->m_pCurrentDevice), reinterpret_cast<PVOID*>(&gpRenderer->m_pCurrentDevice));
		gpRenderer->m_pCurrentDevice->GetImmediateContext(&gpRenderer->m_pCurrentContext);

		ID3D11Texture2D* pTarget = nullptr;
		pInstance->GetBuffer(0, __uuidof(pTarget), reinterpret_cast<PVOID*>(&pTarget));

		gpRenderer->m_pCurrentDevice->CreateRenderTargetView(pTarget, nullptr, &gpRenderer->m_pCurrentView);

		pTarget->Release();

		ID3D11Texture2D* pBuffer = nullptr;
		pInstance->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&pBuffer));

		D3D11_TEXTURE2D_DESC desc = { };
		pBuffer->GetDesc(&desc);

		if (!fWidth)
			gpRenderer->m_lpPrevWndFunc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(gpRenderer->m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));

		fWidth = static_cast<float>(desc.Width);
		fHeight = static_cast<float>(desc.Height);

		pBuffer->Release();

		// Handle application-specific workload.
		ImGui_ImplWin32_Init(gpRenderer->m_hWnd);
		ImGui_ImplDX11_Init(gpRenderer->m_pCurrentDevice, gpRenderer->m_pCurrentContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (!gpRenderer->m_bLockFortInput)
		ImGui::SetNextWindowBgAlpha(0.5f);

	for (auto window = gpRenderer->m_vpWindows.begin(); window != gpRenderer->m_vpWindows.end(); ++window)
	{
		(*window)->Update();

		if((*window)->m_bIsOpen == true)
			(*window)->Draw();
	}

	ImGui::SetNextWindowBgAlpha(1.0f);

	ImGui::Render();
	gpRenderer->m_pCurrentContext->OMSetRenderTargets(1, &gpRenderer->m_pCurrentView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return Present(pInstance, SyncInterval, Flags);
}

// Resize hook.
HRESULT(*ResizeBuffers)(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = nullptr;
__declspec(dllexport) HRESULT ResizeBuffersHook(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	ImGui_ImplDX11_Shutdown();

	for (auto window = gpRenderer->m_vpWindows.begin(); window != gpRenderer->m_vpWindows.end(); ++window)
	{
		(*window)->Resize();
	}

	gpRenderer->m_pCurrentView->Release();
	gpRenderer->m_pCurrentContext->Release();
	gpRenderer->m_pCurrentDevice->Release();

	gpRenderer->m_pCurrentDevice = nullptr;

	return ResizeBuffers(pInstance, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

namespace polaris::ui
{
	UIRenderer::UIRenderer()
	{
		if (gpRenderer != nullptr)
			utilities::ErrorUtils::ThrowException(L"gpRenderer is not null.");

		gpRenderer = this;

		IDXGISwapChain* pSwapChain;
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;
		DXGI_SWAP_CHAIN_DESC desc = { };
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		m_hWnd = FindWindow(L"UnrealWindow", L"Fortnite ");
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;
		desc.BufferCount = 1;
		desc.OutputWindow = m_hWnd;
		desc.Windowed = true;

		// Initialize and check if we failed to initialize our DirectX 11 device.
		if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &desc, &pSwapChain, &pDevice, nullptr, &pContext)))
			utilities::ErrorUtils::ThrowException(L"Failed to create DX11 device.");

		auto pTable = *reinterpret_cast<PVOID**>(pSwapChain);

		auto pPresent = pTable[8];
		auto pResizeBuffers = pTable[13];

		// Prepare ImGui.
		ImGui::CreateContext();
		SetTheme();

		MH_CreateHook(pPresent, PresentHook, reinterpret_cast<PVOID*>(&Present));
		MH_EnableHook(pPresent);

		MH_CreateHook(pResizeBuffers, ResizeBuffersHook, reinterpret_cast<PVOID*>(&ResizeBuffers));
		MH_EnableHook(pResizeBuffers);

		pSwapChain->Release();
		pDevice->Release();
		pContext->Release();
	}

	void UIRenderer::SetTheme()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGuiStyle* style = &ImGui::GetStyle();

		// sugoi ui font
		io.Fonts->AddFontFromFileTTF(utilities::DirectoryUtils::GetConcatPath(utilities::DirectoryUtils::GetFontsDir(), "segoeui.ttf").c_str(), 20);
		io.Fonts->AddFontFromFileTTF(utilities::DirectoryUtils::GetConcatPath(utilities::DirectoryUtils::GetFontsDir(), "segoeui.ttf").c_str(), 15);

		//Headers
		io.Fonts->AddFontFromFileTTF(utilities::DirectoryUtils::GetConcatPath(utilities::DirectoryUtils::GetFontsDir(), "segoeui.ttf").c_str(), 35);
		io.Fonts->AddFontFromFileTTF(utilities::DirectoryUtils::GetConcatPath(utilities::DirectoryUtils::GetFontsDir(), "segoeuib.ttf").c_str(), 25);

		// Bold font
		auto font = io.Fonts->AddFontFromFileTTF(utilities::DirectoryUtils::GetConcatPath(utilities::DirectoryUtils::GetFontsDir(), "segoeuib.ttf").c_str(), 20);

		constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
		{
			return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
		};

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	}
}