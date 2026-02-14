#include "pch.h"
#include "window.h"
#include "game/core/core.h"
#include "game/hooks/hooks.h"
#include "gui/menu/menu.h"
#include "gui/fonts/Roboto_font.h"

void Window::SetMenuStyle() {
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

void Window::OnPresent()
{
    auto device = dx_hook::Hk11::GetDevice();
    auto context = dx_hook::Hk11::GetContext();
    auto targetView = dx_hook::Hk11::GetTargetView();

    if (!device || !context || !*targetView) return;

    if (!g_ImGuiInitialized) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.ConfigWindowsResizeFromEdges = false;
        auto font = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_font_compressed_data, Roboto_font_compressed_size, 16.0f, nullptr);
        io.FontDefault = font;
        io.IniFilename = nullptr;
        io.IniSavingRate = 0.f;
        io.LogFilename = nullptr;
        io.MouseDrawCursor = true;
        SetMenuStyle();

        if (ImGui_ImplWin32_Init(dx_hook::Hk11::GetHwnd()) && ImGui_ImplDX11_Init(device, context))
        {
            Hooks::Init();
            UR::ThreadAttach();
            g_ImGuiInitialized = true;
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (const auto& feature : Core::features)
    {
        feature->Update(ImGui::GetIO().DeltaTime);
    }
   
    Menu::RenderMenu();

    for (const auto& feature : Core::features)
    {
        feature->Render();
    }

    ImGui::Render();

    context->OMSetRenderTargets(1, targetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT Window::MyWndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    if (g_ImGuiInitialized) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureMouseUnlessPopupClose) {
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

    if (uMsg == WM_KEYDOWN) 
    {
        switch (wParam)
        {
		case VK_INSERT:
        {
            auto& menu = Core::config->ShowImGui;
            menu = !menu;
            ImGui::GetIO().MouseDrawCursor = menu;
            ClipCursor(nullptr);
            return 2;
        }
        case VK_F5:
        {
			// toggles cursor visibility
			static bool cursor = false;
			cursor = !cursor;
            cursor ? ShowCursor(TRUE) : ShowCursor(FALSE);
            return 2;
        }
        case VK_F6:
        {
			// unlocks cursor
            ClipCursor(nullptr);
            return 2;
        }
        default:
            break;
        }
    }

    return 1;
}