#include "pch.h"
#include "misc_tab.h"
#include "config/config.h"
#include "window/window.h"

void MiscTab::Render()
{
	ImGui::BeginChild("MiscTab", ImVec2(0, 0), true);

	ImGui::Text("Game Information");
	ImGui::Separator();
	ImGui::Spacing();

	auto io = ImGui::GetIO();

	ImGui::Text("FPS: %.1f", io.Framerate);
	ImGui::Spacing();
	ImGui::Text("DeltaTime: %.3f", io.DeltaTime);
	ImGui::Spacing();
	ImGui::Text("Display Size: %ix%i", static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
	ImGui::Spacing();
	ImGui::Text("MousePos: %ix%i", static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Theme");
	const char* themes[] = {"Light", "Dark", "Classic", "DarkPlus", "WhitePlus"};
	int currentTheme = static_cast<int>(Config::settings.theme);
	if (ImGui::Combo("##Theme", &currentTheme, themes, IM_ARRAYSIZE(themes)))
	{
		Config::settings.theme = static_cast<Theme>(currentTheme);
		Window::ApplyTheme();
	}

	ImGui::EndChild();
}
