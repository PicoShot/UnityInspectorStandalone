#include "pch.h"
#include "window.h"
#include "features/features.h"
#include "menu/menu.h"
#include "input.h"
#include "config/config.h"
#include "themes.h"

namespace Window
{
	void ApplyTheme()
	{
		switch (Config::settings.theme)
		{
		case Theme::Light:
			ImGui::StyleColorsLight();
			break;
		case Theme::Dark:
			ImGui::StyleColorsDark();
			break;
		case Theme::Classic:
			ImGui::StyleColorsClassic();
			break;
		case Theme::WhitePlus:
			ImGui::StyleColorsLight();
			Themes::SetWhitePlusTheme();
			break;
		case Theme::DarkPlus:
		default:
			ImGui::StyleColorsDark();
			Themes::SetDarkPlusTheme();
			break;
		}
	}

	void InitializeImGui(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigWindowsResizeFromEdges = false;
		const auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
		io.FontDefault = font;
		io.IniFilename = nullptr;
		io.IniSavingRate = 0.f;
		io.LogFilename = nullptr;
		io.MouseDrawCursor = true;
		ApplyTheme();
		Themes::SetImGuiStyle();

		if (ImGui_ImplWin32_Init(hwnd) && ImGui_ImplDX11_Init(device, context))
		{
			UR::ThreadAttach();
			g_ImGuiInitialized = true;
		}
	}

	void ShutdownImGui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		g_ImGuiInitialized = false;
	}

	void RenderFrame(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView)
	{
		if (!context) return;
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Features::Update(ImGui::GetIO().DeltaTime);
		Menu::Render();
		Features::Render();

		ImGui::Render();

		context->OMSetRenderTargets(1, &targetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void OnPresent()
	{
		const auto device = dx_hook::Hk11::GetDevice();
		const auto context = dx_hook::Hk11::GetContext();
		const auto targetView = dx_hook::Hk11::GetTargetView();

		if (!device || !context || !*targetView) return;

		if (!g_ImGuiInitialized) InitializeImGui(dx_hook::Hk11::GetHwnd(), device, context);

		RenderFrame(context, *targetView);
	}

	void RenderToExternalOverlay(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView,
	                             const HWND hwnd)
	{
		if (!g_ImGuiInitialized) InitializeImGui(hwnd, device, context);

		RenderFrame(context, targetView);
	}

	void UpdateExternalInput()
	{
		if (!g_ImGuiInitialized) return;

		Input::ProcessExternal();
	}

	LRESULT MyWndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		if (g_ImGuiInitialized)
		{
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

			if (Input::ProcessMessage(uMsg, wParam))
				return 2;

			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureMouseUnlessPopupClose)
			{
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP:
				case WM_MOUSEWHEEL:
				case WM_MOUSEMOVE:
				case WM_MOUSELEAVE:
				case WM_MOUSEACTIVATE:
				case WM_MOUSEHOVER:
				case WM_MOUSELAST:
				case WM_NCMOUSEHOVER:
				case WM_NCMOUSELEAVE:
				case WM_NCMOUSEMOVE:
					return 2;
				}
			}
		}
		return 1;
	}
}
