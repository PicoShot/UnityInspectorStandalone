#include "pch.h"
#include "application_hooks.h"

REGISTER_HOOK(ApplicationHooks)

void UNITY_CALLING_CONVENTION ApplicationHooks::HQuit()
{
	LOG_WARNING("Blocked UnityEngine.Application.Quit().");
}

void UNITY_CALLING_CONVENTION ApplicationHooks::HQuitWithExitCode(const int exitCode)
{
	LOG_WARNING("Blocked UnityEngine.Application.Quit({}).", exitCode);
}

void ApplicationHooks::Install()
{
	const auto* unityCoreModule = UR::Get("UnityEngine.CoreModule.dll");
	if (!unityCoreModule) return;

	auto* applicationClass = unityCoreModule->Get("Application", "UnityEngine");
	if (!applicationClass) return;

	if (auto* quit = applicationClass->Get<UR::Method>("Quit", {}))
	{
		if (auto* casted = quit->Cast<void>())
		{
			if (HookManager::Install(casted, HQuit))
				LOG_DEBUG("Successfully hooked UnityEngine.Application.Quit().");
		}
	}

	if (auto* quitWithExitCode = applicationClass->Get<UR::Method>("Quit", {"System.Int32"}))
	{
		if (auto* casted = quitWithExitCode->Cast<void, int>())
		{
			if (HookManager::Install(casted, HQuitWithExitCode))
				LOG_DEBUG("Successfully hooked UnityEngine.Application.Quit(System.Int32).");
		}
	}
}
