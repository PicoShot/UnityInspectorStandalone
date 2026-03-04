#include "pch.h"
#include "input_forwarder.h"

void InputForwarder::Initialize(const HWND overlayHwnd, const HWND gameHwnd) noexcept
{
    s_overlayHwnd = overlayHwnd;
    s_gameHwnd = gameHwnd;
    s_prevMenuKeyState = false;
    s_prevCursorToggleState = false;
    s_prevCursorUnlockState = false;
}

void InputForwarder::Shutdown() noexcept
{
    s_overlayHwnd = nullptr;
    s_gameHwnd = nullptr;
}

void InputForwarder::Update() noexcept
{
}

bool InputForwarder::IsMenuTogglePressed() noexcept
{
    const bool currentState = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    const bool pressed = currentState && !s_prevMenuKeyState;
    s_prevMenuKeyState = currentState;
    return pressed;
}

bool InputForwarder::IsCursorTogglePressed() noexcept
{
    const bool currentState = (GetAsyncKeyState(VK_F5) & 0x8000) != 0;
    const bool pressed = currentState && !s_prevCursorToggleState;
    s_prevCursorToggleState = currentState;
    return pressed;
}

bool InputForwarder::IsCursorUnlockPressed() noexcept
{
    const bool currentState = (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
    const bool pressed = currentState && !s_prevCursorUnlockState;
    s_prevCursorUnlockState = currentState;
    return pressed;
}
