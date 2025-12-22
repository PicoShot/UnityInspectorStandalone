#include "pch.h"

#include "game/core/core.h"
#include "gui/window/window.h"

BOOL Init()
{
    console::StartConsole(X("Pico Debug Console"), true);
	const auto config = Core::config.get();
	uint8_t try_count = 0;

    do {
	    if (config->gameAssembly = GetModuleHandleA(X("mono-2.0-bdwgc.dll")); config->gameAssembly)
	    {
            config->gameMode = UR::Mode::Mono;
            break;
	    }
        if (config->gameAssembly = GetModuleHandleA(X("GameAssembly.dll")); config->gameAssembly)
        {
            config->gameMode = UR::Mode::Il2Cpp;
            break;
        }

        std::this_thread::sleep_for(5s);
        try_count++;

        if (constexpr uint8_t max_try = 5; try_count >= max_try)
            return FALSE;
    } while (true);

    LOG_INFO(X("Game Loaded, Initializing..."));
    UR::Init(config->gameAssembly, config->gameMode);

    LOG_INFO(X("initializing Overlay Hook."));

    const std::vector<std::pair<dx_hook::Hk11::Mode, const char*>> hookMethods = 
    {
        {dx_hook::Hk11::Mode::Discord, "Discord"},
        {dx_hook::Hk11::Mode::Steam, "Steam"},
        {dx_hook::Hk11::Mode::Kiero, "Kiero"},
        //{dx_hook::Hk11::Mode::SteamLegacy, "Steam Legacy"},
    };

    bool hookSuccess = false;

    for (const auto& [method, name] : hookMethods) {
        LOG_DEBUG(X("Trying {} DirectX Hook."), name);
        if (dx_hook::Hk11::Build(Window::OnPresent, method)) {
            LOG_INFO(X("Successfully initialized {} DirectX Hook."), name);
            hookSuccess = true;
            break;
        }

        LOG_ERROR(X("Failed to initialize {} DirectX Hook."), name);
    }

    if (!hookSuccess) {
        LOG_ERROR(X("All hooking methods failed!"));
        console::EndConsole();
        return FALSE;
    }

    dx_hook::Hk11::SetWndProc(Window::MyWndProc);

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), nullptr, 0, nullptr);
    }
    return TRUE;
}
