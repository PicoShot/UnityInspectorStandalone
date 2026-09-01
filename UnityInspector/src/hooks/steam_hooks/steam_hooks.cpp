#include "steam_hooks.h"

#include "facepunch/facepunch_hooks.h"
#include "heathen/heathen_hooks.h"
#include "rlabrecque/rlabrecque_hooks.h"

REGISTER_HOOK(SteamHooks)

void SteamHooks::Install()
{
    FacepunchHooks::Install();
    HeathenHooks::Install();
    RlabrecqueHooks::Install();
}
