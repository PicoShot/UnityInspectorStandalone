#pragma once
#include "pch.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
{
	void OnPresent();
	void OnResize();
	LRESULT MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void BuildFrame();
	std::recursive_timed_mutex& FrameMutex();

	void RenderToExternalOverlay(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView,
	                             HWND hwnd);

	void UpdateExternalInput();
	void ApplyTheme();
	inline std::atomic_bool g_ImGuiInitialized = false;
	inline std::atomic_bool g_MainThreadUi = false;
}
