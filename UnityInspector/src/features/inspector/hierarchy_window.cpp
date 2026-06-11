#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

static void AppendNodeTree(const HierarchyNode& node, std::string& out, int depth)
{
	for (int i = 0; i < depth; i++)
		out += (i == depth - 1) ? "  " : "| ";

	bool isActive = true;
	Helper::SafeGetActiveSelf(node.gameObject, isActive);

	out += "+-- [" + std::to_string(depth) + "] " + node.name;
	if (!isActive) out += " (inactive)";
	out += "\n";

	int idx = 0;
	for (const auto& child : node.children)
	{
		for (int i = 0; i < depth; i++)
			out += "| ";
		out += "|   [" + std::to_string(idx) + "]\n";
		AppendNodeTree(child, out, depth + 1);
		idx++;
	}
}

bool Inspector::NodeMatchesSearch(const HierarchyNode& node, std::string_view lowerSearch) const
{
	if (lowerSearch.empty()) return true;
	if (Helper::CaseInsensitiveFind(node.name, lowerSearch)) return true;
	for (const auto& child : node.children)
	{
		if (NodeMatchesSearch(child, lowerSearch)) return true;
	}
	return false;
}

void Inspector::SetAllNodesExpanded(std::vector<HierarchyNode>& nodes, bool expanded)
{
	for (auto& node : nodes)
	{
		node.pendingExpand = true;
		node.pendingExpandValue = expanded;
		SetAllNodesExpanded(node.children, expanded);
	}
}

void Inspector::RenderHierarchyNode(HierarchyNode& node, std::string_view lowerSearch, const int depth)
{
	if (!Helper::SafeIsAlive(node.gameObject)) return;

	const bool searching = !lowerSearch.empty();

	if (searching && !NodeMatchesSearch(node, lowerSearch))
		return;

	if (searching)
	{
		ImGui::SetNextItemOpen(true);
	}
	else if (node.pendingExpand)
	{
		ImGui::SetNextItemOpen(node.pendingExpandValue);
		node.pendingExpand = false;
	}

	ImGui::PushID(node.gameObject);

	const bool hasChildren = !node.children.empty();
	const bool isSelected = (FindTabForObject(node.gameObject) >= 0);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;

	if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

	bool isActive = true;
	if (!Helper::SafeGetActiveSelf(node.gameObject, isActive))
	{
		ImGui::PopID();
		return;
	}

	if (!isActive)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

	std::string label = node.name;
	if (hasChildren)
		label += " [" + std::to_string(node.children.size()) + "]";

	const bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);

	if (!isActive)
		ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("%s", node.name.c_str());
		if (!isActive) ImGui::TextDisabled("Inactive");
		if (hasChildren) ImGui::TextDisabled("%zu children", node.children.size());
		if (node.transform)
		{
			std::string path = BuildObjectPath(node.transform);
			ImGui::TextDisabled("%s", path.c_str());
		}
		ImGui::EndTooltip();
	}

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		OpenObjectInNewTab(node.gameObject);

	if (ImGui::BeginPopupContextItem("##NodeCtx"))
	{
		if (ImGui::MenuItem("Inspect"))
			OpenObjectInNewTab(node.gameObject);

		ImGui::Separator();

		if (ImGui::MenuItem(isActive ? "Deactivate" : "Activate"))
			Helper::SafeSetActive(node.gameObject, !isActive);

		ImGui::Separator();

		if (ImGui::MenuItem("Copy Name"))
			ImGui::SetClipboardText(node.name.c_str());

		if (node.transform)
		{
			if (ImGui::MenuItem("Copy Path"))
			{
				const std::string path = BuildObjectPath(node.transform);
				ImGui::SetClipboardText(path.c_str());
			}
		}

		if (ImGui::MenuItem("Copy Hierarchy"))
		{
			std::string tree = node.name + "\n";
			int idx = 0;
			for (const auto& child : node.children)
			{
				tree += "  [" + std::to_string(idx) + "]\n";
				AppendNodeTree(child, tree, 1);
				idx++;
			}
			ImGui::SetClipboardText(tree.c_str());
		}

		if (hasChildren)
		{
			ImGui::Separator();
			if (ImGui::MenuItem("Expand Children"))
				SetAllNodesExpanded(node.children, true);
			if (ImGui::MenuItem("Collapse Children"))
				SetAllNodesExpanded(node.children, false);
		}

		ImGui::EndPopup();
	}

	if (hasChildren && nodeOpen)
	{
		for (auto& child : node.children)
			RenderHierarchyNode(child, lowerSearch, depth + 1);
		ImGui::TreePop();
	}

	ImGui::PopID();
}
