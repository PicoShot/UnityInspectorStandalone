#include "pch.h"
#include "console_hooks.h"

REGISTER_HOOK(ConsoleHooks)

template <LogType Type, int Slot> void UNITY_CALLING_CONVENTION ConsoleHooks::HLog(void* message, void* methodInfo)
{
	if (message) DebugConsole::CaptureManagedLog(message, Type);
	HookManager::Fcall(HLog<Type, Slot>, message, methodInfo);
}

void ConsoleHooks::Install()
{
	const auto* unityCoreModule = UR::Get("UnityEngine.CoreModule.dll");
	if (!unityCoreModule) return;

	auto* debugClass = unityCoreModule->Get("Debug", "UnityEngine");
	if (!debugClass) return;

	const auto hook = [debugClass](const char* name, const char* argType, auto handler)
	{
		if (auto* method = debugClass->Get<UR::Method>(name, {argType}))
		{
			if (auto* target = method->Cast<void, void*, void*>()) HookManager::Install(target, handler);
		}
	};

	hook("Log", "System.Object", HLog<LogType::Log, 0>);
	hook("Log", "System.String", HLog<LogType::Log, 1>);
	hook("LogWarning", "System.Object", HLog<LogType::Warning, 0>);
	hook("LogWarning", "System.String", HLog<LogType::Warning, 1>);
	hook("LogError", "System.Object", HLog<LogType::Error, 0>);
	hook("LogError", "System.String", HLog<LogType::Error, 1>);
	hook("LogException", "System.Exception", HLog<LogType::Exception, 0>);
	hook("LogAssertion", "System.Object", HLog<LogType::Assert, 0>);
}
