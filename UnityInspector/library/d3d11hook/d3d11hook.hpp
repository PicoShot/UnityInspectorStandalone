#pragma once

#include <Windows.h>
#include <psapi.h>
#include <d3d11.h>
#include <functional>
#include "kiero/kiero.h"
#include "detours/HookManager.h"
#pragma comment(lib, "d3d11.lib")
namespace dx_hook {
    class Hk11 {
    public:
        enum class Mode : uint8_t {
            Discord,
            Steam,
            SteamLegacy,
            Kiero
        };
        static auto Build(const std::function<void()>& fn, Mode mode = Mode::Kiero) -> bool {
            if (!fn) return false;
            present = fn;
            currentMode = mode;
            switch (mode) {
            case Mode::Kiero:
            {
                if (kiero::Init(kiero::RenderType::D3D11) != kiero::Status::Success) {
                    return false;
                }
                if (kiero::Bind(8, MyPresent) == kiero::Status::Success) {
                    return true;
                }
                break;
            }
            case Mode::Steam:
            {
                HMODULE steamModule = GetModuleHandleA("GameOverlayRenderer64.dll");
                if (!steamModule) {
                    return false;
                }

                uint64_t refAddr = PatternScan("GameOverlayRenderer64.dll", "48 8B 05 ? ? ? ? FF D0 E9 ? ? ? ? 4C 8D 25");
                if (!refAddr) {
                    return false;
                }

                int32_t relativeOffset = *reinterpret_cast<int32_t*>(refAddr + 3);
                uint64_t varAddr = refAddr + relativeOffset + 7;

                if (IsBadReadPtr(reinterpret_cast<void*>(varAddr), sizeof(NewSteamPresentFunc*))) {
                    return false;
                }

                newSteamPresentPtr = reinterpret_cast<NewSteamPresentFunc*>(varAddr);
                if (!*newSteamPresentPtr) {
                    return false;
                }

                oPresentNewSteam = *newSteamPresentPtr;
                *newSteamPresentPtr = &NewSteamPresent;

                return true;
            }
            case Mode::SteamLegacy:
            {
                HMODULE steamModule = GetModuleHandleA("GameOverlayRenderer64.dll");
                if (!steamModule) {
                    return false;
                }
                uint64_t presentAddr = PatternScan("GameOverlayRenderer64.dll",
                    "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8");
                if (!presentAddr) return false;
                uint64_t createHookAddr = PatternScan("GameOverlayRenderer64.dll",
                    "48 89 5C 24 ? 57 48 83 EC ? 33 C0");
                if (!createHookAddr) return false;
                using CreateHookFunc = __int64(__fastcall*)(
                    unsigned __int64 pFuncAddress,
                    __int64 pDetourFuncAddress,
                    unsigned __int64* pOriginalFuncAddressOut,
                    int a4,
                    __int64 a5
                    );
                CreateHookFunc createHook = (CreateHookFunc)createHookAddr;
                __int64 result = createHook(
                    (unsigned __int64)presentAddr,
                    (__int64)&SteamPresent,
                    (unsigned __int64*)&oPresentSteam,
                    1,
                    (__int64)nullptr
                );
                return true;
            }
            case Mode::Discord:
            {
                HMODULE discordModule = GetModuleHandleA("DiscordHook64.dll");
                if (!discordModule) {
                    return false;
                }
                uint64_t refAddr = PatternScan("DiscordHook64.dll", "48 8B 05 ? ? ? ? 48 89 D9 89");
                if (!refAddr) {
                    return false;
                }
                int32_t relativeOffset = *reinterpret_cast<int32_t*>(refAddr + 3);
                uint64_t varAddr = refAddr + relativeOffset + 7;
                if (IsBadReadPtr(reinterpret_cast<void*>(varAddr), sizeof(DiscordPresentFunc*))) {
                    return false;
                }
                discordPresentPtr = reinterpret_cast<DiscordPresentFunc*>(varAddr);
                if (!*discordPresentPtr) {
                    return false;
                }
                oPresentDiscord = *discordPresentPtr;
                *discordPresentPtr = &DiscordPresent;
                return true;
            }
            default:
                break;
            }
            return false;
        }
        static auto SetWndProc(LRESULT(*wndProcFunc)(HWND, UINT, WPARAM, LPARAM)) -> void {
            if (!hWnd) {
                wndProc = wndProcFunc;
                pendingWndProc = true;
                return;
            }
            wndProc = wndProcFunc;
            oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
            pendingWndProc = false;
        }
        static auto Unbuild() -> void {
            if (oldWndProc && hWnd) {
                SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oldWndProc));
            }
            switch (currentMode) {
            case Mode::Kiero:
                kiero::Unbind(8);
                break;
            case Mode::Steam:
                // Restore the original Steam Present function
                if (newSteamPresentPtr && oPresentNewSteam) {
                    *newSteamPresentPtr = oPresentNewSteam;
                    newSteamPresentPtr = nullptr;
                    oPresentNewSteam = nullptr;
                }
                break;
            case Mode::SteamLegacy:
                oPresentSteam = nullptr;
                break;
            case Mode::Discord:
                if (discordPresentPtr && oPresentDiscord) {
                    *discordPresentPtr = oPresentDiscord;
                    discordPresentPtr = nullptr;
                    oPresentDiscord = nullptr;
                }
                break;
            }
            // Reset state
            init = false;
            hWnd = nullptr;
            gDevice = nullptr;
            gSwapChain = nullptr;
            gContext = nullptr;
            gTargetView = nullptr;
        }
        static auto GetHwnd() -> HWND { return hWnd; }
        static auto GetDevice() -> ID3D11Device* { return gDevice; }
        static auto GetSwapChain() -> IDXGISwapChain* { return gSwapChain; }
        static auto GetContext() -> ID3D11DeviceContext* { return gContext; }
        static auto GetTargetView() -> ID3D11RenderTargetView* const* { return &gTargetView; }
    private:
        inline static bool init{ false };
        inline static bool pendingWndProc{ false };
        inline static HWND hWnd{};
        inline static WNDPROC oldWndProc{};
        inline static std::function<void()> present;
        inline static LRESULT(*wndProc)(HWND, UINT, WPARAM, LPARAM);
        inline static ID3D11Device* gDevice{};
        inline static IDXGISwapChain* gSwapChain{};
        inline static ID3D11DeviceContext* gContext{};
        inline static ID3D11RenderTargetView* gTargetView{};
        inline static Mode currentMode{ Mode::Kiero };

        // SteamOld hook specific variables
        using PresentFunc = HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT);
        inline static PresentFunc oPresentSteam{ nullptr };

        // Steam hook specific variables
        using NewSteamPresentFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
        inline static NewSteamPresentFunc oPresentNewSteam{ nullptr };
        inline static NewSteamPresentFunc* newSteamPresentPtr{ nullptr };

        // Discord hook specific variables
        using DiscordPresentFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
        inline static DiscordPresentFunc oPresentDiscord{ nullptr };
        inline static DiscordPresentFunc* discordPresentPtr{ nullptr };

        static HRESULT __stdcall MyPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
            if (!init) {
                gSwapChain = pSwapChain;
                if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&gDevice)))) {
                    gDevice->GetImmediateContext(&gContext);
                    DXGI_SWAP_CHAIN_DESC sd;
                    pSwapChain->GetDesc(&sd);
                    hWnd = sd.OutputWindow;
                    ID3D11Texture2D* pBackBuffer = nullptr;
                    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer)))) {
                        if (SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView))) {
                            init = true;
                        }
                        pBackBuffer->Release();
                    }
                }
                if (pendingWndProc && wndProc && hWnd) {
                    oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
                    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
                    pendingWndProc = false;
                }
            }
            if (present && init) {
                present();
            }
            return HookManager::Scall(MyPresent, pSwapChain, SyncInterval, Flags);
        }

        static HRESULT __fastcall SteamPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
            if (!init) {
                gSwapChain = pSwapChain;
                if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&gDevice)))) {
                    gDevice->GetImmediateContext(&gContext);
                    DXGI_SWAP_CHAIN_DESC sd;
                    pSwapChain->GetDesc(&sd);
                    hWnd = sd.OutputWindow;
                    ID3D11Texture2D* pBackBuffer = nullptr;
                    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer)))) {
                        if (SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView))) {
                            init = true;
                        }
                        pBackBuffer->Release();
                    }
                }
                if (pendingWndProc && wndProc && hWnd) {
                    oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
                    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
                    pendingWndProc = false;
                }
            }
            if (present && init) {
                present();
            }
            return oPresentSteam(pSwapChain, SyncInterval, Flags);
        }

        static HRESULT __stdcall NewSteamPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
            if (!init) {
                gSwapChain = pSwapChain;
                if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&gDevice)))) {
                    gDevice->GetImmediateContext(&gContext);
                    DXGI_SWAP_CHAIN_DESC sd;
                    pSwapChain->GetDesc(&sd);
                    hWnd = sd.OutputWindow;
                    ID3D11Texture2D* pBackBuffer = nullptr;
                    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer)))) {
                        if (SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView))) {
                            init = true;
                        }
                        pBackBuffer->Release();
                    }
                }
                if (pendingWndProc && wndProc && hWnd) {
                    oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
                    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
                    pendingWndProc = false;
                }
            }
            if (present && init) {
                present();
            }
            return oPresentNewSteam(pSwapChain, SyncInterval, Flags);
        }

        static HRESULT __stdcall DiscordPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
            if (!init) {
                gSwapChain = pSwapChain;
                if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&gDevice)))) {
                    gDevice->GetImmediateContext(&gContext);
                    DXGI_SWAP_CHAIN_DESC sd;
                    pSwapChain->GetDesc(&sd);
                    hWnd = sd.OutputWindow;
                    ID3D11Texture2D* pBackBuffer = nullptr;
                    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer)))) {
                        if (SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView))) {
                            init = true;
                        }
                        pBackBuffer->Release();
                    }
                }
                if (pendingWndProc && wndProc && hWnd) {
                    oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
                    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
                    pendingWndProc = false;
                }
            }
            if (present && init) {
                present();
            }
            return oPresentDiscord(pSwapChain, SyncInterval, Flags);
        }

        static LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            if (wndProc) {
                LRESULT result = wndProc(hWnd, uMsg, wParam, lParam);
                switch (result) {
                case 0: return DefWindowProc(hWnd, uMsg, wParam, lParam);
                case 1: return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
                case 2: return 0;
                default: return result;
                }
            }
            return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
        }

        static MODULEINFO GetModuleInfo(const char* moduleName)
        {
            MODULEINFO modInfo = { 0 };
            HMODULE hModule = GetModuleHandleA(moduleName);
            if (!hModule)
                return modInfo;
            GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo));
            return modInfo;
        }

        static uint64_t PatternScan(const char* module, const char* signature)
        {
            static auto pattern_to_byte = [](const char* pattern)
                {
                    auto bytes = std::vector<char>{};
                    auto start = const_cast<char*>(pattern);
                    auto end = const_cast<char*>(pattern) + strlen(pattern);
                    for (auto current = start; current < end; ++current)
                    {
                        if (*current == '?')
                        {
                            ++current;
                            if (*current == '?')
                                ++current;
                            bytes.push_back('\?');
                        }
                        else
                        {
                            bytes.push_back(static_cast<char>(strtoul(current, &current, 16)));
                        }
                    }
                    return bytes;
                };
            MODULEINFO mInfo = GetModuleInfo(module);
            uint64_t base = reinterpret_cast<uint64_t>(mInfo.lpBaseOfDll);
            uint64_t sizeOfImage = mInfo.SizeOfImage;
            auto patternBytes = pattern_to_byte(signature);
            uint64_t patternLength = patternBytes.size();
            auto data = patternBytes.data();
            for (uint64_t i = 0; i < sizeOfImage - patternLength; i++)
            {
                bool found = true;
                for (uint64_t j = 0; j < patternLength; j++)
                {
                    char a = '\?';
                    char b = *reinterpret_cast<char*>(base + i + j);
                    found &= data[j] == a || data[j] == b;
                }
                if (found)
                {
                    return base + i;
                }
            }
            return NULL;
        }
    };
}