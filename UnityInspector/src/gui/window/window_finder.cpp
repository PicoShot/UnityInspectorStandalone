#include "pch.h"
#include "window_finder.h"

HWND WindowFinder::FindGameWindow() noexcept
{
    const DWORD currentProcessId = GetCurrentProcessId();
    const HWND consoleHwnd = GetConsoleWindow();
    EnumData data{ currentProcessId, consoleHwnd, nullptr };

    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));

    return data.bestHandle;
}

BOOL CALLBACK WindowFinder::EnumWindowsCallback(const HWND hwnd, const LPARAM lParam) noexcept
{
    const auto data = reinterpret_cast<EnumData*>(lParam);

    if (hwnd == data->consoleHwnd || IsConsoleWindow(hwnd)) return TRUE;

    DWORD windowProcessId = 0;
    GetWindowThreadProcessId(hwnd, &windowProcessId);

    if (windowProcessId != data->processId) return TRUE;
    if (!IsMainWindow(hwnd, data->consoleHwnd)) return TRUE;

    data->bestHandle = hwnd;
    return FALSE;
}

bool WindowFinder::IsMainWindow(const HWND hwnd, const HWND consoleHwnd) noexcept
{
    if (hwnd == consoleHwnd) return false;
    if (!IsWindowVisible(hwnd)) return false;
    if (GetParent(hwnd) != nullptr) return false;

    RECT rect;
    GetWindowRect(hwnd, &rect);

    constexpr int MIN_WINDOW_SIZE = 100;
    if ((rect.right - rect.left) < MIN_WINDOW_SIZE || (rect.bottom - rect.top) < MIN_WINDOW_SIZE) return false;

    return true;
}

bool WindowFinder::IsConsoleWindow(const HWND hwnd) noexcept
{
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256) == 0) return false;

    return wcscmp(className, L"ConsoleWindowClass") == 0;
}