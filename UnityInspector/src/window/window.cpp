#include "pch.h"
#include "window.h"
#include "features/features.h"
#include "menu/menu.h"
#include "input.h"
#include "config/config.h"
#include "themes.h"

namespace Window
{
	static ID3D12Device* g_pd3d12Device = nullptr;
	static ID3D12DescriptorHeap* g_pd3d12DescriptorHeap = nullptr;
	static ID3D12CommandAllocator* g_pCommandAllocators[8] = {};
	static ID3D12GraphicsCommandList* g_pCommandList = nullptr;
	static ID3D12DescriptorHeap* g_pd3d12RtvDescHeap = nullptr;
	static ID3D12Resource* g_mainRenderTargetResource[8] = {};
	static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[8] = {};
	static UINT g_BufferCount = 0;
	static DXGI_FORMAT g_RtvFormat = DXGI_FORMAT_UNKNOWN;
	static UINT g_Width = 0;
	static UINT g_Height = 0;

	void ApplyTheme()
	{
		switch (Config::settings.theme)
		{
		case Theme::Light:
			ImGui::StyleColorsLight();
			break;
		case Theme::Dark:
			ImGui::StyleColorsDark();
			break;
		case Theme::Classic:
			ImGui::StyleColorsClassic();
			break;
		case Theme::WhitePlus:
			ImGui::StyleColorsLight();
			Themes::SetWhitePlusTheme();
			break;
		case Theme::DarkPlus:
		default:
			ImGui::StyleColorsDark();
			Themes::SetDarkPlusTheme();
			break;
		}
	}

	void InitializeImGui(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigWindowsResizeFromEdges = false;
		const auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
		io.FontDefault = font;
		io.IniFilename = nullptr;
		io.IniSavingRate = 0.f;
		io.LogFilename = nullptr;
		io.MouseDrawCursor = true;
		ApplyTheme();
		Themes::SetImGuiStyle();

		if (ImGui_ImplWin32_Init(hwnd) && ImGui_ImplDX11_Init(device, context))
		{
			UR::ThreadAttach();
			g_ImGuiInitialized = true;
		}
	}

	void CleanupD3D12()
	{
		if (g_ImGuiInitialized)
		{
			ImGui_ImplDX12_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			g_ImGuiInitialized = false;
		}

		if (g_pCommandList) { g_pCommandList->Release(); g_pCommandList = nullptr; }
		for (UINT i = 0; i < g_BufferCount; i++)
		{
			if (g_pCommandAllocators[i]) { g_pCommandAllocators[i]->Release(); g_pCommandAllocators[i] = nullptr; }
			if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
		}
		if (g_pd3d12DescriptorHeap) { g_pd3d12DescriptorHeap->Release(); g_pd3d12DescriptorHeap = nullptr; }
		if (g_pd3d12RtvDescHeap) { g_pd3d12RtvDescHeap->Release(); g_pd3d12RtvDescHeap = nullptr; }
		g_BufferCount = 0;
		g_pd3d12Device = nullptr;
	}

	static void SrvDescriptorAllocFn(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		*out_cpu_desc_handle = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		*out_gpu_desc_handle = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}

	static void SrvDescriptorFreeFn([[maybe_unused]] ImGui_ImplDX12_InitInfo* info,[[maybe_unused]] D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle,[[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
	{
	}

	bool InitializeD3D12(HWND hwnd, ID3D12Device* device, IDXGISwapChain3* swapchain)
	{
		g_pd3d12Device = device;

		DXGI_SWAP_CHAIN_DESC1 desc;
		if (FAILED(swapchain->GetDesc1(&desc)))
			return false;

		g_BufferCount = desc.BufferCount;
		g_RtvFormat = desc.Format;
		g_Width = desc.Width;
		g_Height = desc.Height;

		{
			D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
			heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heap_desc.NumDescriptors = g_BufferCount;
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (FAILED(g_pd3d12Device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&g_pd3d12DescriptorHeap))))
				return false;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
			heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heap_desc.NumDescriptors = g_BufferCount;
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			if (FAILED(g_pd3d12Device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&g_pd3d12RtvDescHeap))))
				return false;
		}

		UINT rtvDescriptorSize = g_pd3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3d12RtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < g_BufferCount; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;

			if (FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&g_mainRenderTargetResource[i]))))
				return false;

			g_pd3d12Device->CreateRenderTargetView(g_mainRenderTargetResource[i], nullptr, g_mainRenderTargetDescriptor[i]);

			if (FAILED(g_pd3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pCommandAllocators[i]))))
				return false;
		}

		if (FAILED(g_pd3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_pCommandAllocators[0], nullptr, IID_PPV_ARGS(&g_pCommandList))))
			return false;

		if (FAILED(g_pCommandList->Close()))
			return false;

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigWindowsResizeFromEdges = false;
		const auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
		io.FontDefault = font;
		io.IniFilename = nullptr;
		io.IniSavingRate = 0.f;
		io.LogFilename = nullptr;
		io.MouseDrawCursor = true;
		ApplyTheme();
		Themes::SetImGuiStyle();

		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device = g_pd3d12Device;
		init_info.CommandQueue = graphics_hook::GH::GetCommandQueue();
		init_info.NumFramesInFlight = g_BufferCount;
		init_info.RTVFormat = g_RtvFormat;
		init_info.SrvDescriptorHeap = g_pd3d12DescriptorHeap;
		init_info.SrvDescriptorAllocFn = SrvDescriptorAllocFn;
		init_info.SrvDescriptorFreeFn = SrvDescriptorFreeFn;

		if (ImGui_ImplWin32_Init(hwnd) && ImGui_ImplDX12_Init(&init_info))
		{
			UR::ThreadAttach();
			g_ImGuiInitialized = true;
			return true;
		}

		return false;
	}

	void CheckResizeD3D12(IDXGISwapChain3* swapchain)
	{
		DXGI_SWAP_CHAIN_DESC1 desc;
		if (SUCCEEDED(swapchain->GetDesc1(&desc)))
		{
			if (desc.Width != g_Width || desc.Height != g_Height)
			{
				CleanupD3D12();
				InitializeD3D12(graphics_hook::GH::GetHwnd(), graphics_hook::GH::GetDeviceD3D12(), swapchain);
			}
		}
	}

	void RenderFrameD3D12(IDXGISwapChain3* swapchain)
	{
		CheckResizeD3D12(swapchain);

		const UINT backBufferIdx = swapchain->GetCurrentBackBufferIndex();
		ID3D12CommandAllocator* commandAllocator = g_pCommandAllocators[backBufferIdx];
		SUCCEEDED(commandAllocator->Reset());

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		SUCCEEDED(g_pCommandList->Reset(commandAllocator, nullptr));
		g_pCommandList->ResourceBarrier(1, &barrier);

		g_pCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);

		ID3D12DescriptorHeap* heaps[] = { g_pd3d12DescriptorHeap };
		g_pCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Features::Update(ImGui::GetIO().DeltaTime);
		Menu::Render();
		Features::Render();

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pCommandList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		g_pCommandList->ResourceBarrier(1, &barrier);

		SUCCEEDED(g_pCommandList->Close());

		ID3D12CommandList* const commandLists[] = { g_pCommandList };
		graphics_hook::GH::GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	}

	void ShutdownImGui()
	{
		if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D11)
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			g_ImGuiInitialized = false;
		}
		else if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D12)
		{
			CleanupD3D12();
		}
	}

	void RenderFrameD3D11(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView)
	{
		if (!context) return;
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Features::Update(ImGui::GetIO().DeltaTime);
		Menu::Render();
		Features::Render();

		ImGui::Render();

		context->OMSetRenderTargets(1, &targetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void OnPresent()
	{
		if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D11)
		{
			const auto device = graphics_hook::GH::GetDeviceD3D11();
			const auto context = graphics_hook::GH::GetContext();
			const auto targetView = graphics_hook::GH::GetTargetView();

			if (!device || !context || !*targetView) return;

			if (!g_ImGuiInitialized) InitializeImGui(graphics_hook::GH::GetHwnd(), device, context);

			RenderFrameD3D11(context, *targetView);
		}
		else if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D12)
		{
			const auto swapchain = static_cast<IDXGISwapChain3*>(graphics_hook::GH::GetSwapChain());
			const auto device = graphics_hook::GH::GetDeviceD3D12();

			if (const auto queue = graphics_hook::GH::GetCommandQueue(); !swapchain || !device || !queue) return;

			if (!g_ImGuiInitialized) InitializeD3D12(graphics_hook::GH::GetHwnd(), device, swapchain);

			RenderFrameD3D12(swapchain);
		}
	}

	void RenderToExternalOverlay(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView,
	                             const HWND hwnd)
	{
		if (!g_ImGuiInitialized) InitializeImGui(hwnd, device, context);

		RenderFrameD3D11(context, targetView);
	}

	void UpdateExternalInput()
	{
		if (!g_ImGuiInitialized) return;

		Input::ProcessExternal();
	}

	LRESULT MyWndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		if (g_ImGuiInitialized)
		{
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

			if (Input::ProcessMessage(uMsg, wParam))
				return 2;

			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureMouseUnlessPopupClose)
			{
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP:
				case WM_MOUSEWHEEL:
				case WM_MOUSEMOVE:
				case WM_MOUSELEAVE:
				case WM_MOUSEACTIVATE:
				case WM_MOUSEHOVER:
				case WM_MOUSELAST:
				case WM_NCMOUSEHOVER:
				case WM_NCMOUSELEAVE:
				case WM_NCMOUSEMOVE:
					return 2;
				}
			}
		}
		return 1;
	}
}
