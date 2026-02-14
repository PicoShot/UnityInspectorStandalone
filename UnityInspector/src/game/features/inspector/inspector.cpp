#include "pch.h"
#include "inspector.h"

Inspector::Inspector() = default;
Inspector::~Inspector() = default;

void Inspector::Update(const float deltaTime)
{
	const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer, ShowDebugConsole] = Core::config->inspector;
	if (!Enabled || !Core::config->ShowImGui) return;

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
	if (!Core::config->inspector.Enabled || !Core::config->ShowImGui) return;

	UR::ThreadAttach();

	ImGui::SetNextWindowSize(ImVec2(380, 600), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Hierarchy", nullptr))
	{
		if (ImGui::Button("Refresh"))
		{
			RefreshHierarchy();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto", &Core::config->inspector.AutoRefresh);

		ImGui::SameLine();
		ImGui::TextDisabled("| %zu objects", rootNodes.size());

		ImGui::SameLine();
		ImGui::SetNextItemWidth(-1);
		ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, sizeof(searchBuffer));

		ImGui::Separator();

		if (ImGui::BeginChild("HierarchyTree", ImVec2(0, 0), false))
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
