#pragma once
#include "pch.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Window
{
    static void OnPresent();
    static LRESULT MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static void RenderToExternalOverlay(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView, HWND hwnd);

    [[nodiscard]] static bool IsImGuiInitialized() noexcept { return g_ImGuiInitialized; }
    static void SetImGuiInitialized(const bool value) noexcept { g_ImGuiInitialized = value; }

    static void UpdateExternalInput();

private:
    inline static bool g_ImGuiInitialized = false;
    static void SetMenuStyle();
    static void InitializeImGui(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    static void ShutdownImGui();
    static void RenderFrame(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView);
};