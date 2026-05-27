#pragma once
#include "pch.h"

namespace InputForwarder
{
	void Initialize(HWND overlayHwnd, HWND gameHwnd) noexcept;
	void Shutdown() noexcept;
	void Update() noexcept;

	bool IsMenuTogglePressed() noexcept;
	bool IsCursorTogglePressed() noexcept;
	bool IsCursorUnlockPressed() noexcept;
}