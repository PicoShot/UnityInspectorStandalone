#include "pch.h"
#include "window_finder.h"

namespace WindowFinder
{
	struct EnumData
	{
		DWORD processId;
		HWND consoleHwnd;
		HWND bestHandle;
	};

	HWND FindGameWindow()
	{
		const DWORD currentProcessId = GetCurrentProcessId();
		const HWND consoleHwnd = GetConsoleWindow();
		EnumData data{currentProcessId, consoleHwnd, nullptr};

		EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));

		return data.bestHandle;
	}

	BOOL CALLBACK EnumWindowsCallback(const HWND hwnd, const LPARAM lParam)
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

	bool IsMainWindow(const HWND hwnd, const HWND consoleHwnd)
	{
		if (hwnd == consoleHwnd) return false;
		if (!IsWindowVisible(hwnd)) return false;
		if (GetParent(hwnd) != nullptr) return false;

		RECT rect;
		GetWindowRect(hwnd, &rect);

		if (constexpr int MIN_WINDOW_SIZE = 100; (rect.right - rect.left) < MIN_WINDOW_SIZE || (rect.bottom - rect.top) < MIN_WINDOW_SIZE) return false;

		return true;
	}

	bool IsConsoleWindow(const HWND hwnd)
	{
		wchar_t className[256];
		if (GetClassNameW(hwnd, className, 256) == 0) return false;

		return wcscmp(className, L"ConsoleWindowClass") == 0;
	}
}
