#pragma once
#include "pch.h"
#include "hooks/hooks.h"
#include <cstdint>

class SteamHooks : public IHook
{
public:
	void Install() override;

private:
	static void UNITY_CALLING_CONVENTION HSteamInit();
	static bool UNITY_CALLING_CONVENTION HRestartAppIfNecessary(uint32_t appid);
	static void UNITY_CALLING_CONVENTION HFPSteamInit(uint32_t appid, bool asyncCallbacks = true);

	struct AppData
	{
		uint32_t id;
	};

	static void UNITY_CALLING_CONVENTION HHeathenSteamInit(AppData appId);
	static void UNITY_CALLING_CONVENTION HHeathenSteamInit2(AppData appId, void* actions);
};
