#include "pch.h"
#include "external_overlay.h"
#include "window.h"
#include "input_forwarder.h"

bool ExternalOverlay::Create(const HWND gameHwnd) noexcept
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

void ExternalOverlay::Destroy() noexcept
{
    Stop();

    if (m_renderThread.joinable())
        m_renderThread.join();

    CleanupD3D11();

    if (m_overlayHwnd) {
        DestroyWindow(m_overlayHwnd);
        m_overlayHwnd = nullptr;
    }
}

bool ExternalOverlay::CreateOverlayWindow() noexcept
{
    const wchar_t className[] = L"UnityInspectorOverlay";

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

    SetLayeredWindowAttributes(m_overlayHwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);

    ShowWindow(m_overlayHwnd, SW_SHOW);
    UpdateWindow(m_overlayHwnd);

    return true;
}

bool ExternalOverlay::InitializeD3D11() noexcept
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
        CleanupD3D11();
        return false;
    }

    const HRESULT rtResult = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_targetView);
    backBuffer->Release();

    if (FAILED(rtResult)) {
        CleanupD3D11();
        return false;
    }

    return true;
}

void ExternalOverlay::CleanupD3D11() noexcept
{
    if (m_targetView) 
    {
        m_targetView->Release();
        m_targetView = nullptr;
    }
    if (m_swapChain) 
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
    if (m_context) 
    {
        m_context->Release();
        m_context = nullptr;
    }
    if (m_device) 
    {
        m_device->Release();
        m_device = nullptr;
    }
}

void ExternalOverlay::RunRenderLoop() noexcept
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

        std::this_thread::sleep_for(5ms);
    }
}

void ExternalOverlay::UpdatePosition() noexcept
{
    if (!IsWindow(m_gameHwnd)) return;

    RECT gameRect;
    GetWindowRect(m_gameHwnd, &gameRect);

    RECT overlayRect;
    GetWindowRect(m_overlayHwnd, &overlayRect);

    const int gameWidth = gameRect.right - gameRect.left;
    const int gameHeight = gameRect.bottom - gameRect.top;
    const int overlayWidth = overlayRect.right - overlayRect.left;
    const int overlayHeight = overlayRect.bottom - overlayRect.top;

    if (gameRect.left != overlayRect.left || gameRect.top != overlayRect.top ||
        gameWidth != overlayWidth || gameHeight != overlayHeight) {

        SetWindowPos(
            m_overlayHwnd,
            HWND_TOPMOST,
            gameRect.left,
            gameRect.top,
            gameWidth,
            gameHeight,
            SWP_NOACTIVATE | SWP_SHOWWINDOW
        );

        if (m_swapChain && (gameWidth != overlayWidth || gameHeight != overlayHeight)) 
        {
            m_targetView->Release();
            m_targetView = nullptr;

            if (SUCCEEDED(m_swapChain->ResizeBuffers(0, gameWidth, gameHeight, DXGI_FORMAT_UNKNOWN, 0))) 
            {
                ID3D11Texture2D* backBuffer = nullptr;
                if (SUCCEEDED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
                    m_device->CreateRenderTargetView(backBuffer, nullptr, &m_targetView);
                    backBuffer->Release();
                }
            }
        }
    }
}

void ExternalOverlay::Present() noexcept
{
    if (!m_device || !m_context || !m_targetView) return;

    if (m_capturingInput && Window::IsImGuiInitialized()) 
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

    constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->ClearRenderTargetView(m_targetView, clearColor);

    Window::RenderToExternalOverlay(m_device, m_context, m_targetView, m_overlayHwnd);

    m_swapChain->Present(1, 0);
}


void ExternalOverlay::SetInputCapture(const bool capture) noexcept
{
    if (!m_overlayHwnd || m_capturingInput == capture) return;

    m_capturingInput = capture;

    if (capture) 
    {
        EnableWindow(m_gameHwnd, FALSE);

        SetWindowLongPtr(m_overlayHwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);
        SetWindowPos(m_overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        RECT rect;
        GetClientRect(m_overlayHwnd, &rect);
        const HRGN hRgn = CreateRectRgn(0, 0, rect.right - rect.left, rect.bottom - rect.top);
        SetWindowRgn(m_overlayHwnd, hRgn, TRUE);

        const DWORD foregroundThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
        const DWORD overlayThread = GetCurrentThreadId();

        AttachThreadInput(foregroundThread, overlayThread, TRUE);
        BringWindowToTop(m_overlayHwnd);
        SetForegroundWindow(m_overlayHwnd);
        SetActiveWindow(m_overlayHwnd);
        SetFocus(m_overlayHwnd);
        AttachThreadInput(foregroundThread, overlayThread, FALSE);

        while (ShowCursor(TRUE) < 0);
    } 
    else 
    {
        EnableWindow(m_gameHwnd, TRUE);
        SetWindowRgn(m_overlayHwnd, nullptr, TRUE);

        SetWindowLongPtr(m_overlayHwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
        SetWindowPos(m_overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        const DWORD foregroundThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
        const DWORD gameThread = GetWindowThreadProcessId(m_gameHwnd, nullptr);

        AttachThreadInput(foregroundThread, gameThread, TRUE);
        SetForegroundWindow(m_gameHwnd);
        SetActiveWindow(m_gameHwnd);
        AttachThreadInput(foregroundThread, gameThread, FALSE);

        while (ShowCursor(FALSE) >= 0);
    }
}

LRESULT CALLBACK ExternalOverlay::WndProc(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
    if (Window::IsImGuiInitialized()) 
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
