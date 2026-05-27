#include "pch.h"
#include "loader.h"

#include "config/config.h"
#include "hooks/hooks.h"
#include "window/external_overlay.h"
#include "window/window.h"
#include "window/window_finder.h"
#include "features/features.h"

namespace Loader
{
	static DWORD WINAPI OverlayInitThread(LPVOID)
	{
		if (!Config::settings.ini.internal_overlay && !Config::settings.ini.external_overlay)
			return NULL;

		bool hookSuccess = false;

		LOG_INFO("Initializing Overlay Hook.");
		while (!WindowFinder::FindGameWindow()) Sleep(2000);

		if (Config::settings.ini.internal_overlay)
		{
			if (dx_hook::Hk11::Build(Window::OnPresent, dx_hook::Hk11::Mode::Kiero))
			{
				LOG_INFO("Successfully initialized DirectX Hook.");
				hookSuccess = true;
			}
		}

		if (!hookSuccess && Config::settings.ini.external_overlay)
		{
			HWND gameHwnd = nullptr;
			for (uint8_t i = 0; i < 10 && !gameHwnd; i++)
			{
				gameHwnd = WindowFinder::FindGameWindow();
				if (!gameHwnd) Sleep(500);
			}

			if (!gameHwnd)
			{
				LOG_ERROR("Failed to find game window for external overlay!");
			}

			LOG_INFO("Found game window: {}", reinterpret_cast<uint64_t>(gameHwnd));

			if (const auto externalOverlay = ExternalOverlay::Create(gameHwnd); !externalOverlay)
			{
				LOG_ERROR("Failed to create external overlay!");
			}

			ExternalOverlay::SetInputCapture(Config::state.showMenu);

			LOG_INFO("External overlay initialized successfully.");
			ExternalOverlay::RunRenderLoop();
		}
		else
		{
			dx_hook::Hk11::SetWndProc(Window::MyWndProc);
		}
		return NULL;
	}

	static DWORD WINAPI LoaderThread(LPVOID)
	{
		while (!WindowFinder::FindGameWindow()) Sleep(100);

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
		}
		while (true);

		if (Config::settings.ini.debug_console) console::StartConsole("Pico Debug Console", false);
		LOG_INFO("loaded, initializing...");

		UR::Init(Config::state.gameHandle, Config::state.unityMode);
		Hooks::Init();
		Features::Init();

		LOG_INFO("initialized successfully");

		CloseHandle(CreateThread(nullptr, 0, OverlayInitThread, nullptr, 0, nullptr));
		return NULL;
	}

	void Load()
	{
		Config::settings.Load();
		CloseHandle(CreateThread(nullptr, 0, LoaderThread, nullptr, 0, nullptr));
	}
}
