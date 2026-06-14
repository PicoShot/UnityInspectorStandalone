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
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a hierarchy window to browse all objects in scene");

	ImGui::Checkbox("Show Assembly Explorer", &Config::settings.inspector.showAssemblyExplorer);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a assembly explorer window to browse all loaded assemblies");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Debug Console");
	ImGui::Checkbox("Show Debug Console", &Config::settings.inspector.showDebugConsole);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a debug console window to view all game logs");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Memory Scanner");
	ImGui::Checkbox("Show Memory Scanner", &Config::settings.memoryScanner.showWindow);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a memory scanner window to scan specific values");

	ImGui::EndChild();
}
