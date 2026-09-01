#pragma once
#include "pch.h"

struct RuntimeState
{
	HMODULE gameHandle = nullptr;
	UR::Mode unityMode;
};

struct UserSettings
{
	struct INISettings
	{
		uint32_t appid = 0;

		bool debug_console = true;
	} ini;

	void Load()
	{
		try
		{
			char buffer[MAX_PATH];
			GetModuleFileNameA(nullptr, buffer, MAX_PATH);
			const auto configPath = std::filesystem::path(buffer).parent_path() / "config.ini";

			if (!std::filesystem::exists(configPath))
			{
				ini::IniFile configFile;
				configFile["Config"]["debug_console"] = ini.debug_console;
				configFile.save(configPath.string());
				return;
			}

			ini::IniFile configFile;
			configFile.load(configPath.string());

			if (configFile.contains("Config"))
			{
				auto& c = configFile["Config"];
				if (c.contains("debug_console"))    ini.debug_console = c["debug_console"].as<bool>();
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
