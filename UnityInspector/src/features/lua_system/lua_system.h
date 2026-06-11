#pragma once
#include "pch.h"
#include "features/features.h"

class LuaPlugin;

class LuaSystem final : public IFeature
{
public:
	void Init() override;
	void Update(float deltaTime) override;
	void Render() override;

	void ReloadAll();
	void ExecuteString(const std::string& code);
	void EnsureInitialized();
	bool IsReady() const { return initialized; }

	sol::state& GetState() { return *luaState; }
	std::vector<std::unique_ptr<LuaPlugin>>& GetPlugins() { return plugins; }

	bool IsConsoleVisible() const { return showConsole; }
	void SetConsoleVisible(bool visible) { showConsole = visible; }

	static LuaSystem* GetInstance() { return s_Instance; }

private:
	static inline LuaSystem* s_Instance = nullptr;
	std::unique_ptr<sol::state> luaState;
	std::vector<std::unique_ptr<LuaPlugin>> plugins;
	std::filesystem::path pluginsDir;
	bool initialized = false;
	bool showConsole = false;
	float reloadTimer = 0.0f;
	static constexpr float RELOAD_INTERVAL = .5f;

	std::unique_ptr<TextEditor> textEditor;

	void ScanPlugins();
	void CheckHotReload();
	void RenderLuaConsole();
};
