#pragma once

// Defines
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define WINDOWS_MODE 1

// Standard
#include <windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <cstdio>
#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <format>
#include <ranges>
#include <deque>
#include <array>
#include <utility>
#include <excpt.h>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <cctype>

// SDK
#include "unityresolve/UnityResolve.hpp"

// Hook
#include "detours/HookManager.h"
#include "kiero/kiero.h"
#include "d3d11hook/d3d11hook.hpp"

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

// Misc
#include "json/json.hpp"
#include "xorstr/xorstr.hpp"
#include "console/Console.hpp"

// Macros
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define PAD(size) char TOKENPASTE2(padding_, __LINE__) [size]
#define X(str) xorstr_(str)

// Usings
using namespace std::literals::chrono_literals;
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
//using Sprite = UT::Sprite;
using Material = UT::Material;
//using Shader = UT::Shader;

// Proxy exports for version.dll
#pragma comment(linker, "/export:GetFileVersionInfoA=C:\\Windows\\System32\\version.GetFileVersionInfoA")
#pragma comment(linker, "/export:GetFileVersionInfoByHandle=C:\\Windows\\System32\\version.GetFileVersionInfoByHandle")
#pragma comment(linker, "/export:GetFileVersionInfoExA=C:\\Windows\\System32\\version.GetFileVersionInfoExA")
#pragma comment(linker, "/export:GetFileVersionInfoExW=C:\\Windows\\System32\\version.GetFileVersionInfoExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeA")
#pragma comment(linker, "/export:GetFileVersionInfoSizeExA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExA")
#pragma comment(linker, "/export:GetFileVersionInfoSizeExW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeW")
#pragma comment(linker, "/export:GetFileVersionInfoW=C:\\Windows\\System32\\version.GetFileVersionInfoW")
#pragma comment(linker, "/export:VerFindFileA=C:\\Windows\\System32\\version.VerFindFileA")
#pragma comment(linker, "/export:VerFindFileW=C:\\Windows\\System32\\version.VerFindFileW")
#pragma comment(linker, "/export:VerInstallFileA=C:\\Windows\\System32\\version.VerInstallFileA")
#pragma comment(linker, "/export:VerInstallFileW=C:\\Windows\\System32\\version.VerInstallFileW")
#pragma comment(linker, "/export:VerLanguageNameA=C:\\Windows\\System32\\version.VerLanguageNameA")
#pragma comment(linker, "/export:VerLanguageNameW=C:\\Windows\\System32\\version.VerLanguageNameW")
#pragma comment(linker, "/export:VerQueryValueA=C:\\Windows\\System32\\version.VerQueryValueA")
#pragma comment(linker, "/export:VerQueryValueW=C:\\Windows\\System32\\version.VerQueryValueW")
