#pragma once
#include "pch.h"
#include "features/features.h"

class LuaPlugin
{
public:
	LuaPlugin(sol::state& lua, const std::filesystem::path& path);
	~LuaPlugin();

	void Init();
	void Update(float deltaTime);
	void Render();
	void Unload();

	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool value) { enabled = value; }
	bool IsLoaded() const { return loaded; }
	const std::string& GetName() const { return name; }
	const std::string& GetError() const { return lastError; }
	bool HasError() const { return !lastError.empty(); }
	const std::filesystem::path& GetFilePath() const { return filePath; }

	std::filesystem::file_time_type GetStoredWriteTime() const { return storedWriteTime; }
	void UpdateStoredWriteTime();
	void Reload();

private:
	sol::state& luaState;
	std::filesystem::path filePath;
	std::string name;
	std::string lastError;
	bool enabled = true;
	bool loaded = false;
	std::filesystem::file_time_type storedWriteTime;

	sol::protected_function onInit;
	sol::protected_function onUpdate;
	sol::protected_function onRender;
	sol::protected_function onUnload;

	bool LoadFile();
	void ClearFunctions();
	void CaptureGlobalFunction(const char* funcName, sol::protected_function& out) const;
};
