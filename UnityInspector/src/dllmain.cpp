#include "pch.h"
#include "loader/loader.h"

void SetupEnvironmentVariables()
{
	_putenv_s(X("STEAMVIDEOTOKEN"), X("480"));
	_putenv_s(X("SteamClientLaunch"), X("1"));
	_putenv_s(X("SteamGameId"), X("480"));
	_putenv_s(X("SteamAppId"), X("480"));
	_putenv_s(X("SteamOverlayGameId"), X("480"));
	_putenv_s(X("SteamEnv"), X("1"));
}

void LoadSteamOverlay()
{
	const auto registryPath = X("SOFTWARE\\WOW6432Node\\Valve\\Steam");
	const auto valueName = X("InstallPath");

	DWORD pathSize = 0;
	if (RegGetValueA(HKEY_LOCAL_MACHINE, registryPath, valueName,
					 RRF_RT_REG_SZ, nullptr, nullptr, &pathSize) != ERROR_SUCCESS ||
		pathSize == 0)
	{
		return;
	}

	std::string steamPath(pathSize / sizeof(char), '\0');
	if (RegGetValueA(HKEY_LOCAL_MACHINE, registryPath, valueName,
					 RRF_RT_REG_SZ, nullptr, steamPath.data(), &pathSize) != ERROR_SUCCESS)
	{
		return;
	}

	steamPath.resize(strlen(steamPath.c_str()));
	if (!steamPath.empty() && steamPath.back() != '\\' && steamPath.back() != '/')
		steamPath += '\\';

	steamPath += X("gameoverlayrenderer64.dll");

	if (GetModuleHandleA(steamPath.c_str()))
		return;

	LoadLibraryA(steamPath.c_str()) ? 0 : 1;
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
