#pragma once
#include "pch.h"

struct ExternalOverlay;

struct Config
{
private:
	struct INIConfig
	{
		uint32_t appid = 0;
		bool debug_console = false;
		bool avoid_quiting = false;
		bool internal_overlay = true;
		bool external_overlay = false;
	};
	struct Internal
	{
		bool showImGui = true;
		HMODULE gameHandle = nullptr;
		UR::Mode unityMode;
		ExternalOverlay* externalOverlay = nullptr;
	};
	struct Inspector
	{
		bool enabled = false;
		bool autoUpdateObject = false;
		bool autoRefresh = false;
		bool showAssemblyExplorer = false;
		bool showDebugConsole = false;
	};
public:
	INIConfig ini;
	Internal internal;
	Inspector inspector;

	inline void LoadConfig() noexcept
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		auto configPath = std::filesystem::path(buffer).parent_path() / X("config.ini");

		if (!std::filesystem::exists(configPath)) return;

		ini::IniFile configFile;
		configFile.load(configPath.string());

		if (configFile.count(X("Config")))
		{
			auto& c = configFile[X("Config")];
			if (c.count(X("appid")))             ini.appid = c[X("appid")].as<uint32_t>();
			if (c.count(X("debug_console")))     ini.debug_console = c[X("debug_console")].as<bool>();
			if (c.count(X("avoid_quiting")))     ini.avoid_quiting = c[X("avoid_quiting")].as<bool>();
			if (c.count(X("internal_overlay")))  ini.internal_overlay = c[X("internal_overlay")].as<bool>();
			if (c.count(X("external_overlay")))  ini.external_overlay = c[X("external_overlay")].as<bool>();
		}
	}
};