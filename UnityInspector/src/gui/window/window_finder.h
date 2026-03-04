#pragma once
#include "pch.h"

struct WindowFinder
{
    [[nodiscard]] static HWND FindGameWindow() noexcept;

private:
    struct EnumData
    {
        DWORD processId;
        HWND consoleHwnd;
        HWND bestHandle;
    };

    static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) noexcept;
    static bool IsMainWindow(HWND hwnd, HWND consoleHwnd) noexcept;
    static bool IsConsoleWindow(HWND hwnd) noexcept;
};