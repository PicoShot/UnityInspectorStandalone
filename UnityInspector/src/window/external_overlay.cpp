#include "pch.h"
#include "external_overlay.h"
#include "window.h"

namespace ExternalOverlay
{
	bool m_capturingInput = false;
	HWND m_gameHwnd = nullptr;
	HWND m_overlayHwnd = nullptr;
	HINSTANCE m_instance = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_targetView = nullptr;
	std::atomic m_running = false;

	LRESULT CALLBACK WndProc(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
	{
		if (Window::g_ImGuiInitialized)
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return 0;
		}

		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MOUSEACTIVATE:
			return MA_ACTIVATE;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	bool CreateOverlayWindow()
	{
		constexpr wchar_t className[] = L"UnityInspectorOverlay";

		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = m_instance;
		wc.lpszClassName = className;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

		if (!RegisterClassExW(&wc))
			return false;

		RECT gameRect;
		GetWindowRect(m_gameHwnd, &gameRect);

		m_overlayHwnd = CreateWindowExW(
			WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
			className,
			L"UnityInspector",
			WS_POPUP,
			gameRect.left,
			gameRect.top,
			gameRect.right - gameRect.left,
			gameRect.bottom - gameRect.top,
			nullptr,
			nullptr,
			m_instance,
			nullptr
		);

		if (!m_overlayHwnd) return false;

		SetLayeredWindowAttributes(m_overlayHwnd, 0, 255, LWA_ALPHA);

		MARGINS margins = { -1, -1, -1, -1 };
		if (!SUCCEEDED(DwmExtendFrameIntoClientArea(m_overlayHwnd, &margins))) return false;

		ShowWindow(m_overlayHwnd, SW_SHOW);
		UpdateWindow(m_overlayHwnd);

		return true;
	}

	bool InitializeD3D11()
	{
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = m_overlayHwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		const HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			&sd,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_context
		);

		if (FAILED(hr)) return false;

		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
		{
			return false;
		}

		const HRESULT rtResult = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_targetView);
		backBuffer->Release();

		if (FAILED(rtResult))
		{
			return false;
		}

		return true;
	}

	bool Create(const HWND gameHwnd)
	{
		if (!gameHwnd || !IsWindow(gameHwnd)) return false;

		m_gameHwnd = gameHwnd;
		m_instance = GetModuleHandle(nullptr);

		if (!CreateOverlayWindow()) return false;

		if (!InitializeD3D11())
		{
			DestroyWindow(m_overlayHwnd);
			m_overlayHwnd = nullptr;
			return false;
		}

		m_running = true;
		return true;
	}

	void UpdatePosition()
	{
		if (!IsWindow(m_gameHwnd)) return;

		const HWND foregroundWnd = GetForegroundWindow();
		const bool isGameActive = (foregroundWnd == m_gameHwnd || foregroundWnd == m_overlayHwnd);

		if (const bool isMinimized = IsIconic(m_gameHwnd); isMinimized || !isGameActive)
		{
			if (IsWindowVisible(m_overlayHwnd))
			{
				ShowWindow(m_overlayHwnd, SW_HIDE);
			}
			return;
		}
		if (!IsWindowVisible(m_overlayHwnd))
		{
			ShowWindow(m_overlayHwnd, SW_SHOW);
		}

		RECT clientRect;
		if (!GetClientRect(m_gameHwnd, &clientRect)) return;

		POINT topLeft = { clientRect.left, clientRect.top };
		ClientToScreen(m_gameHwnd, &topLeft);

		const int clientWidth = clientRect.right - clientRect.left;
		const int clientHeight = clientRect.bottom - clientRect.top;

		RECT overlayRect;
		GetWindowRect(m_overlayHwnd, &overlayRect);
		const int overlayWidth = overlayRect.right - overlayRect.left;

		if (const int overlayHeight = overlayRect.bottom - overlayRect.top; topLeft.x != overlayRect.left || topLeft.y != overlayRect.top ||
			clientWidth != overlayWidth || clientHeight != overlayHeight)
		{
			SetWindowPos(
				m_overlayHwnd,
				HWND_TOPMOST,
				topLeft.x,
				topLeft.y,
				clientWidth,
				clientHeight,
				SWP_NOACTIVATE | SWP_SHOWWINDOW
			);

			if (m_swapChain && (clientWidth != overlayWidth || clientHeight != overlayHeight))
			{
				m_targetView->Release();
				m_targetView = nullptr;

				if (SUCCEEDED(m_swapChain->ResizeBuffers(0, clientWidth, clientHeight, DXGI_FORMAT_UNKNOWN, 0)))
				{
					ID3D11Texture2D* backBuffer = nullptr;
					if (SUCCEEDED(
						m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
					{
						SUCCEEDED(m_device->CreateRenderTargetView(backBuffer, nullptr, &m_targetView));
						backBuffer->Release();
					}
				}
			}
		}
	}

	void Present()
	{
		if (!m_device || !m_context || !m_targetView) return;

		if (m_capturingInput && Window::g_ImGuiInitialized)
		{
			ImGuiIO& io = ImGui::GetIO();

			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(m_overlayHwnd, &pos);
			io.MousePos = ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y));

			io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
			io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
			io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
		}

		Window::UpdateExternalInput();

		constexpr float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		m_context->ClearRenderTargetView(m_targetView, clearColor);

		Window::RenderToExternalOverlay(m_device, m_context, m_targetView, m_overlayHwnd);

		SUCCEEDED(m_swapChain->Present(1, 0));
	}

	void RunRenderLoop()
	{
		while (m_running)
		{
			if (!IsWindow(m_gameHwnd))
			{
				m_running = false;
				break;
			}

			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			UpdatePosition();
			Present();

			Sleep(5);
		}
	}

	void SetInputCapture(bool capture)
	{
		if (!m_overlayHwnd || m_capturingInput == capture) return;

		m_capturingInput = capture;

		if (capture)
		{
			SetWindowLongPtr(m_overlayHwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);
			SetWindowPos(m_overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0,
			             SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			BringWindowToTop(m_overlayHwnd);
			SetForegroundWindow(m_overlayHwnd);
			SetActiveWindow(m_overlayHwnd);
			SetFocus(m_overlayHwnd);

			while (ShowCursor(TRUE) < 0);
		}
		else
		{
			SetWindowLongPtr(m_overlayHwnd, GWL_EXSTYLE,
			                 WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
			SetWindowPos(m_overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0,
			             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			SetForegroundWindow(m_gameHwnd);
			SetActiveWindow(m_gameHwnd);

			while (ShowCursor(FALSE) >= 0);
		}
	}
}
