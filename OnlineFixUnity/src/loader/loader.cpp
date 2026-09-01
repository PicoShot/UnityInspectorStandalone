#include "pch.h"
#include "loader.h"

#include "config/config.h"
#include "hooks/hooks.h"

namespace Loader
{
	static DWORD WINAPI LoaderThread(LPVOID)
	{
		do
		{
			if (const auto module = GetModuleHandleA("GameAssembly.dll"); module)
			{
				Config::state.unityMode = UR::Mode::Il2Cpp;
				Config::state.gameHandle = module;
				break;
			}

			if (const auto module = GetModuleHandleA("mono-2.0-bdwgc.dll"); module)
			{
				Config::state.unityMode = UR::Mode::Mono;
				Config::state.gameHandle = module;
				break;
			}

			Sleep(100);
		} while (true);

		if (Config::settings.ini.debug_console)
			console::StartConsole(std::format(
									  "Pico Debug Console ({})",
									  Config::state.unityMode == UR::Mode::Il2Cpp ? "Il2Cpp" : "Mono")
									  .c_str(),
								  false);
		LOG_INFO("loaded, initializing...");

		if (Config::state.unityMode == UR::Mode::Mono)
			Hooks::Init();

		UR::Init(Config::state.gameHandle, Config::state.unityMode);

		if (Config::state.unityMode == UR::Mode::Il2Cpp)
			Hooks::Init();

		LOG_INFO("initialized successfully");
		return NULL;
	}

	void Load()
	{
		Config::settings.Load();
		CloseHandle(CreateThread(nullptr, 0, LoaderThread, nullptr, 0, nullptr));
	}
}
