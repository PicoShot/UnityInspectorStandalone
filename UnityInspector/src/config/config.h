#pragma once
#include "pch.h"

enum class Theme
{
	Light,
	Dark,
	Classic,
	DarkPlus
};

struct RuntimeState
{
	bool showMenu = true;
	bool showCursor = false;
	HMODULE gameHandle = nullptr;
	UR::Mode unityMode;
};

struct UserSettings
{
	struct INISettings
	{
		uint32_t appid = 0;
#ifdef _DEBUG
		bool debug_console = true;
#else
		bool debug_console = false;
#endif
		bool internal_overlay = true;
		bool external_overlay = false;
		bool lua_jit_enabled = false;
	} ini;

	struct InspectorSettings
	{
		bool enabled = false;
		bool autoUpdateObject = false;
		bool autoRefresh = false;
		bool showAssemblyExplorer = false;
		bool showDebugConsole = false;
		bool objectPickerEnabled = false;
	} inspector;

	struct MemoryScannerSettings
	{
		bool showWindow = false;
	} memoryScanner;

	Theme theme = Theme::DarkPlus;

	void Load()
	{
		try
		{
			char buffer[MAX_PATH];
			GetModuleFileNameA(nullptr, buffer, MAX_PATH);
			const auto configPath = std::filesystem::path(buffer).parent_path() / "config.ini";

			if (!std::filesystem::exists(configPath)) return;

			ini::IniFile configFile;
			configFile.load(configPath.string());

			if (configFile.contains("Config"))
			{
				auto& c = configFile["Config"];
				if (c.contains("debug_console"))    ini.debug_console = c["debug_console"].as<bool>();
				if (c.contains("internal_overlay"))  ini.internal_overlay = c["internal_overlay"].as<bool>();
				if (c.contains("external_overlay"))  ini.external_overlay = c["external_overlay"].as<bool>();
				if (c.contains("lua_jit_enabled"))   ini.lua_jit_enabled = c["lua_jit_enabled"].as<bool>();
			}
		}
		catch (...)
		{
			LOG_DEBUG("Failed to load config");
		}
	}
};

namespace Config
{
	inline UserSettings settings;
	inline RuntimeState state;
}