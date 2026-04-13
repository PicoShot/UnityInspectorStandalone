#pragma once
#include "pch.h"

namespace ExternalOverlay
{
	bool Create(HWND gameHwnd);
	void RunRenderLoop();
	void SetInputCapture(bool capture);
}