#include "pch.h"
#include "menu.h"
#include "game/core/core.h"


void Menu::RenderVisualTab() {
    ImGui::BeginChild(X("VisualTab"), ImVec2(0, 0), true);

    ImGui::Text(X("Visual Enhancements"));
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader(X("ESP")))
    {
       
    }

    if (ImGui::CollapsingHeader(X("Field of View")))
    {
        
    }

    if (ImGui::CollapsingHeader(X("Other")))
    {
       
    }

    ImGui::EndChild();
}

void Menu::RenderAimTab() {
    ImGui::BeginChild(X("AimTab"), ImVec2(0, 0), true);

    ImGui::Text(X("Aim Assistance"));
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::EndChild();
}

void Menu::RenderMiscTab() {
    ImGui::BeginChild(X("MiscTab"), ImVec2(0, 0), true);

    ImGui::Text(X("Miscellaneous"));
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader(X("Movement")))
    {
       
    }

    if (ImGui::CollapsingHeader(X("Network")))
    {
        
    }

    if (ImGui::CollapsingHeader(X("Il2Cpp GC")))
    {
        
    }

    ImGui::EndChild();
}

void Menu::RenderInfoTab() {
    ImGui::BeginChild(X("InfoTab"), ImVec2(0, 0), true);

    ImGui::Text(X("Game Information"));
    ImGui::Separator();
    ImGui::Spacing();

    auto io = ImGui::GetIO();

    ImGui::Text(X("FPS: %.1f"), io.Framerate);
    ImGui::Spacing();
    ImGui::Text(X("DeltaTime: %.3f"), io.DeltaTime);
    ImGui::Spacing();
    ImGui::Text(X("Display Size: %ix%i"), static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    ImGui::Spacing();
    ImGui::Text(X("MousePos: %ix%i"), static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));

    ImGui::EndChild();
}

void Menu::RenderDebugTab() {
    ImGui::BeginChild("DebugTab", ImVec2(0, 0), true);

    ImGui::Text(X("Debug Tools"));
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox(X("Enable Inspector"), &Core::config->inspector.Enabled);
    
    if (Core::config->inspector.Enabled)
    {
        ImGui::Indent();
        
        ImGui::Checkbox(X("Show Assembly Explorer"), &Core::config->inspector.ShowAssemblyExplorer);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Open a window to browse all loaded assemblies and their classes");
        }
        
        ImGui::Unindent();
    }

    ImGui::EndChild();
}

void Menu::RenderMenu()
{
    if (Core::config->ShowImGui) {

        ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(600, 400), ImGuiCond_FirstUseEver);

#ifdef _DEBUG
        const char* menuTitle = X("Pico.dll (Debug Build)");
#else
        const char* menuTitle = X("Pico.dll (Release Build)");
#endif

        ImGui::Begin(menuTitle, &Core::config->ShowImGui, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);

        if (ImGui::BeginTabBar(X("MainTabs"), ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {

            if (ImGui::BeginTabItem(X("Visual"))) {
                RenderVisualTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(X("Aim"))) {
                RenderAimTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(X("Misc"))) {
                RenderMiscTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(X("Info"))) {
                RenderInfoTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(X("Debug"))) {
                RenderDebugTab();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}