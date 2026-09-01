#include "steam_hooks.h"

#include "heathen/heathen_hooks.h"

REGISTER_HOOK(SteamHooks)

void SteamHooks::Install()
{
    HeathenHooks::Install();
}
