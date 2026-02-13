#include "pch.h"
#include "inspector.h"

void Inspector::RenderHierarchyNode(HierarchyNode& node, const int depth)
{
	if (!Core::helper->SafeIsAlive(node.gameObject)) return;

	bool matchesSearch = true;
	if (searchBuffer[0] != '\0')
	{
		matchesSearch = node.name.find(searchBuffer) != std::string::npos;

		if (!matchesSearch)
		{
			for (auto& child : node.children)
			{
				std::function<bool(HierarchyNode&)> checkChildren = [&](HierarchyNode& n) -> bool {
					if (n.name.find(searchBuffer) != std::string::npos) return true;
					for (auto& c : n.children)
					{
						if (checkChildren(c)) return true;
					}
					return false;
				};
				if (checkChildren(child))
				{
					matchesSearch = true;
					break;
				}
			}
		}
	}

	if (!matchesSearch) return;

	ImGui::PushID(node.gameObject);

	const bool hasChildren = !node.children.empty();
	const bool isSelected = (FindTabForObject(node.gameObject) >= 0);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | 
	                           ImGuiTreeNodeFlags_SpanAvailWidth |
	                           ImGuiTreeNodeFlags_FramePadding;
	
	if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

	bool isActive = true;
	if (!Core::helper->SafeGetActiveSelf(node.gameObject, isActive))
	{
		ImGui::PopID();
		return;
	}


	const char* icon = hasChildren ? " " : "  ";
	const char* activeIcon = isActive ? "" : " ";
	
	std::string label = activeIcon + std::string(icon) + node.name;
	
	const bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);


	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		OpenObjectInNewTab(node.gameObject);
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Inspect", nullptr, false, true))
		{
			OpenObjectInNewTab(node.gameObject);
		}
		ImGui::Separator();
		if (ImGui::MenuItem(isActive ? "Deactivate" : "Activate"))
		{
			Core::helper->SafeSetActive(node.gameObject, !isActive);
		}
		ImGui::EndPopup();
	}

	if (hasChildren && nodeOpen)
	{
		for (auto& child : node.children)
		{
			RenderHierarchyNode(child, depth + 1);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}
