#pragma once
#include "pch.h"

struct Config {
private:
    struct Inspector {
        bool Enabled = true;
        bool AutoUpdateObject = false;
		bool AutoRefresh = false;
        bool ShowAssemblyExplorer = true;
    };
public:
    bool ShowImGui = false;
	HMODULE gameAssembly = nullptr;
	UR::Mode gameMode;
    Inspector inspector;
};