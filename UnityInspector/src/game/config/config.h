#pragma once
#include "pch.h"

struct Config {
private:
    struct Inspector {
        bool Enabled = false;
        bool AutoUpdateObject = false;
		bool AutoRefresh = false;
        bool ShowAssemblyExplorer = false;
        bool ShowDebugConsole = false;
    };
public:
    bool ShowImGui = true;
	HMODULE gameAssembly = nullptr;
	UR::Mode gameMode;
    Inspector inspector;
};