#include "pch.h"
#include "input_forwarder.h"

namespace InputForwarder
{
	bool s_prevMenuKeyState = false;
	bool s_prevCursorToggleState = false;
	bool s_prevCursorUnlockState = false;

	bool IsMenuTogglePressed() noexcept
	{
		const bool currentState = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
		const bool pressed = currentState && !s_prevMenuKeyState;
		s_prevMenuKeyState = currentState;
		return pressed;
	}

	bool IsCursorTogglePressed() noexcept
	{
		const bool currentState = (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
		const bool pressed = currentState && !s_prevCursorToggleState;
		s_prevCursorToggleState = currentState;
		return pressed;
	}

	bool IsCursorUnlockPressed() noexcept
	{
		const bool currentState = (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
		const bool pressed = currentState && !s_prevCursorUnlockState;
		s_prevCursorUnlockState = currentState;
		return pressed;
	}
}
