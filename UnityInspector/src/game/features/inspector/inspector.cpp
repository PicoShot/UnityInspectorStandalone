#include "pch.h"
#include "inspector.h"
#include "assembly_explorer.h"

Inspector::Inspector() = default;
Inspector::~Inspector() = default;

void Inspector::Update(const float deltaTime)
{
	const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer] = Core::config->inspector;
	if (!Enabled) return;

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
	
	if (ShowAssemblyExplorer)
	{
		if (!assemblyExplorer)
		{
			assemblyExplorer = std::make_unique<AssemblyExplorer>();
		}
		assemblyExplorer->Update(deltaTime);
	}
}

void Inspector::Render()
{
	if (!Core::config->inspector.Enabled) return;

	UR::ThreadAttach();

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

		ImGui::SameLine();

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

	if (Core::config->inspector.ShowAssemblyExplorer)
	{
		if (!assemblyExplorer)
		{
			assemblyExplorer = std::make_unique<AssemblyExplorer>();
		}
		
		auto* explorer = static_cast<AssemblyExplorer*>(assemblyExplorer.get());
		if (explorer)
		{
			explorer->Render();
		}
	}
	else if (assemblyExplorer)
	{
		assemblyExplorer.reset();
	}
}