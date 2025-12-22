#pragma once
#include "pch.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Window 
{
    static void OnPresent();
    static LRESULT MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
    inline static bool g_ImGuiInitialized = false;
    static void SetMenuStyle();
};