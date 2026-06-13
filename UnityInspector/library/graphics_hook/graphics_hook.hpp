#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <functional>
#include <vector>

#include "kiero.hpp"
#include "kiero.generated.hpp"
#include "detours/HookManager.h"
#include "console/Console.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace graphics_hook
{
	enum class RenderAPI
	{
		None,
		D3D11,
		D3D12
	};

	using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
	using Present1Fn = HRESULT(__stdcall*)(IDXGISwapChain1*, UINT, UINT, const DXGI_PRESENT_PARAMETERS*);
	using ExecuteCommandListsFn = void(__stdcall*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);

	class GH
	{
	public:
		static auto Build(const std::function<void()>& fn) -> bool
		{
			if (!fn)
			{
				LOG_ERROR("GraphicsHook::Build failed: callback function is null!");
				return false;
			}
			present = fn;

			bool ok = false;

			// Try D3D11 Present Hook
			kiero::D3D11Output d3d11Out;
			if (auto err11 = kiero::locate<kiero::Implementation_D3D11>(nullptr, &d3d11Out); err11 == kiero::Error_Nil)
			{
				if (d3d11Out.swapchain_methods.size() > 8)
				{
					if (auto oPresent = d3d11Out.swapchain_methods[8]; HookManager::Install(
						reinterpret_cast<PresentFn>(oPresent), MyPresent))
					{
						ok = true;
					}
				}
				if (d3d11Out.swapchain_methods.size() > 22)
				{
					auto oPresent1 = d3d11Out.swapchain_methods[22];
					HookManager::Install(reinterpret_cast<Present1Fn>(oPresent1), MyPresent1);
				}
			}

			// Try D3D12 Present / ExecuteCommandLists Hook
			kiero::D3D12Output d3d12Out;
			if (auto err12 = kiero::locate<kiero::Implementation_D3D12>(nullptr, &d3d12Out); err12 == kiero::Error_Nil)
			{
				if (d3d12Out.swapchain_methods.size() > 8 && d3d12Out.command_queue_methods.size() > 10)
				{
					auto oPresent = d3d12Out.swapchain_methods[8];
					auto oExecuteCommandLists = d3d12Out.command_queue_methods[10];

					if (!ok)
					{
						if (HookManager::Install(reinterpret_cast<PresentFn>(oPresent), MyPresent))
						{
							ok = true;
						}
						if (d3d12Out.swapchain_methods.size() > 22)
						{
							auto oPresent1 = d3d12Out.swapchain_methods[22];
							HookManager::Install(reinterpret_cast<Present1Fn>(oPresent1), MyPresent1);
						}
					}

					HookManager::Install(reinterpret_cast<ExecuteCommandListsFn>(oExecuteCommandLists),
					                     MyExecuteCommandLists);
				}
			}

			return ok;
		}

		static auto SetWndProc(LRESULT (*wndProcFunc)(HWND, UINT, WPARAM, LPARAM)) -> void
		{
			if (!hWnd)
			{
				wndProc = wndProcFunc;
				pendingWndProc = true;
				return;
			}
			wndProc = wndProcFunc;
			oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
			pendingWndProc = false;
		}

		static auto Unbuild() -> void
		{
			if (oldWndProc && hWnd)
			{
				SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oldWndProc));
			}
			HookManager::DetachAll();

			init = false;
			hWnd = nullptr;
			gDevice = nullptr;
			gSwapChain = nullptr;
			gContext = nullptr;
			gTargetView = nullptr;
			gDevice12 = nullptr;
			gCommandQueue = nullptr;
			g_Api = RenderAPI::None;
		}

		static auto GetApi() -> RenderAPI { return g_Api; }
		static auto GetHwnd() -> HWND { return hWnd; }

		// D3D11
		static auto GetDeviceD3D11() -> ID3D11Device* { return gDevice; }
		static auto GetSwapChain() -> IDXGISwapChain* { return gSwapChain; }
		static auto GetContext() -> ID3D11DeviceContext* { return gContext; }
		static auto GetTargetView() -> ID3D11RenderTargetView* const* { return &gTargetView; }

		// D3D12
		static auto GetDeviceD3D12() -> ID3D12Device* { return gDevice12; }
		static auto GetCommandQueue() -> ID3D12CommandQueue* { return gCommandQueue; }

		inline static auto g_Api{RenderAPI::None};
		inline static bool init{false};
		inline static bool pendingWndProc{false};
		inline static HWND hWnd{};
		inline static WNDPROC oldWndProc{};
		inline static std::function<void()> present;
		inline static LRESULT (*wndProc)(HWND, UINT, WPARAM, LPARAM);

		// D3D11
		inline static ID3D11Device* gDevice{};
		inline static IDXGISwapChain* gSwapChain{};
		inline static ID3D11DeviceContext* gContext{};
		inline static ID3D11RenderTargetView* gTargetView{};

		// D3D12
		inline static ID3D12Device* gDevice12{};
		inline static ID3D12CommandQueue* gCommandQueue{};

	private:
		static HRESULT __stdcall MyPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
		{
			if (!init)
			{
				gSwapChain = pSwapChain;

				// Detect API: Try D3D11 Device
				ID3D11Device* d3d11Device = nullptr;
				if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&d3d11Device))))
				{
					g_Api = RenderAPI::D3D11;
					gDevice = d3d11Device;
					gDevice->GetImmediateContext(&gContext);

					DXGI_SWAP_CHAIN_DESC sd;
					SUCCEEDED(pSwapChain->GetDesc(&sd));
					hWnd = sd.OutputWindow;

					ID3D11Texture2D* pBackBuffer = nullptr;
					if (SUCCEEDED(
						pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer))))
					{
						SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView));
						pBackBuffer->Release();
						init = true;
					}
				}
				else
				{
					// Try D3D12 Device
					ID3D12Device* d3d12Device = nullptr;
					if (SUCCEEDED(
						pSwapChain->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&d3d12Device))))
					{
						g_Api = RenderAPI::D3D12;
						gDevice12 = d3d12Device;

						DXGI_SWAP_CHAIN_DESC sd;
						SUCCEEDED(pSwapChain->GetDesc(&sd));
						hWnd = sd.OutputWindow;
						init = true;
					}
				}

				if (init && pendingWndProc && wndProc && hWnd)
				{
					oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
					SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
					pendingWndProc = false;
				}
			}

			if (present && init)
			{
				present();
			}

			return HookManager::Scall(MyPresent, pSwapChain, SyncInterval, Flags);
		}

		static HRESULT __stdcall MyPresent1(IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags,
		                                    const DXGI_PRESENT_PARAMETERS* pPresentParameters)
		{
			if (!init)
			{
				gSwapChain = pSwapChain;

				// Detect API: Try D3D11 Device
				ID3D11Device* d3d11Device = nullptr;
				if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&d3d11Device))))
				{
					g_Api = RenderAPI::D3D11;
					gDevice = d3d11Device;
					gDevice->GetImmediateContext(&gContext);

					DXGI_SWAP_CHAIN_DESC sd;
					SUCCEEDED(pSwapChain->GetDesc(&sd));
					hWnd = sd.OutputWindow;

					ID3D11Texture2D* pBackBuffer = nullptr;
					if (SUCCEEDED(
						pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer))))
					{
						SUCCEEDED(gDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gTargetView));
						pBackBuffer->Release();
						init = true;
					}
				}
				else
				{
					// Try D3D12 Device
					ID3D12Device* d3d12Device = nullptr;
					if (SUCCEEDED(
						pSwapChain->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&d3d12Device))))
					{
						g_Api = RenderAPI::D3D12;
						gDevice12 = d3d12Device;

						DXGI_SWAP_CHAIN_DESC sd;
						SUCCEEDED(pSwapChain->GetDesc(&sd));
						hWnd = sd.OutputWindow;
						init = true;
					}
				}

				if (init && pendingWndProc && wndProc && hWnd)
				{
					oldWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hWnd, GWLP_WNDPROC));
					SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(NewWndProc));
					pendingWndProc = false;
				}
			}

			if (present && init)
			{
				present();
			}

			return HookManager::Scall(MyPresent1, pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
		}

		static void __stdcall MyExecuteCommandLists(ID3D12CommandQueue* pQueue, UINT NumCommandLists,
		                                            ID3D12CommandList* const* ppCommandLists)
		{
			if (!gCommandQueue && pQueue && pQueue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			{
				gCommandQueue = pQueue;
			}
			HookManager::Scall(MyExecuteCommandLists, pQueue, NumCommandLists, ppCommandLists);
		}

		static LRESULT CALLBACK NewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (wndProc)
			{
				switch (const LRESULT result = wndProc(hwnd, uMsg, wParam, lParam))
				{
				case 0: return DefWindowProc(hwnd, uMsg, wParam, lParam);
				case 1: return CallWindowProc(oldWndProc, hwnd, uMsg, wParam, lParam);
				case 2: return 0;
				default: return result;
				}
			}
			return CallWindowProc(oldWndProc, hwnd, uMsg, wParam, lParam);
		}
	};
}
