#pragma once

// Defines
#define VERSION "0.6.8"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define WINDOWS_MODE 1
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// Windows
#include <dwmapi.h>
#include <excpt.h>
#include <shellapi.h>
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

// std
#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <format>
#include <future>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <ranges>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Graphics
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// Unity SDK
#include "unityresolve/UnityResolve.hpp"

// Hook
#include "graphics_hook/graphics_hook.hpp"
#include "safetyhook/HookManager.h"

// ImGui
#include "imgui/TextEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

// Misc
#include "console/Console.hpp"
#include "ini/inicpp.h"
#include "json/json.hpp"
#include "proxy.h"
#include "xorstr/xorstr.hpp"

// Lua
#include "sol/sol.hpp"
extern "C"
{
#include "luajit/luajit.h"
}

// Macros
#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define PAD(size) char TOKENPASTE2(padding_, __LINE__)[size]
#define X(str) xorstr_(str)

// Usings
using Json = nlohmann::json;
using UR = UnityResolve;
using UT = UR::UnityType;
using Vec2 = UT::Vector2;
using Vec3 = UT::Vector3;
using Vec4 = UT::Vector4;
using Quat = UT::Quaternion;
using Mat4 = UT::Matrix4x4;
using Object = UT::Object;
using UnityObject = UT::UnityObject;
using Renderer = UT::Renderer;
using Color = UT::Color;
using Animator = UT::Animator;
using Transform = UT::Transform;
using GameObject = UT::GameObject;
using Component = UT::Component;
using Physics = UT::Physics;
using MonoBehaviour = UT::MonoBehaviour;
using Camera = UT::Camera;
using Rigidbody = UT::Rigidbody;
using Collider = UT::Collider;
using Mesh = UT::Mesh;
using Light = UT::Light;
using Ray = UT::Ray;
using RaycastHit = UT::RaycastHit;
using Texture = UT::Texture;
using Texture2D = UT::Texture2D;
using Sprite = UT::Sprite;
using Material = UT::Material;
using Shader = UT::Shader;
