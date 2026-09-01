#include "pch.h"
#include "hooks.h"

void Hooks::Init()
{
	for (auto& factory : GetRegistry())
	{
		const auto hook = factory();
		hook->Install();
	}
}
