#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

void Inspector::DrawSelectedObjectBoundingBox() const
{
	UR::ThreadAttach();

	if (activeTabIndex < 0 || std::cmp_greater_equal(activeTabIndex, openTabs.size())) return;

	const auto& tab = openTabs[activeTabIndex];
	if (!tab.gameObject || !Helper::SafeIsAlive(tab.gameObject)) return;

	UT::Transform* transform = nullptr;
	if (!Helper::SafeGetTransform(tab.gameObject, transform) || !transform) return;

	Vec3 position;
	if (!Helper::TryGetPosition(transform, position)) return;

	Vec2 screenPos;
	if (!Helper::WorldToScreen(position, screenPos)) return;

	const auto& drawList = ImGui::GetBackgroundDrawList();
	const ImU32 redColor = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	constexpr float radius = 10.0f;

	drawList->AddCircle(ImVec2(screenPos.x, screenPos.y), radius, redColor, 0, 2.0f);
}

void Inspector::ProcessObjectPicker()
{
	if (!Config::settings.inspector.objectPickerEnabled) return;

	UR::ThreadAttach();

	if (ImGui::IsKeyPressed(ImGuiKey_Escape))
	{
		Config::settings.inspector.objectPickerEnabled = false;
		return;
	}

	const auto& io = ImGui::GetIO();
	if (!io.MouseDrawCursor) return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (ImGui::GetIO().WantCaptureMouse) return;

		const ImVec2 mousePos = io.MousePos;
		const Vec2 screenPos = {mousePos.x, mousePos.y};

		if (GameObject* hitObj = Helper::RaycastPick(screenPos))
		{
			RefreshHierarchy();
			OpenObjectInNewTab(hitObj);
			Config::settings.inspector.objectPickerEnabled = false;
		}
	}
}
