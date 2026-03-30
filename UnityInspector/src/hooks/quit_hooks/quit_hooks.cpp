#include "pch.h"
#include "quit_hooks.h"
#include "core/core.h"

void UNITY_CALLING_CONVENTION QuitHooks::HApplicationQuit0()
{
	if (Core::context->settings.ini.avoid_quiting) return;
	HookManager::Fcall(HApplicationQuit0);
}

void UNITY_CALLING_CONVENTION QuitHooks::HApplicationQuit1(int exitCode)
{
	if (Core::context->settings.ini.avoid_quiting) return;
	HookManager::Fcall(HApplicationQuit1, exitCode);
}

void QuitHooks::Install()
{
	auto* unityCoreModule = UR::Get(X("UnityEngine.CoreModule.dll"));
	if (!unityCoreModule) return;

	auto* appClass = unityCoreModule->Get("Application", "UnityEngine");
	if (!appClass) return;

	if (auto* mQuit0 = appClass->Get<UR::Method>("Quit"))
	{
		if (auto* casted = mQuit0->Cast<void>())
			HookManager::Install(casted, HApplicationQuit0);
	}

	if (auto* mQuit1 = appClass->Get<UR::Method>("Quit", { "System.Int32" }))
	{
		if (auto* casted = mQuit1->Cast<void, int>())
			HookManager::Install(casted, HApplicationQuit1);
	}
}
