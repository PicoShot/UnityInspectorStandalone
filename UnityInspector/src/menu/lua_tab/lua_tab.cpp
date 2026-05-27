#include "pch.h"
#include "lua_tab.h"
#include "features/lua_system/lua_system.h"
#include "features/lua_system/lua_plugin.h"

void LuaConsoleTab::Render()
{
	ImGui::BeginChild("LuaTab", ImVec2(0, 0), true);

	ImGui::Text("Lua Plugins");
	ImGui::Separator();
	ImGui::Spacing();

	if (auto* luaSystem = LuaSystem::GetInstance())
	{
		bool consoleVisible = luaSystem->IsConsoleVisible();
		if (ImGui::Checkbox("Show Lua Console", &consoleVisible))
			luaSystem->SetConsoleVisible(consoleVisible);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button("Reload All Plugins", ImVec2(150, 0)))
			luaSystem->ReloadAll();

		ImGui::Spacing();
		ImGui::Text("Loaded Plugins (%zu):", luaSystem->GetPlugins().size());
		ImGui::Spacing();

		for (size_t i = 0; i < luaSystem->GetPlugins().size(); ++i)
		{
			auto& plugin = luaSystem->GetPlugins()[i];
			ImGui::PushID(static_cast<int>(i));

			bool enabled = plugin->IsEnabled();
			if (ImGui::Checkbox(plugin->GetName().c_str(), &enabled))
				plugin->SetEnabled(enabled);

			if (plugin->HasError())
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ERROR]");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("%s", plugin->GetError().c_str());
			}

			ImGui::SameLine();
			if (plugin->IsLoaded())
			{
				if (ImGui::SmallButton("Unload"))
					plugin->Unload();
			}
			else
			{
				if (ImGui::SmallButton("Reload"))
					plugin->Reload();
			}

			ImGui::PopID();
		}
	}
	else
	{
		ImGui::TextDisabled("LuaSystem not initialized");
	}

	ImGui::EndChild();
}
