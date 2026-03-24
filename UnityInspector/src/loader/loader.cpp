#include "pch.h"
#include "loader.h"

#include "core/core.h"
#include "hooks/hooks.h"
#include "gui/window/external_overlay.h"
#include "gui/window/input_forwarder.h"
#include "gui/window/window.h"
#include "gui/window/window_finder.h"

void Loader::Load()
{
	Core::config->LoadConfig();
	FixSteam();
	CloseHandle(CreateThread(nullptr, 0, LoaderThread, nullptr, 0, nullptr));
}

void* Loader::HRuntimeInvoke(void* method, void* obj, void** params, void** exc)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&reinterpret_cast<PVOID&>(oRuntimeInvoke), HRuntimeInvoke);
	DetourTransactionCommit();

	const auto& config = Core::config;

	if (config->ini.debug_console) console::StartConsole(X("Pico Debug Console"), false);

	LOG_INFO(X("il2cpp detected & intercepted, initializing..."));

	UR::Init(config->internal.gameHandle, config->internal.unityMode);
	Hooks::Init();

	LOG_INFO(X("initialized successfully"));

	CloseHandle(CreateThread(nullptr, 0, OverlayInitThread, nullptr, 0, nullptr));

	return oRuntimeInvoke(method, obj, params, exc);
}

DWORD WINAPI Loader::MonoInitThread(LPVOID)
{
	const auto& config = Core::config->internal;

	console::StartConsole(X("Pico Debug Console"), false);
	LOG_INFO(X("mono loaded, initializing..."));

	UR::Init(config.gameHandle, config.unityMode);
	Hooks::Init();

	LOG_INFO(X("initialized successfully"));

	CloseHandle(CreateThread(nullptr, 0, OverlayInitThread, nullptr, 0, nullptr));

	return NULL;
}

void* Loader::HMonoAssemblyLoadFromFull(void* image, const char* fname, int status, unsigned* refi, void** error)
{
	void* result = oMonoAssemblyLoadFromFull(image, fname, status, refi, error);

	if (result && fname && !g_MonoInitDone)
	{
		const std::string stem = std::filesystem::path(fname).stem().string();
		if (stem == X("Assembly-CSharp"))
		{
			if (!g_MonoInitDone.exchange(true))
			{
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());
				DetourDetach(&reinterpret_cast<PVOID&>(oMonoAssemblyLoadFromFull), HMonoAssemblyLoadFromFull);
				DetourTransactionCommit();

				CloseHandle(CreateThread(nullptr, 0, MonoInitThread, nullptr, 0, nullptr));
			}
		}
	}

	return result;
}

void Loader::HookRuntimeInvoke(HMODULE gameAssembly, UR::Mode mode)
{
	bool expected = false;
	if (!g_Hooked.compare_exchange_strong(expected, true))
		return;

	if (mode == UR::Mode::Mono)
	{
		oMonoAssemblyLoadFromFull = reinterpret_cast<MonoAssemblyLoadFromFullFn>(
			GetProcAddress(gameAssembly, X("mono_assembly_load_from_full")));

		if (!oMonoAssemblyLoadFromFull)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&reinterpret_cast<PVOID&>(oMonoAssemblyLoadFromFull), HMonoAssemblyLoadFromFull);
		DetourTransactionCommit();
	}
	else
	{
		oRuntimeInvoke = reinterpret_cast<RuntimeInvokeFunc>(
			GetProcAddress(gameAssembly, X("il2cpp_runtime_invoke")));

		if (!oRuntimeInvoke)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&reinterpret_cast<PVOID&>(oRuntimeInvoke), HRuntimeInvoke);
		DetourTransactionCommit();
	}
}

void Loader::FixSteam()
{
	if (Core::config->ini.appid == 0) return;
	auto appid = std::to_string(Core::config->ini.appid);
	SetEnvironmentVariableA(X("SteamClientLaunch"), X("1"));
	SetEnvironmentVariableA(X("SteamGameId"), appid.c_str());
	SetEnvironmentVariableA(X("SteamAppId"), appid.c_str());
	SetEnvironmentVariableA(X("SteamOverlayGameId"), appid.c_str());
	SetEnvironmentVariableA(X("SteamEnv"), X("1"));

	HKEY hKey = nullptr;
	DWORD status = RegOpenKeyExA(HKEY_CURRENT_USER,
								 X("Software\\Valve\\Steam\\ActiveProcess"), 0,
								 KEY_QUERY_VALUE, &hKey);

	if (status != ERROR_SUCCESS)
	{
		LOG_ERROR(X("RegOpenKeyExA failed: {}"), status);
		return;
	}

	const auto keyGuard =
		std::unique_ptr<std::remove_pointer_t<HKEY>, decltype(&RegCloseKey)>(
		hKey, RegCloseKey);

	std::string clientDllPath(MAX_PATH, '\0');
	DWORD dataSize = static_cast<DWORD>(clientDllPath.size());

	status = RegQueryValueExA(hKey, X("SteamClientDll"), nullptr, nullptr,
							  reinterpret_cast<LPBYTE>(clientDllPath.data()),
							  &dataSize);

	if (status != ERROR_SUCCESS)
	{
		LOG_ERROR(X("RegQueryValueExA failed: {}"), status);
		return;
	}

	clientDllPath.resize(dataSize > 0 ? dataSize - 1 : 0);

	namespace fs = std::filesystem;
	const fs::path overlayPath =
		fs::path(clientDllPath).parent_path() / X("GameOverlayRenderer64.dll");

	if (!LoadLibraryExW(overlayPath.wstring().c_str(), nullptr,
		LOAD_WITH_ALTERED_SEARCH_PATH))
	{
		LOG_ERROR(X("Failed to load steam overlay dll: {}"), GetLastError());
	}
}

void Loader::CleanupAndExit()
{
	LOG_INFO(X("Exiting..."));
	std::this_thread::sleep_for(3s);
	console::EndConsole();
	ExitThread(1);
}

DWORD WINAPI Loader::OverlayInitThread(LPVOID)
{
	auto& config = Core::config;
	if (!config->ini.internal_overlay && !config->ini.external_overlay)
		return NULL;

	bool hookSuccess = false;
	const std::vector<std::pair<dx_hook::Hk11::Mode, std::string>> hookMethods = {
		{dx_hook::Hk11::Mode::Discord, X("Discord")},
		{dx_hook::Hk11::Mode::Steam,   X("Steam")},
		{dx_hook::Hk11::Mode::Kiero,   X("Kiero")},
	};


	LOG_INFO(X("Initializing Overlay Hook."));
	while (!WindowFinder::FindGameWindow())
	{
		std::this_thread::sleep_for(2s);
	}

	if (config->ini.internal_overlay)
	{
		for (const auto& [method, name] : hookMethods)
		{
			if (dx_hook::Hk11::Build(Window::OnPresent, method))
			{
				LOG_INFO(X("Successfully initialized {} DirectX Hook."), name.c_str());
				hookSuccess = true;
				break;
			}
		}
	}

	if (!hookSuccess && config->ini.external_overlay)
	{
		HWND gameHwnd = nullptr;
		for (uint8_t i = 0; i < 10 && !gameHwnd; i++)
		{
			gameHwnd = WindowFinder::FindGameWindow();
			if (!gameHwnd)
				std::this_thread::sleep_for(500ms);
		}

		if (!gameHwnd)
		{
			LOG_ERROR(X("Failed to find game window for external overlay!"));
			CleanupAndExit();
		}

		LOG_INFO(X("Found game window: {}"), reinterpret_cast<uint64_t>(gameHwnd));

		const auto externalOverlay = std::make_unique<ExternalOverlay>();
		if (!externalOverlay->Create(gameHwnd))
		{
			LOG_ERROR(X("Failed to create external overlay!"));
			CleanupAndExit();
		}

		config->internal.externalOverlay = externalOverlay.get();
		InputForwarder::Initialize(externalOverlay->GetOverlayHwnd(), gameHwnd);
		externalOverlay->SetInputCapture(config->internal.showImGui);

		LOG_INFO(X("External overlay initialized successfully."));
		externalOverlay->RunRenderLoop();

		config->internal.externalOverlay = nullptr;
		InputForwarder::Shutdown();
		externalOverlay->Destroy();
		LOG_INFO(X("External overlay shut down."));
	}
	else
	{
		dx_hook::Hk11::SetWndProc(Window::MyWndProc);
	}
	return NULL;
}

DWORD WINAPI Loader::LoaderThread(LPVOID)
{
	auto& config = Core::config->internal;
	do
	{
		if (auto module = GetModuleHandleA(X("GameAssembly.dll")); module)
		{
			config.unityMode = UR::Mode::Il2Cpp;
			config.gameHandle = module;
			HookRuntimeInvoke(module, UR::Mode::Il2Cpp);
			break;
		}

		if (auto module = GetModuleHandleA(X("mono-2.0-bdwgc.dll")); module)
		{
			config.unityMode = UR::Mode::Mono;
			config.gameHandle = module;
			HookRuntimeInvoke(module, UR::Mode::Mono);
			break;
		}

		Sleep(1);
	}
	while (true);
	return NULL;
}