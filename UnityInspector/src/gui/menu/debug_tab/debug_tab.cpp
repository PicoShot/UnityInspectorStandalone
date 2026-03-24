#include "pch.h"
#include "debug_tab.h"
#include "core/core.h"

void DebugTab::Render()
{
	ImGui::BeginChild("DebugTab", ImVec2(0, 0), true);

	ImGui::Text(X("Debug Tools"));
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox(X("Enable Inspector"), &Core::config->inspector.enabled);

	ImGui::Checkbox(X("Show Assembly Explorer"), &Core::config->inspector.showAssemblyExplorer);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open a window to browse all loaded assemblies and their classes");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text(X("Debug Console"));
	ImGui::Checkbox(X("Show Debug Console"), &Core::config->inspector.showDebugConsole);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::EndChild();
}
