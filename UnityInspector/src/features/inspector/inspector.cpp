#include "pch.h"
#include "inspector.h"

REGISTER_FEATURE(Inspector)

void Inspector::Update(const float deltaTime)
{
	const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer, ShowDebugConsole] = Config::settings.inspector;
	if (!Enabled || !Config::state.showMenu) return;

	UR::ThreadAttach();

	static float timer = 0.0f;
	timer += deltaTime;

	if (timer >= 1.f)
	{
		timer -= 1.f;

		if (AutoRefresh)
			RefreshHierarchy();

		if (AutoUpdateObject && activeTabIndex >= 0 && std::cmp_less(activeTabIndex, openTabs.size()))
		{
			RefreshTabData(openTabs[activeTabIndex]);
		}
	}
}

void Inspector::Render()
{
	if (!Config::settings.inspector.enabled || !Config::state.showMenu) return;

	UR::ThreadAttach();

	ImGui::SetNextWindowSize(ImVec2(380, 600), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Hierarchy", nullptr))
	{
		if (ImGui::Button("Refresh"))
			RefreshHierarchy();

		ImGui::SameLine();
		ImGui::Checkbox("Auto", &Config::settings.inspector.autoRefresh);

		ImGui::SameLine();
		if (ImGui::SmallButton("Collapse"))
			SetAllNodesExpanded(rootNodes, false);

		ImGui::SameLine();
		if (ImGui::SmallButton("Expand"))
			SetAllNodesExpanded(rootNodes, true);

		ImGui::SameLine();
		ImGui::TextDisabled("| %zu", rootNodes.size());

		ImGui::PushItemWidth(-1);
		ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, sizeof(searchBuffer),
			ImGuiInputTextFlags_EscapeClearsAll);
		ImGui::PopItemWidth();

		ImGui::Separator();

		if (ImGui::BeginChild("HierarchyTree", ImVec2(0, 0), false))
		{
			if (rootNodes.empty())
			{
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::NewLine();
				float cx = (ImGui::GetContentRegionAvail().x - 180.0f) * 0.5f;
				if (cx > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);
				ImGui::TextDisabled("No scene hierarchy loaded");
				ImGui::Spacing();
				if (cx > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);
				if (ImGui::Button("Load Scene Hierarchy", ImVec2(180, 0)))
					RefreshHierarchy();
			}
			else
			{
				for (auto& node : rootNodes)
				{
					RenderHierarchyNode(node);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();

	RenderDetailsWindow();
	DrawSelectedObjectBoundingBox();
}
