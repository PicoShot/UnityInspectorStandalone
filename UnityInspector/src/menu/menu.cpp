#include "pch.h"
#include "menu.h"
#include "misc_tab/misc_tab.h"
#include "debug_tab/debug_tab.h"
#include "lua_tab/lua_tab.h"
#include "config/config.h"

namespace Menu
{
	static std::vector<std::unique_ptr<ITab>> s_Tabs;
	static bool s_Initialized = false;

	void Init()
	{
		if (s_Initialized) return;
		s_Tabs.push_back(std::make_unique<DebugTab>());
		s_Tabs.push_back(std::make_unique<LuaConsoleTab>());
		s_Tabs.push_back(std::make_unique<MiscTab>());
		s_Initialized = true;
	}

	void Render()
	{
		if (!s_Initialized) Init();

		if (Config::state.showMenu)
		{
			ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(600, 400), ImGuiCond_FirstUseEver);

#ifdef _DEBUG
			const auto menuTitle = "UnityInspector (Debug)";
#else
			const auto menuTitle = "UnityInspector (Release)";
#endif

			ImGui::Begin(menuTitle, &Config::state.showMenu,
			             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);

			if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
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
}
