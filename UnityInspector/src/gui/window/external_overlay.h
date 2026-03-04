#pragma once
#include "pch.h"

struct ExternalOverlay
{
    [[nodiscard]] bool Create(HWND gameHwnd) noexcept;
    void Destroy() noexcept;
    [[nodiscard]] bool IsRunning() const noexcept { return m_running; }
    void Stop() noexcept { m_running = false; }

    [[nodiscard]] HWND GetOverlayHwnd() const noexcept { return m_overlayHwnd; }
    [[nodiscard]] HWND GetGameHwnd() const noexcept { return m_gameHwnd; }
    [[nodiscard]] ID3D11Device* GetDevice() const noexcept { return m_device; }
    [[nodiscard]] ID3D11DeviceContext* GetContext() const noexcept { return m_context; }
    [[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept { return m_swapChain; }
    [[nodiscard]] ID3D11RenderTargetView* const* GetTargetView() const noexcept { return &m_targetView; }

    void RunRenderLoop() noexcept;
    void SetInputCapture(bool capture) noexcept;

private:
    bool m_capturingInput = false;
    [[nodiscard]] bool InitializeD3D11() noexcept;
    void CleanupD3D11() noexcept;
    [[nodiscard]] bool CreateOverlayWindow() noexcept;
    void UpdatePosition() noexcept;
    void Present() noexcept;

    HWND m_gameHwnd = nullptr;
    HWND m_overlayHwnd = nullptr;
    HINSTANCE m_instance = nullptr;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_targetView = nullptr;

    std::atomic<bool> m_running = false;
    std::jthread m_renderThread;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};