#pragma once
#include "pch.h"
#include "hooks/hooks.h"

class QuitHooks : public IHook
{
public:
	void Install() override;

private:
	static void UNITY_CALLING_CONVENTION HApplicationQuit0();
	static void UNITY_CALLING_CONVENTION HApplicationQuit1(int exitCode);
};
