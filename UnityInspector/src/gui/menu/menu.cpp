#include "pch.h"
#include "menu.h"
#include "info_tab/info_tab.h"
#include "debug_tab/debug_tab.h"
#include "core/core.h"

void Menu::Init()
{
	if (s_Initialized) return;
	s_Tabs.push_back(std::make_unique<InfoTab>());
	s_Tabs.push_back(std::make_unique<DebugTab>());
	s_Initialized = true;
}

void Menu::Render()
{
	if (!s_Initialized) Init();

	if (Core::config->internal.showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(600, 400), ImGuiCond_FirstUseEver);

#ifdef _DEBUG
		const char* menuTitle = X("UnityInspector (Debug)");
#else
		const char* menuTitle = X("UnityInspector (Release)");
#endif

		ImGui::Begin(menuTitle, &Core::config->internal.showImGui, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);

		if (ImGui::BeginTabBar(X("MainTabs"), ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
		{
			for (const auto& tab : s_Tabs)
			{
				if (ImGui::BeginTabItem(tab->GetName().c_str()))
				{
					tab->Render();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}