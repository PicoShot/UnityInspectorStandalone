#include "pch.h"
#include "input.h"
#include "input_forwarder.h"
#include "external_overlay.h"
#include "imgui/imgui.h"
#include "config/config.h"

namespace Input
{
	enum class InputAction : uint8_t
	{
		ToggleMenu,
		ToggleCursor,
		UnlockCursor
	};

	void TriggerAction(InputAction action)
	{
		switch (action)
		{
		case InputAction::ToggleMenu:
			{
				auto& showMenu = Config::state.showMenu;
				showMenu = !showMenu;
				ImGui::GetIO().MouseDrawCursor = showMenu;
				ClipCursor(nullptr);

				if (Config::settings.ini.external_overlay)
					ExternalOverlay::SetInputCapture(showMenu);
				break;
			}
		case InputAction::ToggleCursor:
			{
				auto& showCursor = Config::state.showCursor;
				showCursor = !showCursor;
				showCursor ? ShowCursor(TRUE) : ShowCursor(FALSE);
				break;
			}
		case InputAction::UnlockCursor:
			{
				ClipCursor(nullptr);
				break;
			}
		}
	}

	bool ProcessMessage(UINT uMsg, WPARAM wParam)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_INSERT: TriggerAction(InputAction::ToggleMenu);
				return true;
			case VK_F5: TriggerAction(InputAction::ToggleCursor);
				return true;
			case VK_F6: TriggerAction(InputAction::UnlockCursor);
				return true;
			default: break;
			}
		}
		return false;
	}

	void ProcessExternal()
	{
		if (InputForwarder::IsMenuTogglePressed()) TriggerAction(InputAction::ToggleMenu);
		if (InputForwarder::IsCursorTogglePressed()) TriggerAction(InputAction::ToggleCursor);
		if (InputForwarder::IsCursorUnlockPressed()) TriggerAction(InputAction::UnlockCursor);
	}
}
