#pragma once
#include "pch.h"

struct InputForwarder
{
    static void Initialize(HWND overlayHwnd, HWND gameHwnd) noexcept;
    static void Shutdown() noexcept;
    static void Update() noexcept;

    [[nodiscard]] static bool IsMenuTogglePressed() noexcept;
    [[nodiscard]] static bool IsCursorTogglePressed() noexcept;
    [[nodiscard]] static bool IsCursorUnlockPressed() noexcept;

private:
    inline static HWND s_overlayHwnd = nullptr;
    inline static HWND s_gameHwnd = nullptr;
    inline static bool s_prevMenuKeyState = false;
    inline static bool s_prevCursorToggleState = false;
    inline static bool s_prevCursorUnlockState = false;
};