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
	std::atomic_bool appIdInstalled{};
	std::atomic_bool clientIdInstalled{};
	std::atomic_bool settingsApplicationIdInstalled{};

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

	AppData UNITY_CALLING_CONVENTION HGetAppId()
	{
		return {SteamAppId};
	}

	AppData UNITY_CALLING_CONVENTION HGetClientId()
	{
		return {SteamAppId};
	}

	AppData UNITY_CALLING_CONVENTION HGetSettingsApplicationId()
	{
		return {SteamAppId};
	}

	bool InstallHooks(const UR::Assembly *module)
	{
		if (!module)
			return false;

		const auto clientClass = module->Get("Client");

		if (clientClass && !clientInstalled)
		{
			if (auto *initialize =
					clientClass->Get<UR::Method>("Initialize", {AppDataType}))
				clientInstalled =
					HookManager::Install(initialize->Cast<void, AppData>(), HInitialize);
		}

		if (clientClass && !clientIdInstalled)
		{
			if (auto *getId = clientClass->Get<UR::Method>("get_Id"))
				clientIdInstalled = HookManager::Install(
					getId->Cast<AppData>(), HGetClientId);
		}

		const auto serverClass = module->Get("Server");

		if (serverClass && !serverInstalled)
		{
			if (auto *initialize = serverClass->Get<UR::Method>(
					"Initialize", {AppDataType, ServerConfigurationType}))
				serverInstalled = HookManager::Install(
					initialize->Cast<void, AppData, void *>(), HInitializeServer);
		}

		if (!appIdInstalled)
		{
			if (const auto appClass = module->Get(
					"App", "HeathenEngineering.SteamworksIntegration.API"))
			{
				if (auto *getId = appClass->Get<UR::Method>("get_Id"))
					appIdInstalled = HookManager::Install(
						getId->Cast<AppData>(), HGetAppId);
			}
		}

		if (!settingsApplicationIdInstalled)
		{
			if (const auto settingsClass = module->Get(
					"SteamSettings", "HeathenEngineering.SteamworksIntegration"))
			{
				if (auto *getApplicationId =
						settingsClass->Get<UR::Method>("get_ApplicationId"))
				{
					settingsApplicationIdInstalled = HookManager::Install(
						getApplicationId->Cast<AppData>(),
						HGetSettingsApplicationId);
				}
			}
		}

		LOG_INFO(
			"Heathen hooks: client Initialize={}, server Initialize={}, App.Id={}, "
			"Client.Id={}, SteamSettings.ApplicationId={}.",
			clientInstalled ? "installed" : "not found",
			serverInstalled ? "installed" : "not found",
			appIdInstalled ? "installed" : "not found",
			clientIdInstalled ? "installed" : "not found",
			settingsApplicationIdInstalled ? "installed" : "not found");

		return clientInstalled || serverInstalled || appIdInstalled ||
			   clientIdInstalled || settingsApplicationIdInstalled;
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
