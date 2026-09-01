#include "pch.h"
#include "loader/loader.h"

void SetupEnvironmentVariables()
{
    _putenv_s("STEAMVIDEOTOKEN", "480");
    _putenv_s("SteamClientLaunch", "1");
    _putenv_s("SteamGameId","480");
    _putenv_s("SteamAppId", "480");
    _putenv_s("SteamOverlayGameId", "480");
    _putenv_s("SteamEnv", "1");
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		SetupEnvironmentVariables();
		Loader::Load();
	}
	return TRUE;
}
