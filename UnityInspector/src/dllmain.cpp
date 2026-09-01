#include "pch.h"
#include "loader/loader.h"

#pragma comment(lib, "Advapi32.lib")

void SetupEnvironmentVariables()
{
    _putenv_s("STEAMVIDEOTOKEN", "480");
    _putenv_s("SteamClientLaunch", "1");
    _putenv_s("SteamGameId","480");
    _putenv_s("SteamAppId", "480");
    _putenv_s("SteamOverlayGameId", "480");
    _putenv_s("SteamEnv", "1");
}

namespace
{
	DWORD WINAPI LoadSteamOverlayThread(LPVOID)
	{
		constexpr auto registryPath = L"SOFTWARE\\WOW6432Node\\Valve\\Steam";
		constexpr auto valueName = L"InstallPath";

		DWORD pathSize = 0;
		if (RegGetValueW(HKEY_LOCAL_MACHINE, registryPath, valueName,
			RRF_RT_REG_SZ, nullptr, nullptr, &pathSize) != ERROR_SUCCESS || pathSize == 0)
		{
			return 1;
		}

		std::wstring steamPath(pathSize / sizeof(wchar_t), L'\0');
		if (RegGetValueW(HKEY_LOCAL_MACHINE, registryPath, valueName,
			RRF_RT_REG_SZ, nullptr, steamPath.data(), &pathSize) != ERROR_SUCCESS)
		{
			return 1;
		}

		steamPath.resize(wcslen(steamPath.c_str()));
		if (!steamPath.empty() && steamPath.back() != L'\\' && steamPath.back() != L'/')
			steamPath += L'\\';

#ifdef _WIN64
		steamPath += L"gameoverlayrenderer64.dll";
#else
		steamPath += L"gameoverlayrenderer.dll";
#endif

		if (GetModuleHandleW(steamPath.c_str()))
			return 0;

		return LoadLibraryW(steamPath.c_str()) ? 0 : 1;
	}

	void LoadSteamOverlay()
	{
		if (const auto thread = CreateThread(nullptr, 0, LoadSteamOverlayThread, nullptr, 0, nullptr))
			CloseHandle(thread);
	}
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		SetupEnvironmentVariables();
		LoadSteamOverlay();
		Loader::Load();
	}
	return TRUE;
}
