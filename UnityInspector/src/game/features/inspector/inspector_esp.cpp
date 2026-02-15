#include "pch.h"
#include "inspector.h"

void Inspector::DrawSelectedObjectBoundingBox() const
{
	UR::ThreadAttach();

	if (activeTabIndex < 0 || std::cmp_greater_equal(activeTabIndex, openTabs.size())) return;

	const auto& tab = openTabs[activeTabIndex];
	if (!tab.gameObject || !Core::helper->SafeIsAlive(tab.gameObject)) return;

	UT::Transform* transform = nullptr;
	if (!Core::helper->SafeGetTransform(tab.gameObject, transform) || !transform) return;

	Vec3 position;
	if (!Core::helper->TryGetPosition(transform, position)) return;

	Vec2 screenPos;
	if (!Core::helper->WorldToScreen(position, screenPos)) return;

	const auto& drawList = ImGui::GetBackgroundDrawList();
	const ImU32 redColor = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	constexpr float radius = 10.0f;

	drawList->AddCircle(ImVec2(screenPos.x, screenPos.y), radius, redColor, 0, 2.0f);
}
