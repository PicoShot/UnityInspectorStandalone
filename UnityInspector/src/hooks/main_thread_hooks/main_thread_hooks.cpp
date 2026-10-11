#include "pch.h"
#include "features/features.h"
#include "hooks/main_thread_hooks/main_thread_hooks.h"
#include "window/window.h"

REGISTER_HOOK(MainThreadHooks)

void UNITY_CALLING_CONVENTION MainThreadHooks::HExec(void* instance, void* methodInfo)
{
	HookManager::Fcall(HExec, instance, methodInfo);

	static bool running = false;
	if (running) return;
	running = true;

	static auto previous = std::chrono::steady_clock::now();
	const auto now = std::chrono::steady_clock::now();
	const float deltaTime = std::chrono::duration<float>(now - previous).count();
	previous = now;

	try
	{
		Features::Update(std::min(deltaTime, 0.25f));
	}
	catch (...)
	{
	}
	Window::BuildFrame();
	running = false;
}

void MainThreadHooks::Install()
{
	const auto* unityCore = UR::Get(TargetAssembly().data());
	if (!unityCore) return;

	auto* context = unityCore->Get("UnitySynchronizationContext", "UnityEngine");
	if (!context)
	{
		LOG_ERROR("Main-thread update hook failed: UnitySynchronizationContext class not found");
		return;
	}

	auto* exec = context->Get<UR::Method>("Exec");
	if (!exec)
	{
		LOG_ERROR("Main-thread update hook failed: UnitySynchronizationContext.Exec not found");
		return;
	}

	if (auto* target = exec->Cast<void, void*, void*>(); !target || !HookManager::Install(target, HExec))
	{
		LOG_ERROR("Main-thread update hook failed: could not install UnitySynchronizationContext.Exec hook");
		return;
	}

	Window::g_MainThreadUi = true;
	LOG_INFO("Main-thread feature update hook installed");
}
