#include "pch.h"
#include "lua_plugin.h"
#include "features/debug_console/debug_console.h"

LuaPlugin::LuaPlugin(sol::state& lua, const std::filesystem::path& path)
	: luaState(lua), filePath(path)
{
	name = path.stem().string();
}

LuaPlugin::~LuaPlugin()
{
	if (loaded)
		Unload();
}

bool LuaPlugin::LoadFile()
{
	try
	{
		sol::load_result result = luaState.load_file(filePath.string());
		if (!result.valid())
		{
			sol::error err = result;
			lastError = std::format("[{}] Load error: {}", name, err.what());
			DebugConsole::AddLog(lastError, LogType::Error);
			return false;
		}

		sol::protected_function script = result;
		if (sol::protected_function_result pfr = script(); !pfr.valid())
		{
			sol::error err = pfr;
			lastError = std::format("[{}] Runtime error: {}", name, err.what());
			DebugConsole::AddLog(lastError, LogType::Error);
			return false;
		}

		lastError.clear();
		return true;
	}
	catch (const std::exception& e)
	{
		lastError = std::format("[{}] Exception: {}", name, e.what());
		DebugConsole::AddLog(lastError, LogType::Error);
		return false;
	}
}

void LuaPlugin::CaptureGlobalFunction(const char* funcName, sol::protected_function& out) const
{
	if (sol::object obj = luaState[funcName]; obj.is<sol::protected_function>())
	{
		out = obj.as<sol::protected_function>();
		luaState[funcName] = sol::nil;
	}
	else
	{
		out = sol::protected_function();
	}
}

void LuaPlugin::ClearFunctions()
{
	onInit = sol::protected_function();
	onUpdate = sol::protected_function();
	onRender = sol::protected_function();
	onUnload = sol::protected_function();
}

void LuaPlugin::UpdateStoredWriteTime()
{
	try
	{
		storedWriteTime = std::filesystem::last_write_time(filePath);
	}
	catch (...)
	{
		storedWriteTime = std::filesystem::file_time_type::min();
	}
}

void LuaPlugin::Init()
{
	if (!LoadFile())
		return;

	CaptureGlobalFunction("onInit", onInit);
	CaptureGlobalFunction("onUpdate", onUpdate);
	CaptureGlobalFunction("onRender", onRender);
	CaptureGlobalFunction("onUnload", onUnload);

	UpdateStoredWriteTime();
	loaded = true;

	if (onInit.valid())
	{
		if (sol::protected_function_result result = onInit(); !result.valid())
		{
			sol::error err = result;
			lastError = std::format("[{}] onInit error: {}", name, err.what());
			DebugConsole::AddLog(lastError, LogType::Error);
		}
	}
}

void LuaPlugin::Update(float deltaTime)
{
	if (!loaded || !enabled || !onUpdate.valid())
		return;

	if (sol::protected_function_result result = onUpdate(deltaTime); !result.valid())
	{
		sol::error err = result;
		lastError = std::format("[{}] onUpdate error: {}", name, err.what());
		DebugConsole::AddLog(lastError, LogType::Error);
		onUpdate = sol::protected_function();
	}
}

void LuaPlugin::Render()
{
	if (!loaded || !enabled || !onRender.valid())
		return;

	if (sol::protected_function_result result = onRender(); !result.valid())
	{
		sol::error err = result;
		lastError = std::format("[{}] onRender error: {}", name, err.what());
		DebugConsole::AddLog(lastError, LogType::Error);
		onRender = sol::protected_function();
	}
}

void LuaPlugin::Unload()
{
	if (loaded && onUnload.valid())
	{
		if (sol::protected_function_result result = onUnload(); !result.valid())
		{
			sol::error err = result;
			lastError = std::format("[{}] onUnload error: {}", name, err.what());
			DebugConsole::AddLog(lastError, LogType::Error);
		}
	}

	ClearFunctions();
	loaded = false;
}

void LuaPlugin::Reload()
{
	Unload();
	Init();
}
