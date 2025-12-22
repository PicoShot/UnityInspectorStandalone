#include "pch.h"
#include "inspector.h"

void Inspector::Update(const float deltaTime)
{
	UR::ThreadAttach();
	const auto& [Enabled, AutoUpdateObject, AutoRefresh] = Core::config->inspector;
	if (!Enabled) return;

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
	UR::ThreadAttach();
	if (!Core::config->inspector.Enabled) return;

	ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Hierarchy", nullptr))
	{
		if (ImGui::Button("Refresh"))
		{
			RefreshHierarchy();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto Refresh", &Core::config->inspector.AutoRefresh);

		ImGui::SameLine();
		ImGui::Text("Objects: %zu", rootNodes.size());

		ImGui::SetNextItemWidth(-1);
		ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, sizeof(searchBuffer));

		ImGui::Separator();

		if (ImGui::BeginChild("HierarchyTree", ImVec2(0, 0), true))
		{
			if (rootNodes.empty())
			{
				ImGui::TextDisabled("Click Refresh to load scene hierarchy");
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