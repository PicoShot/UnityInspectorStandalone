#pragma once
#include <Windows.h>
#include <cstdio>
#include <print>
#include <format>
#include <string>

#define LOG_DEBUG(format, ...) console::OutConsole(console::Debug, format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(format, ...) console::OutConsole(console::Info, format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) console::OutConsole(console::Warning, format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) console::OutConsole(console::Error, format, __FILE__, __LINE__, ##__VA_ARGS__)


namespace console {
	enum OutType : uint8_t {
		Info,
		Debug,
		Warning,
		Error
	};
	enum Color : uint8_t {
		Black,
		Blue,
		Green,
		LightGreen,
		Red,
		Purple,
		Yellow,
		White,
		Grey,
		LightBlue,
		ThinGreen,
		LightLightGreen,
		LightRed,
		Lavender,
		CanaryYellow,
		BrightWhite
	};

	template<typename... Args>
	static auto OutConsole(const OutType type, const std::string& format, const std::string& file, int line, Args&&... args) -> void {
		const auto hWnd_ = GetStdHandle(STD_OUTPUT_HANDLE);

		std::string text;
		if constexpr (sizeof...(args) > 0) {
			text = std::vformat(format, std::make_format_args(args...));
		}
		else {
			text = format;
		}

		switch (type) {
		case Info:
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Green * 16);
			std::print(" ");
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Black);
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
			std::print("[");
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | Green);
			std::print("Info ");
			break;
		case Debug:
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Blue * 16);
			std::print(" ");
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Black);
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
			std::print("[");
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | Blue);
			std::print("Debug");
			break;
		case Warning:
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Yellow * 16);
			std::print(" ");
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Black);
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
			std::print("[");
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | Yellow);
			std::print("Warn ");
			break;
		case Error:
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Red * 16);
			std::print(" ");
			SetConsoleTextAttribute(hWnd_, BACKGROUND_INTENSITY | Black);
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
			std::print("[");
			SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | Red);
			std::print("Error");
			break;
		}
		SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
		std::print("]>[");
		SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | Purple);
		std::print("{}:{}", file.substr(file.find_last_of('\\') + 1), line);
		SetConsoleTextAttribute(hWnd_, FOREGROUND_INTENSITY | White);
		std::print("] :{}\n", text);
	}

	static auto StartConsole(const char* title, const bool close) -> HWND {
		HWND hWnd_ = nullptr;
		AllocConsole();
		SetConsoleTitleA(title);
		while (nullptr == hWnd_) hWnd_ = GetConsoleWindow();
		const auto menu_ = GetSystemMenu(hWnd_, FALSE);
		if (!close) DeleteMenu(menu_, SC_CLOSE, MF_BYCOMMAND);
		SetWindowLong(hWnd_, GWL_STYLE, GetWindowLong(hWnd_, GWL_STYLE) & ~WS_MAXIMIZEBOX);
		SetWindowLong(hWnd_, GWL_STYLE, GetWindowLong(hWnd_, GWL_STYLE) & ~WS_THICKFRAME);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w+", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w+", stderr);
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r+", stdin);
		return hWnd_;
	}

	static auto EndConsole() -> void {
		fclose(stdout);
		fclose(stderr);
		fclose(stdin);
		FreeConsole();
	}
}