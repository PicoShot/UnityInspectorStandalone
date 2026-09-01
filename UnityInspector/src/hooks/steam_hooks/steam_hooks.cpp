#include "pch.h"
#include "steam_hooks.h"
#include "config/config.h"

REGISTER_HOOK(SteamHooks)

struct AppData
{
    uint32_t id{};
};

namespace
{
	constexpr uint32_t SteamAppId = 480;
	constexpr auto AppDataType = "HeathenEngineering.SteamworksIntegration.AppData";
	constexpr auto ServerConfigurationType = "HeathenEngineering.SteamworksIntegration.SteamGameServerConfiguration";

	UR::Class* FindNestedClass(const UR::Assembly* assembly, const std::string& nestedName)
	{
		for (const auto& klass : assembly->classes)
		{
			const auto& name = klass->m_name;
			if (name == nestedName ||
				name == "App/" + nestedName ||
				name == "App+" + nestedName ||
				name == "App." + nestedName)
			{
				return klass.get();
			}
		}
		return nullptr;
	}

	void UNITY_CALLING_CONVENTION HInitialize(AppData appId)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitialize, appId);
	}

	void UNITY_CALLING_CONVENTION HInitializeServer(AppData appId, void* serverConfiguration)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitializeServer, appId, serverConfiguration);
	}
}

void SteamHooks::Install()
{
	const auto* HeathenModule = UR::Get("Heathen.Steamworks.dll");
	if (!HeathenModule)
	{
		LOG_ERROR("Steam hooks: Heathen.Steamworks.dll was not found.");
		return;
	}

	const auto clientClass = FindNestedClass(HeathenModule, "Client");
	const auto serverClass = FindNestedClass(HeathenModule, "Server");

	LOG_INFO("Steam hooks: Found Heathen.Steamworks.dll, Client class: {}, Server class: {}", clientClass != nullptr, serverClass != nullptr);

	if (clientClass)
	{
		if (auto* initialize = clientClass->Get<UR::Method>("Initialize", { AppDataType }))
		{
			HookManager::Install(initialize->Cast<void, AppData>(), HInitialize);
		}
	}

	if (serverClass)
	{
		if (auto* initialize = serverClass->Get<UR::Method>("Initialize", { AppDataType, ServerConfigurationType }))
		{
			HookManager::Install(initialize->Cast<void, AppData, void*>(), HInitializeServer);
		}
	}
}
