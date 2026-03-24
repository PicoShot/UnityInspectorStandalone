#include "pch.h"
#include "hooks.h"
#include "console_hooks/console_hooks.h"
#include "quit_hooks/quit_hooks.h"
#include "steam_hooks/steam_hooks.h"

void Hooks::Init()
{
	s_Hooks.push_back(std::make_unique<ConsoleHooks>());
	s_Hooks.push_back(std::make_unique<QuitHooks>());
	s_Hooks.push_back(std::make_unique<SteamHooks>());

	for (const auto& hook : s_Hooks)
	{
		hook->Install();
	}
}
