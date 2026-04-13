#pragma once
#include "pch.h"

namespace WindowFinder
{
    HWND FindGameWindow();
    BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
    bool IsMainWindow(HWND hwnd, HWND consoleHwnd);
    bool IsConsoleWindow(HWND hwnd);
}