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
using Sprite = UT::Sprite;
using Material = UT::Material;
using Shader = UT::Shader;

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


// Proxy exports for winhttp.dll
#pragma comment(linker, "/export:DllCanUnloadNow=C:\\Windows\\System32\\winhttp.DllCanUnloadNow")
#pragma comment(linker, "/export:DllGetClassObject=C:\\Windows\\System32\\winhttp.DllGetClassObject")
#pragma comment(linker, "/export:Private1=C:\\Windows\\System32\\winhttp.Private1")
#pragma comment(linker, "/export:SvchostPushServiceGlobals=C:\\Windows\\System32\\winhttp.SvchostPushServiceGlobals")
#pragma comment(linker, "/export:WinHttpAddRequestHeaders=C:\\Windows\\System32\\winhttp.WinHttpAddRequestHeaders")
#pragma comment(linker, "/export:WinHttpAddRequestHeadersEx=C:\\Windows\\System32\\winhttp.WinHttpAddRequestHeadersEx")
#pragma comment(linker, "/export:WinHttpAutoProxySvcMain=C:\\Windows\\System32\\winhttp.WinHttpAutoProxySvcMain")
#pragma comment(linker, "/export:WinHttpCheckPlatform=C:\\Windows\\System32\\winhttp.WinHttpCheckPlatform")
#pragma comment(linker, "/export:WinHttpCloseHandle=C:\\Windows\\System32\\winhttp.WinHttpCloseHandle")
#pragma comment(linker, "/export:WinHttpConnect=C:\\Windows\\System32\\winhttp.WinHttpConnect")
#pragma comment(linker, "/export:WinHttpConnectionDeletePolicyEntries=C:\\Windows\\System32\\winhttp.WinHttpConnectionDeletePolicyEntries")
#pragma comment(linker, "/export:WinHttpConnectionDeleteProxyInfo=C:\\Windows\\System32\\winhttp.WinHttpConnectionDeleteProxyInfo")
#pragma comment(linker, "/export:WinHttpConnectionFreeNameList=C:\\Windows\\System32\\winhttp.WinHttpConnectionFreeNameList")
#pragma comment(linker, "/export:WinHttpConnectionFreeProxyInfo=C:\\Windows\\System32\\winhttp.WinHttpConnectionFreeProxyInfo")
#pragma comment(linker, "/export:WinHttpConnectionFreeProxyList=C:\\Windows\\System32\\winhttp.WinHttpConnectionFreeProxyList")
#pragma comment(linker, "/export:WinHttpConnectionGetNameList=C:\\Windows\\System32\\winhttp.WinHttpConnectionGetNameList")
#pragma comment(linker, "/export:WinHttpConnectionGetProxyInfo=C:\\Windows\\System32\\winhttp.WinHttpConnectionGetProxyInfo")
#pragma comment(linker, "/export:WinHttpConnectionGetProxyList=C:\\Windows\\System32\\winhttp.WinHttpConnectionGetProxyList")
#pragma comment(linker, "/export:WinHttpConnectionOnlyConvert=C:\\Windows\\System32\\winhttp.WinHttpConnectionOnlyConvert")
#pragma comment(linker, "/export:WinHttpConnectionOnlyReceive=C:\\Windows\\System32\\winhttp.WinHttpConnectionOnlyReceive")
#pragma comment(linker, "/export:WinHttpConnectionOnlySend=C:\\Windows\\System32\\winhttp.WinHttpConnectionOnlySend")
#pragma comment(linker, "/export:WinHttpConnectionSetPolicyEntries=C:\\Windows\\System32\\winhttp.WinHttpConnectionSetPolicyEntries")
#pragma comment(linker, "/export:WinHttpConnectionSetProxyInfo=C:\\Windows\\System32\\winhttp.WinHttpConnectionSetProxyInfo")
#pragma comment(linker, "/export:WinHttpConnectionUpdateIfIndexTable=C:\\Windows\\System32\\winhttp.WinHttpConnectionUpdateIfIndexTable")
#pragma comment(linker, "/export:WinHttpCrackUrl=C:\\Windows\\System32\\winhttp.WinHttpCrackUrl")
#pragma comment(linker, "/export:WinHttpCreateProxyList=C:\\Windows\\System32\\winhttp.WinHttpCreateProxyList")
#pragma comment(linker, "/export:WinHttpCreateProxyManager=C:\\Windows\\System32\\winhttp.WinHttpCreateProxyManager")
#pragma comment(linker, "/export:WinHttpCreateProxyResolver=C:\\Windows\\System32\\winhttp.WinHttpCreateProxyResolver")
#pragma comment(linker, "/export:WinHttpCreateProxyResult=C:\\Windows\\System32\\winhttp.WinHttpCreateProxyResult")
#pragma comment(linker, "/export:WinHttpCreateUiCompatibleProxyString=C:\\Windows\\System32\\winhttp.WinHttpCreateUiCompatibleProxyString")
#pragma comment(linker, "/export:WinHttpCreateUrl=C:\\Windows\\System32\\winhttp.WinHttpCreateUrl")
#pragma comment(linker, "/export:WinHttpDetectAutoProxyConfigUrl=C:\\Windows\\System32\\winhttp.WinHttpDetectAutoProxyConfigUrl")
#pragma comment(linker, "/export:WinHttpFreeProxyResult=C:\\Windows\\System32\\winhttp.WinHttpFreeProxyResult")
#pragma comment(linker, "/export:WinHttpFreeProxyResultEx=C:\\Windows\\System32\\winhttp.WinHttpFreeProxyResultEx")
#pragma comment(linker, "/export:WinHttpFreeProxySettings=C:\\Windows\\System32\\winhttp.WinHttpFreeProxySettings")
#pragma comment(linker, "/export:WinHttpFreeProxySettingsEx=C:\\Windows\\System32\\winhttp.WinHttpFreeProxySettingsEx")
#pragma comment(linker, "/export:WinHttpFreeQueryConnectionGroupResult=C:\\Windows\\System32\\winhttp.WinHttpFreeQueryConnectionGroupResult")
#pragma comment(linker, "/export:WinHttpGetDefaultProxyConfiguration=C:\\Windows\\System32\\winhttp.WinHttpGetDefaultProxyConfiguration")
#pragma comment(linker, "/export:WinHttpGetIEProxyConfigForCurrentUser=C:\\Windows\\System32\\winhttp.WinHttpGetIEProxyConfigForCurrentUser")
#pragma comment(linker, "/export:WinHttpGetProxyForUrl=C:\\Windows\\System32\\winhttp.WinHttpGetProxyForUrl")
#pragma comment(linker, "/export:WinHttpGetProxyForUrlEx=C:\\Windows\\System32\\winhttp.WinHttpGetProxyForUrlEx")
#pragma comment(linker, "/export:WinHttpGetProxyForUrlEx2=C:\\Windows\\System32\\winhttp.WinHttpGetProxyForUrlEx2")
#pragma comment(linker, "/export:WinHttpGetProxyForUrlHvsi=C:\\Windows\\System32\\winhttp.WinHttpGetProxyForUrlHvsi")
#pragma comment(linker, "/export:WinHttpGetProxyResult=C:\\Windows\\System32\\winhttp.WinHttpGetProxyResult")
#pragma comment(linker, "/export:WinHttpGetProxyResultEx=C:\\Windows\\System32\\winhttp.WinHttpGetProxyResultEx")
#pragma comment(linker, "/export:WinHttpGetProxySettingsEx=C:\\Windows\\System32\\winhttp.WinHttpGetProxySettingsEx")
#pragma comment(linker, "/export:WinHttpGetProxySettingsResultEx=C:\\Windows\\System32\\winhttp.WinHttpGetProxySettingsResultEx")
#pragma comment(linker, "/export:WinHttpGetProxySettingsVersion=C:\\Windows\\System32\\winhttp.WinHttpGetProxySettingsVersion")
#pragma comment(linker, "/export:WinHttpGetTunnelSocket=C:\\Windows\\System32\\winhttp.WinHttpGetTunnelSocket")
#pragma comment(linker, "/export:WinHttpOpen=C:\\Windows\\System32\\winhttp.WinHttpOpen")
#pragma comment(linker, "/export:WinHttpOpenRequest=C:\\Windows\\System32\\winhttp.WinHttpOpenRequest")
#pragma comment(linker, "/export:WinHttpPacJsWorkerMain=C:\\Windows\\System32\\winhttp.WinHttpPacJsWorkerMain")
#pragma comment(linker, "/export:WinHttpProbeConnectivity=C:\\Windows\\System32\\winhttp.WinHttpProbeConnectivity")
#pragma comment(linker, "/export:WinHttpProtocolCompleteUpgrade=C:\\Windows\\System32\\winhttp.WinHttpProtocolCompleteUpgrade")
#pragma comment(linker, "/export:WinHttpProtocolReceive=C:\\Windows\\System32\\winhttp.WinHttpProtocolReceive")
#pragma comment(linker, "/export:WinHttpProtocolSend=C:\\Windows\\System32\\winhttp.WinHttpProtocolSend")
#pragma comment(linker, "/export:WinHttpQueryAuthSchemes=C:\\Windows\\System32\\winhttp.WinHttpQueryAuthSchemes")
#pragma comment(linker, "/export:WinHttpQueryConnectionGroup=C:\\Windows\\System32\\winhttp.WinHttpQueryConnectionGroup")
#pragma comment(linker, "/export:WinHttpQueryDataAvailable=C:\\Windows\\System32\\winhttp.WinHttpQueryDataAvailable")
#pragma comment(linker, "/export:WinHttpQueryHeaders=C:\\Windows\\System32\\winhttp.WinHttpQueryHeaders")
#pragma comment(linker, "/export:WinHttpQueryHeadersEx=C:\\Windows\\System32\\winhttp.WinHttpQueryHeadersEx")
#pragma comment(linker, "/export:WinHttpQueryOption=C:\\Windows\\System32\\winhttp.WinHttpQueryOption")
#pragma comment(linker, "/export:WinHttpReadData=C:\\Windows\\System32\\winhttp.WinHttpReadData")
#pragma comment(linker, "/export:WinHttpReadDataEx=C:\\Windows\\System32\\winhttp.WinHttpReadDataEx")
#pragma comment(linker, "/export:WinHttpReadProxySettings=C:\\Windows\\System32\\winhttp.WinHttpReadProxySettings")
#pragma comment(linker, "/export:WinHttpReadProxySettingsHvsi=C:\\Windows\\System32\\winhttp.WinHttpReadProxySettingsHvsi")
#pragma comment(linker, "/export:WinHttpReceiveResponse=C:\\Windows\\System32\\winhttp.WinHttpReceiveResponse")
#pragma comment(linker, "/export:WinHttpRefreshProxySettings=C:\\Windows\\System32\\winhttp.WinHttpRefreshProxySettings")
#pragma comment(linker, "/export:WinHttpRegisterProxyChangeNotification=C:\\Windows\\System32\\winhttp.WinHttpRegisterProxyChangeNotification")
#pragma comment(linker, "/export:WinHttpResetAutoProxy=C:\\Windows\\System32\\winhttp.WinHttpResetAutoProxy")
#pragma comment(linker, "/export:WinHttpResolverGetProxyForUrl=C:\\Windows\\System32\\winhttp.WinHttpResolverGetProxyForUrl")
#pragma comment(linker, "/export:WinHttpSaveProxyCredentials=C:\\Windows\\System32\\winhttp.WinHttpSaveProxyCredentials")
#pragma comment(linker, "/export:WinHttpSendRequest=C:\\Windows\\System32\\winhttp.WinHttpSendRequest")
#pragma comment(linker, "/export:WinHttpSetCredentials=C:\\Windows\\System32\\winhttp.WinHttpSetCredentials")
#pragma comment(linker, "/export:WinHttpSetDefaultProxyConfiguration=C:\\Windows\\System32\\winhttp.WinHttpSetDefaultProxyConfiguration")
#pragma comment(linker, "/export:WinHttpSetOption=C:\\Windows\\System32\\winhttp.WinHttpSetOption")
#pragma comment(linker, "/export:WinHttpSetProxySettingsPerUser=C:\\Windows\\System32\\winhttp.WinHttpSetProxySettingsPerUser")
#pragma comment(linker, "/export:WinHttpSetSecureLegacyServersAppCompat=C:\\Windows\\System32\\winhttp.WinHttpSetSecureLegacyServersAppCompat")
#pragma comment(linker, "/export:WinHttpSetStatusCallback=C:\\Windows\\System32\\winhttp.WinHttpSetStatusCallback")
#pragma comment(linker, "/export:WinHttpSetTimeouts=C:\\Windows\\System32\\winhttp.WinHttpSetTimeouts")
#pragma comment(linker, "/export:WinHttpTimeFromSystemTime=C:\\Windows\\System32\\winhttp.WinHttpTimeFromSystemTime")
#pragma comment(linker, "/export:WinHttpTimeToSystemTime=C:\\Windows\\System32\\winhttp.WinHttpTimeToSystemTime")
#pragma comment(linker, "/export:WinHttpUnregisterProxyChangeNotification=C:\\Windows\\System32\\winhttp.WinHttpUnregisterProxyChangeNotification")
#pragma comment(linker, "/export:WinHttpWebSocketClose=C:\\Windows\\System32\\winhttp.WinHttpWebSocketClose")
#pragma comment(linker, "/export:WinHttpWebSocketCompleteUpgrade=C:\\Windows\\System32\\winhttp.WinHttpWebSocketCompleteUpgrade")
#pragma comment(linker, "/export:WinHttpWebSocketQueryCloseStatus=C:\\Windows\\System32\\winhttp.WinHttpWebSocketQueryCloseStatus")
#pragma comment(linker, "/export:WinHttpWebSocketReceive=C:\\Windows\\System32\\winhttp.WinHttpWebSocketReceive")
#pragma comment(linker, "/export:WinHttpWebSocketSend=C:\\Windows\\System32\\winhttp.WinHttpWebSocketSend")
#pragma comment(linker, "/export:WinHttpWebSocketShutdown=C:\\Windows\\System32\\winhttp.WinHttpWebSocketShutdown")
#pragma comment(linker, "/export:WinHttpWriteData=C:\\Windows\\System32\\winhttp.WinHttpWriteData")
#pragma comment(linker, "/export:WinHttpWriteProxySettings=C:\\Windows\\System32\\winhttp.WinHttpWriteProxySettings")