#include "pch.h"
#include "steam_hooks.h"
#include "core/core.h"

void UNITY_CALLING_CONVENTION SteamHooks::HSteamInit()
{
	LOG_DEBUG(X("Init called."));
	HookManager::Fcall(HSteamInit);
}

bool UNITY_CALLING_CONVENTION SteamHooks::HRestartAppIfNecessary(uint32_t appid)
{
	LOG_DEBUG(X("Restart app called."));
	return false;
}

void UNITY_CALLING_CONVENTION SteamHooks::HFPSteamInit(uint32_t appid, bool asyncCallbacks)
{
	appid = Core::config->ini.appid;
	LOG_DEBUG(X("F Init called."));
	HookManager::Fcall(HFPSteamInit, appid, asyncCallbacks);
}

void UNITY_CALLING_CONVENTION SteamHooks::HHeathenSteamInit(AppData appId)
{
	LOG_DEBUG(X("H Init called."));
	appId.id = Core::config->ini.appid;
	HookManager::Fcall(HHeathenSteamInit, appId);
}

void UNITY_CALLING_CONVENTION SteamHooks::HHeathenSteamInit2(AppData appId, void* actions)
{
	LOG_DEBUG(X("H Init (with actions) called."));
	appId.id = Core::config->ini.appid;
	HookManager::Fcall(HHeathenSteamInit2, appId, actions);
}

void SteamHooks::Install()
{
	if (Core::config->ini.appid == 0) return;

	auto* steamworksModule = UR::Get(X("com.rlabrecque.steamworks.net.dll"));
	if (steamworksModule)
	{
		auto* steamworksClass = steamworksModule->Get(X("SteamAPI"), X("Steamworks"));
		if (steamworksClass)
		{
			if (auto* mInit = steamworksClass->Get<UR::Method>(X("Init")))
			{
				if (auto* casted = mInit->Cast<void>())
					HookManager::Install(casted, HSteamInit);
			}

			if (auto* mRestartAppIfNecessary = steamworksClass->Get<UR::Method>(X("RestartAppIfNecessary"), { "*" }))
			{
				if (auto* casted = mRestartAppIfNecessary->Cast<bool, uint32_t>())
					HookManager::Install(casted, HRestartAppIfNecessary);
			}
		}
	}

	auto* fpSteamworksModule = UR::Get(X("Facepunch.Steamworks.Win64.dll"));
	if (fpSteamworksModule)
	{
		auto* fpSteamClientClass = fpSteamworksModule->Get(X("SteamClient"), X("Steamworks"));
		if (fpSteamClientClass)
		{
			if (auto* mInit = fpSteamClientClass->Get<UR::Method>(X("Init")))
			{
				if (auto* casted = mInit->Cast<void, uint32_t, bool>())
				{
					HookManager::Install(casted, HFPSteamInit);
				}
			}
		}

		auto* fpSteamAPIClass = fpSteamworksModule->Get(X("SteamAPI"), X("Steamworks"));
		if (fpSteamAPIClass)
		{
			if (auto* mRestartAppIfNecessary = fpSteamAPIClass->Get<UR::Method>(X("RestartAppIfNecessary")))
			{
				if (auto* casted = mRestartAppIfNecessary->Cast<bool, uint32_t>())
				{
					HookManager::Install(casted, HRestartAppIfNecessary);
				}

			}
		}
	}

	auto* heathenSteamworksModule = UR::Get(X("Heathen.Steamworks.dll"));
	if (heathenSteamworksModule)
	{
		auto* clientClass = heathenSteamworksModule->Get(X("Client"), X("*"));
		if (clientClass)
		{
			if (auto* mInit = clientClass->Get<UR::Method>("Initialize", { "*" }))
			{
				if (auto* casted = mInit->Cast<void, AppData>())
				{
					HookManager::Install(casted, HHeathenSteamInit);
				}
			}

			if (auto* mInit2 = clientClass->Get<UR::Method>("Initialize", { "*", "*" }))
			{
				if (auto* casted = mInit2->Cast<void, AppData, void*>())
				{
					HookManager::Install(casted, HHeathenSteamInit2);
				}
			}
		}
	}
}
