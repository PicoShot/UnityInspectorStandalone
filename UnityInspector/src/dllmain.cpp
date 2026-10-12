#include "pch.h"
#include "helper/helper.h"
#include "loader/loader.h"

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		Loader::Load();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		Helper::g_ShuttingDown = true;
	}
	return TRUE;
}
