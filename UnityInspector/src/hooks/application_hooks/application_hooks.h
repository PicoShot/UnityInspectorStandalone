#pragma once
#include "hooks/hooks.h"

class ApplicationHooks : public IHook
{
public:
	void Install() override;

private:
	static void UNITY_CALLING_CONVENTION HQuit();
	static void UNITY_CALLING_CONVENTION HQuitWithExitCode(int exitCode);
};
