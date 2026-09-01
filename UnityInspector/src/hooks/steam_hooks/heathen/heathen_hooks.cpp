#include "heathen_hooks.h"
#include "pch.h"

namespace
{
	struct AppData
	{
		uint32_t id{};
	};

	constexpr uint32_t SteamAppId = 480;
	constexpr auto AppDataType = "HeathenEngineering.SteamworksIntegration.AppData";
	constexpr auto ServerConfigurationType =
		"HeathenEngineering.SteamworksIntegration.SteamGameServerConfiguration";
	std::atomic_bool clientInstalled{};
	std::atomic_bool serverInstalled{};

	void UNITY_CALLING_CONVENTION HInitialize(AppData appId)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitialize, appId);
	}

	void UNITY_CALLING_CONVENTION HInitializeServer(AppData appId,
													void *serverConfiguration)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitializeServer, appId, serverConfiguration);
	}

	bool InstallHooks(const UR::Assembly *module)
	{
		if (!module)
			return false;

		static const auto clientClass = module->Get("Client");

		if (clientClass && !clientInstalled)
		{
			if (auto *initialize =
					clientClass->Get<UR::Method>("Initialize", {AppDataType}))
				clientInstalled =
					HookManager::Install(initialize->Cast<void, AppData>(), HInitialize);
		}

		static const auto serverClass = module->Get("Server");

		if (serverClass && !serverInstalled)
		{
			if (auto *initialize = serverClass->Get<UR::Method>(
					"Initialize", {AppDataType, ServerConfigurationType}))
				serverInstalled = HookManager::Install(
					initialize->Cast<void, AppData, void *>(), HInitializeServer);
		}

		return clientInstalled || serverInstalled;
	}

	void OnAssemblyLoaded(UR::Assembly *assembly)
	{
		if (!assembly || assembly->name != "Heathen.Steamworks.dll")
			return;

		InstallHooks(assembly);
	}
}

namespace HeathenHooks
{
	void Install()
	{
		if (InstallHooks(UR::Get("Heathen.Steamworks.dll")))
			return;

		UR::OnAssemblyLoaded(OnAssemblyLoaded);
	}
}
