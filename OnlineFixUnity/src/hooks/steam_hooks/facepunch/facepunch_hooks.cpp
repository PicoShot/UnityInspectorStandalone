#include "facepunch_hooks.h"
#include "pch.h"

namespace
{
	struct AppId
	{
		uint32_t value{};
	};

	constexpr uint32_t SteamAppId = 480;
	constexpr std::array FacepunchAssemblies{
		"Facepunch.Steamworks.Win64.dll",
		"Facepunch.Steamworks.Win32.dll",
		"Facepunch.Steamworks.Posix.dll",
		"Facepunch.Steamworks.dll",
	};

	std::mutex installMutex;
	bool clientInitInstalled{};
	bool serverInitInstalled{};
	bool restartInstalled{};
	bool appIdInstalled{};

	void UNITY_CALLING_CONVENTION HClientInit(uint32_t, bool asyncCallbacks)
	{
		HookManager::Fcall(HClientInit, SteamAppId, asyncCallbacks);
	}

	void UNITY_CALLING_CONVENTION HServerInit(AppId, void *serverInit,
											 bool asyncCallbacks)
	{
		HookManager::Fcall(
			HServerInit, AppId{SteamAppId}, serverInit, asyncCallbacks);
	}

	bool UNITY_CALLING_CONVENTION HRestartAppIfNecessary(uint32_t)
	{
		HookManager::Fcall(HRestartAppIfNecessary, SteamAppId);
		return false;
	}

	AppId UNITY_CALLING_CONVENTION HGetAppId()
	{
		return {SteamAppId};
	}

	bool IsFacepunchAssembly(const std::string &name)
	{
		return std::ranges::find(FacepunchAssemblies, name) !=
			   FacepunchAssemblies.end();
	}

	UR::Method *FindMethod(const UR::Class *klass, const std::string &name,
						   const std::vector<std::string> &arguments)
	{
		if (!klass)
			return nullptr;

		for (const auto &method : klass->methods)
		{
			if (method->name != name || method->m_args.size() != arguments.size())
				continue;

			bool matches = true;
			for (size_t index = 0; index < arguments.size(); ++index)
			{
				const auto &argument = method->m_args[index];
				if (!argument || !argument->pType ||
					argument->pType->name != arguments[index])
				{
					matches = false;
					break;
				}
			}

			if (matches)
				return method.get();
		}

		return nullptr;
	}

	bool InstallHooks(const UR::Assembly *assembly)
	{
		if (!assembly)
			return false;

		std::scoped_lock lock(installMutex);

		if (const auto steamClient = assembly->Get("SteamClient", "Steamworks"))
		{
			if (!clientInitInstalled)
			{
				if (auto *init = FindMethod(
						steamClient, "Init", {"System.UInt32", "System.Boolean"}))
				{
					clientInitInstalled = HookManager::Install(
						init->Cast<void, uint32_t, bool>(), HClientInit);
				}
			}

			if (!restartInstalled)
			{
				if (auto *restart = FindMethod(
						steamClient, "RestartAppIfNecessary", {"System.UInt32"}))
				{
					restartInstalled = HookManager::Install(
						restart->Cast<bool, uint32_t>(), HRestartAppIfNecessary);
				}
			}

			if (!appIdInstalled)
			{
				if (auto *getAppId = FindMethod(steamClient, "get_AppId", {}))
				{
					appIdInstalled = HookManager::Install(
						getAppId->Cast<AppId>(), HGetAppId);
				}
			}
		}

		if (!serverInitInstalled)
		{
			if (const auto steamServer = assembly->Get("SteamServer", "Steamworks"))
			{
				if (auto *init = FindMethod(
						steamServer, "Init",
						{"Steamworks.AppId", "Steamworks.SteamServerInit",
						 "System.Boolean"}))
				{
					serverInitInstalled = HookManager::Install(
						init->Cast<void, AppId, void *, bool>(), HServerInit);
				}
			}
		}

		LOG_INFO(
			"Facepunch hooks: client Init={}, server Init={}, "
			"RestartAppIfNecessary={}, AppId={}.",
			clientInitInstalled ? "installed" : "not found",
			serverInitInstalled ? "installed" : "not found",
			restartInstalled ? "installed" : "not found",
			appIdInstalled ? "installed" : "not found");

		return clientInitInstalled || serverInitInstalled || restartInstalled ||
			   appIdInstalled;
	}

	void OnAssemblyLoaded(UR::Assembly *assembly)
	{
		if (!assembly || !IsFacepunchAssembly(assembly->name))
			return;

		InstallHooks(assembly);
	}
}

namespace FacepunchHooks
{
	void Install()
	{
		for (const auto assemblyName : FacepunchAssemblies)
		{
			if (InstallHooks(UR::Get(assemblyName)))
				return;
		}

		if (UR::OnAssemblyLoaded(OnAssemblyLoaded))
			LOG_INFO("Facepunch hooks: waiting for the assembly to load.");
	}
}
