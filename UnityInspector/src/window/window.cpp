#include "pch.h"
#include "config/config.h"
#include "features/features.h"
#include "input.h"
#include "menu/menu.h"
#include "themes.h"
#include "window.h"

namespace Window
{
	static constexpr UINT MaxBuffers = 8;

	static ID3D12Device* g_pd3d12Device = nullptr;
	static ID3D12DescriptorHeap* g_pd3d12DescriptorHeap = nullptr;
	static ID3D12CommandAllocator* g_pCommandAllocators[MaxBuffers] = {};
	static ID3D12GraphicsCommandList* g_pCommandList = nullptr;
	static ID3D12DescriptorHeap* g_pd3d12RtvDescHeap = nullptr;
	static ID3D12Resource* g_mainRenderTargetResource[MaxBuffers] = {};
	static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[MaxBuffers] = {};
	static ID3D12Fence* g_Fence = nullptr;
	static HANDLE g_FenceEvent = nullptr;
	static UINT64 g_FenceValue = 0;
	static UINT64 g_FrameFenceValues[MaxBuffers] = {};
	static UINT g_BufferCount = 0;

	static std::recursive_timed_mutex g_FrameMutex;
	static bool g_FrameReady = false;
	static bool g_UseDX12 = false;
	static std::atomic_bool g_MainThreadSeen = false;

	std::recursive_timed_mutex& FrameMutex()
	{
		return g_FrameMutex;
	}

	void ApplyTheme()
	{
		switch (Config::settings.theme)
		{
			case Theme::Light: ImGui::StyleColorsLight(); break;
			case Theme::Dark: ImGui::StyleColorsDark(); break;
			case Theme::Classic: ImGui::StyleColorsClassic(); break;
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

	static void CreateImGuiContext()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigWindowsResizeFromEdges = false;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
		io.IniFilename = nullptr;
		io.IniSavingRate = 0.f;
		io.LogFilename = nullptr;
		io.MouseDrawCursor = true;
		ApplyTheme();
		Themes::SetImGuiStyle();
	}

	// Caller holds g_FrameMutex.
	static void BuildUi()
	{
		if (g_UseDX12)
			ImGui_ImplDX12_NewFrame();
		else
			ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiErrorRecoveryState recovery;
		ImGui::ErrorRecoveryStoreState(&recovery);
		try
		{
			if (!g_MainThreadUi) Features::Update(ImGui::GetIO().DeltaTime);
			Menu::Render();
			Features::Render();
		}
		catch (...)
		{
			// /EHa: also catches access violations, so one bad object read never takes the game down
			ImGui::ErrorRecoveryTryToRecoverState(&recovery);
		}

		ImGui::Render();
		g_FrameReady = true;
	}

	void BuildFrame()
	{
		g_MainThreadSeen = true;
		if (!g_ImGuiInitialized) return;

		std::scoped_lock lock(g_FrameMutex);
		BuildUi();
	}

	// Render thread: bounded wait, the main thread may be blocked on the render thread inside a Unity call.
	static std::unique_lock<std::recursive_timed_mutex> LockForDraw()
	{
		static int presentsWithoutMainThread = 0;
		if (g_MainThreadUi && !g_MainThreadSeen && ++presentsWithoutMainThread > 600)
		{
			LOG_WARNING("Main-thread hook never fired, building UI on the render thread instead");
			g_MainThreadUi = false;
		}

		std::unique_lock lock(g_FrameMutex, std::chrono::milliseconds(50));
		if (lock && !g_MainThreadUi) BuildUi();
		return lock;
	}

	static void InitializeImGui(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		std::scoped_lock lock(g_FrameMutex);
		CreateImGuiContext();
		g_UseDX12 = false;

		if (ImGui_ImplWin32_Init(hwnd) && ImGui_ImplDX11_Init(device, context))
		{
			UR::ThreadAttach();
			g_ImGuiInitialized = true;
		}
	}

	static void WaitForFence(const UINT64 value)
	{
		if (!g_Fence || g_Fence->GetCompletedValue() >= value) return;
		if (SUCCEEDED(g_Fence->SetEventOnCompletion(value, g_FenceEvent))) WaitForSingleObject(g_FenceEvent, 1000);
	}

	static void ReleaseRenderTargetsD3D12()
	{
		WaitForFence(g_FenceValue);
		for (auto& resource : g_mainRenderTargetResource)
		{
			if (resource)
			{
				resource->Release();
				resource = nullptr;
			}
		}
	}

	static bool CreateRenderTargetsD3D12(IDXGISwapChain3* swapchain)
	{
		DXGI_SWAP_CHAIN_DESC1 desc;
		if (FAILED(swapchain->GetDesc1(&desc))) return false;

		g_BufferCount = std::min(desc.BufferCount, MaxBuffers);
		for (UINT i = 0; i < g_BufferCount; i++)
		{
			if (FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&g_mainRenderTargetResource[i]))))
			{
				ReleaseRenderTargetsD3D12();
				return false;
			}
			g_pd3d12Device->CreateRenderTargetView(g_mainRenderTargetResource[i], nullptr,
			                                       g_mainRenderTargetDescriptor[i]);
		}
		return true;
	}

	static void SrvDescriptorAllocFn(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle,
	                                 D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		*out_cpu_desc_handle = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		*out_gpu_desc_handle = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}

	static void SrvDescriptorFreeFn([[maybe_unused]] ImGui_ImplDX12_InitInfo* info,
	                                [[maybe_unused]] D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle,
	                                [[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
	{
	}

	static bool InitializeD3D12(HWND hwnd, ID3D12Device* device, IDXGISwapChain3* swapchain)
	{
		g_pd3d12Device = device;

		DXGI_SWAP_CHAIN_DESC1 desc;
		if (FAILED(swapchain->GetDesc1(&desc))) return false;

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_pd3d12DescriptorHeap)))) return false;

		// sized for the max so a resize that changes the buffer count never overflows the heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = MaxBuffers;
		if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_pd3d12RtvDescHeap)))) return false;

		const UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3d12RtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < MaxBuffers; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;

			if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			                                          IID_PPV_ARGS(&g_pCommandAllocators[i]))))
				return false;
		}

		if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_pCommandAllocators[0], nullptr,
		                                     IID_PPV_ARGS(&g_pCommandList))) ||
		    FAILED(g_pCommandList->Close()))
			return false;

		if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_Fence)))) return false;
		g_FenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!g_FenceEvent) return false;

		if (!CreateRenderTargetsD3D12(swapchain)) return false;

		std::scoped_lock lock(g_FrameMutex);
		CreateImGuiContext();
		g_UseDX12 = true;

		ImGui_ImplDX12_InitInfo initInfo = {};
		initInfo.Device = device;
		initInfo.CommandQueue = graphics_hook::GH::GetCommandQueue();
		initInfo.NumFramesInFlight = static_cast<int>(MaxBuffers);
		initInfo.RTVFormat = desc.Format;
		initInfo.SrvDescriptorHeap = g_pd3d12DescriptorHeap;
		initInfo.SrvDescriptorAllocFn = SrvDescriptorAllocFn;
		initInfo.SrvDescriptorFreeFn = SrvDescriptorFreeFn;

		if (!ImGui_ImplWin32_Init(hwnd) || !ImGui_ImplDX12_Init(&initInfo)) return false;

		UR::ThreadAttach();
		g_ImGuiInitialized = true;
		return true;
	}

	static void RenderFrameD3D12(IDXGISwapChain3* swapchain)
	{
		if (!g_mainRenderTargetResource[0] && !CreateRenderTargetsD3D12(swapchain)) return;

		const UINT backBufferIdx = swapchain->GetCurrentBackBufferIndex();
		if (backBufferIdx >= g_BufferCount) return;

		const auto lock = LockForDraw();
		if (!lock || !g_FrameReady) return;

		// the allocator may still be in use by the GPU from the last time this back buffer was drawn
		WaitForFence(g_FrameFenceValues[backBufferIdx]);

		ID3D12CommandAllocator* commandAllocator = g_pCommandAllocators[backBufferIdx];
		if (FAILED(commandAllocator->Reset()) || FAILED(g_pCommandList->Reset(commandAllocator, nullptr))) return;

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		g_pCommandList->ResourceBarrier(1, &barrier);

		g_pCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
		ID3D12DescriptorHeap* heaps[] = {g_pd3d12DescriptorHeap};
		g_pCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pCommandList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		g_pCommandList->ResourceBarrier(1, &barrier);
		if (FAILED(g_pCommandList->Close())) return;

		auto* queue = graphics_hook::GH::GetCommandQueue();
		ID3D12CommandList* const commandLists[] = {g_pCommandList};
		queue->ExecuteCommandLists(1, commandLists);
		if (SUCCEEDED(queue->Signal(g_Fence, ++g_FenceValue))) g_FrameFenceValues[backBufferIdx] = g_FenceValue;
	}

	static void RenderFrameD3D11(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetView)
	{
		const auto lock = LockForDraw();
		if (!lock || !g_FrameReady) return;

		context->OMSetRenderTargets(1, &targetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void OnPresent()
	{
		if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D11)
		{
			const auto device = graphics_hook::GH::GetDeviceD3D11();
			const auto context = graphics_hook::GH::GetContext();
			const auto targetView = *graphics_hook::GH::GetTargetView();

			if (!device || !context || !targetView) return;

			if (!g_ImGuiInitialized) InitializeImGui(graphics_hook::GH::GetHwnd(), device, context);

			RenderFrameD3D11(context, targetView);
		}
		else if (graphics_hook::GH::GetApi() == graphics_hook::RenderAPI::D3D12)
		{
			const auto swapchain = static_cast<IDXGISwapChain3*>(graphics_hook::GH::GetSwapChain());
			const auto device = graphics_hook::GH::GetDeviceD3D12();

			if (const auto queue = graphics_hook::GH::GetCommandQueue(); !swapchain || !device || !queue) return;

			if (!g_ImGuiInitialized)
			{
				// shortcut: a failed init is retried every frame and leaks the partial objects; fine for a one-shot
				// path
				if (!InitializeD3D12(graphics_hook::GH::GetHwnd(), device, swapchain)) return;
			}

			RenderFrameD3D12(swapchain);
		}
	}

	void OnResize()
	{
		if (g_UseDX12) ReleaseRenderTargetsD3D12();
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
		if (!g_ImGuiInitialized) return 1;

		std::scoped_lock lock(g_FrameMutex);
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		if (Input::ProcessMessage(uMsg, wParam)) return 2;

		if (const ImGuiIO& io = ImGui::GetIO(); io.WantCaptureMouse || io.WantCaptureMouseUnlessPopupClose)
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
				case WM_NCMOUSEMOVE: return 2;
			}
		}
		return 1;
	}
}
