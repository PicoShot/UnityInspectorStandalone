#include "pch.h"
#include "heathen_hooks.h"

namespace
{
	struct AppData
	{
		uint32_t id{};
	};

	constexpr uint32_t SteamAppId = 480;
	constexpr auto AppDataType = "HeathenEngineering.SteamworksIntegration.AppData";
	constexpr auto ServerConfigurationType = "HeathenEngineering.SteamworksIntegration.SteamGameServerConfiguration";
	std::atomic_bool clientInstalled{};
	std::atomic_bool serverInstalled{};
	std::atomic_flag installing = ATOMIC_FLAG_INIT;

	void UNITY_CALLING_CONVENTION HInitialize(AppData appId)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitialize, appId);
	}

	void UNITY_CALLING_CONVENTION HInitializeServer(AppData appId, void *serverConfiguration)
	{
		appId.id = SteamAppId;
		HookManager::Fcall(HInitializeServer, appId, serverConfiguration);
	}

	bool InstallHooks()
	{
		const auto *module = UR::Get("Heathen.Steamworks.dll");
		if (!module)
			return false;

		static const auto clientClass = module->Get("Client");

		if (clientClass && !clientInstalled)
		{
			if (auto *initialize = clientClass->Get<UR::Method>("Initialize", {AppDataType}))
				clientInstalled = HookManager::Install(initialize->Cast<void, AppData>(), HInitialize);
		}

		static const auto serverClass = module->Get("Server");

		if (serverClass && !serverInstalled)
		{
			if (auto *initialize = serverClass->Get<UR::Method>("Initialize", {AppDataType, ServerConfigurationType}))
				serverInstalled = HookManager::Install(initialize->Cast<void, AppData, void *>(), HInitializeServer);
		}

		return clientInstalled || serverInstalled;
	}

	void OnAssemblyLoaded(void *assembly, void *)
	{
		if (!assembly || clientInstalled)
			return;

		const auto image = UR::Invoke<void *>("mono_assembly_get_image", assembly);
		const auto name = image ? UR::Invoke<const char *>("mono_image_get_name", image) : nullptr;
		if (!name || (std::string_view(name) != "Heathen.Steamworks" &&
					  std::string_view(name) != "Heathen.Steamworks.dll"))
		{
			return;
		}

		if (installing.test_and_set())
			return;

		LOG_INFO("Steam hooks: Heathen.Steamworks assembly loaded.");
		UR::RefreshAssemblies();
		InstallHooks();
		installing.clear();
	}
}
namespace HeathenHooks
{
	void Install()
	{
		if (InstallHooks())
			return;

		static std::once_flag loadHook;
		std::call_once(loadHook, []
					   {
		if (!UR::RegisterAssemblyLoadCallback(OnAssemblyLoaded))
			return;
		
		UR::RefreshAssemblies();
		InstallHooks(); });
	}
}