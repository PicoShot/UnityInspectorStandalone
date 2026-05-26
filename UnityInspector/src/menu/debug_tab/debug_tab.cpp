#include "pch.h"
#include "debug_tab.h"
#include "config/config.h"

void DebugTab::Render()
{
	ImGui::BeginChild("DebugTab", ImVec2(0, 0), true);

	ImGui::Text("Debug Tools");
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Enable Inspector", &Config::settings.inspector.enabled);

	ImGui::Checkbox("Show Assembly Explorer", &Config::settings.inspector.showAssemblyExplorer);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a window to browse all loaded assemblies and their classes");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Debug Console");
	ImGui::Checkbox("Show Debug Console", &Config::settings.inspector.showDebugConsole);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Memory Scanner");
	ImGui::Checkbox("Show Memory Scanner", &Config::settings.memoryScanner.showWindow);
	ImGui::Text("(not usable, under development)");

	ImGui::EndChild();
}
