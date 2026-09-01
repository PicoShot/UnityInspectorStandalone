#include "rlabrecque_hooks.h"
#include "pch.h"

namespace
{
	struct AppId
	{
		uint32_t value{};
	};

	constexpr uint32_t SteamAppId = 480;
	constexpr std::array SteamworksAssemblies{
		"com.rlabrecque.steamworks.net.dll",
		"Steamworks.NET.dll",
	};

	std::mutex installMutex;
	bool restartInstalled{};
	bool clientAppIdInstalled{};
	bool serverAppIdInstalled{};

	bool UNITY_CALLING_CONVENTION HRestartAppIfNecessary(AppId)
	{
		return false;
	}

	AppId UNITY_CALLING_CONVENTION HGetClientAppId()
	{
		return {SteamAppId};
	}

	AppId UNITY_CALLING_CONVENTION HGetServerAppId()
	{
		return {SteamAppId};
	}

	bool IsSteamworksNetAssembly(const std::string &name)
	{
		return std::ranges::find(SteamworksAssemblies, name) !=
			   SteamworksAssemblies.end();
	}

	bool InstallHooks(const UR::Assembly *assembly)
	{
		if (!assembly)
			return false;

		std::scoped_lock lock(installMutex);

		if (!restartInstalled)
		{
			if (const auto steamApi = assembly->Get("SteamAPI", "Steamworks"))
			{
				if (auto restart = steamApi->Get<UR::Method>(
						"RestartAppIfNecessary", {"Steamworks.AppId_t"}))
				{
					restartInstalled = HookManager::Install(
						restart->Cast<bool, AppId>(), HRestartAppIfNecessary);
				}
			}
		}

		if (!clientAppIdInstalled)
		{
			if (const auto steamUtils = assembly->Get("SteamUtils", "Steamworks"))
			{
				if (auto getAppId = steamUtils->Get<UR::Method>("GetAppID"))
				{
					clientAppIdInstalled = HookManager::Install(
						getAppId->Cast<AppId>(), HGetClientAppId);
				}
			}
		}

		if (!serverAppIdInstalled)
		{
			if (const auto steamGameServerUtils =
					assembly->Get("SteamGameServerUtils", "Steamworks"))
			{
				if (auto getAppId =
						steamGameServerUtils->Get<UR::Method>("GetAppID"))
				{
					serverAppIdInstalled = HookManager::Install(
						getAppId->Cast<AppId>(), HGetServerAppId);
				}
			}
		}

		LOG_INFO(
			"Steamworks.NET hooks: RestartAppIfNecessary={}, client GetAppID={}, "
			"server GetAppID={}.",
			restartInstalled ? "installed" : "not found",
			clientAppIdInstalled ? "installed" : "not found",
			serverAppIdInstalled ? "installed" : "not found");

		return restartInstalled || clientAppIdInstalled || serverAppIdInstalled;
	}

	void OnAssemblyLoaded(UR::Assembly *assembly)
	{
		if (!assembly || !IsSteamworksNetAssembly(assembly->name))
			return;

		InstallHooks(assembly);
	}
}

namespace RlabrecqueHooks
{
	void Install()
	{
		for (const auto assemblyName : SteamworksAssemblies)
		{
			if (InstallHooks(UR::Get(assemblyName)))
				return;
		}

		if (UR::OnAssemblyLoaded(OnAssemblyLoaded))
			LOG_INFO("Steamworks.NET hooks: waiting for the assembly to load.");
	}
}
