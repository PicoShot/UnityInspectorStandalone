#pragma once
#include "features/debug_console/debug_console.h"
#include "hooks/hooks.h"

class ConsoleHooks : public IHook
{
public:
	std::string_view TargetAssembly() const override { return "UnityEngine.CoreModule.dll"; }
	void Install() override;

private:
	template <LogType Type, int Slot> static void UNITY_CALLING_CONVENTION HLog(void* message, void* methodInfo);
};
