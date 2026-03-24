#include "pch.h"

class Loader
{
public:
	static void Load();
private:
	static void HookRuntimeInvoke(HMODULE gameAssembly, UR::Mode mode);
	static void* HRuntimeInvoke(void* method, void* obj, void** params, void** exc);
	static void* HMonoAssemblyLoadFromFull(void* image, const char* fname, int status, unsigned* refi, void** error);
	static DWORD WINAPI OverlayInitThread(LPVOID);
	static DWORD WINAPI MonoInitThread(LPVOID);
	static DWORD WINAPI LoaderThread(LPVOID);
	static void FixSteam();
	static void CleanupAndExit();

	inline static std::atomic<bool> g_Hooked = false;
	inline static std::atomic<bool> g_MonoInitDone = false;
	using RuntimeInvokeFunc = void* (*)(void*, void*, void**, void**);
	inline static RuntimeInvokeFunc oRuntimeInvoke = nullptr;
	using MonoAssemblyLoadFromFullFn = void* (*)(void* image, const char* fname, int status, unsigned* refi, void** error);
	inline static MonoAssemblyLoadFromFullFn oMonoAssemblyLoadFromFull = nullptr;
};