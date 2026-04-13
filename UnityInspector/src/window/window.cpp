#include "pch.h"
#include "window.h"
#include "features/features.h"
#include "hooks/hooks.h"
#include "menu/menu.h"
#include "input_forwarder.h"
#include "external_overlay.h"
#include "input.h"

namespace Window
{
	void SetMenuStyle() {
		auto& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.WindowRounding = 4;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.WindowTitleAlign = ImVec2(0.5, 0.5);
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
		SetMenuStyle();

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

	void RenderToExternalOverlay(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView, const HWND hwnd)
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

			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureMouseUnlessPopupClose)
			{
				switch (uMsg) {
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

		if (Input::ProcessMessage(uMsg, wParam))
			return 2;

		return 1;
	}
}