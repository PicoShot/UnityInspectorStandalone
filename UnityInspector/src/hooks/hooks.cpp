#include "pch.h"
#include "hooks.h"

void Hooks::Init()
{
	for (auto& factory : GetRegistry())
	{
		auto hook = factory();
		hook->Install();
	}
}
