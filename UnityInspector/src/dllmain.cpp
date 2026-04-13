#include "pch.h"
#include "loader/loader.h"

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		Loader::Load();
	}
	return TRUE;
}