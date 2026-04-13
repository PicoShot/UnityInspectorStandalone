#include "pch.h"
#include "info_tab.h"

void InfoTab::Render()
{
	ImGui::BeginChild("InfoTab", ImVec2(0, 0), true);

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

	ImGui::EndChild();
}
