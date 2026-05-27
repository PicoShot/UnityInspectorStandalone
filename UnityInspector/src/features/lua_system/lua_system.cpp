#include "pch.h"
#include "lua_system.h"
#include "lua_plugin.h"
#include "lua_bindings.h"
#include "features/debug_console/debug_console.h"

REGISTER_FEATURE(LuaSystem)

void LuaSystem::Init()
{
	s_Instance = this;
	try
	{
		LuaBindings::RegisterAll(luaState);

		textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
		textEditor.SetPalette(TextEditor::GetDarkPalette());
		textEditor.SetShowWhitespaces(false);

		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		pluginsDir = std::filesystem::path(buffer).parent_path() / "plugins";

		if (!std::filesystem::exists(pluginsDir))
		{
			std::filesystem::create_directories(pluginsDir);
			LOG_INFO("Created plugins directory: {}", pluginsDir.string());
		}

		ScanPlugins();
		initialized = true;
		LOG_INFO("LuaSystem initialized. Plugins dir: {}", pluginsDir.string());
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("LuaSystem init failed: {}", e.what());
		DebugConsole::AddLog(std::format("LuaSystem init failed: {}", e.what()), LogType::Error);
	}
}

void LuaSystem::ScanPlugins()
{
	plugins.clear();

	if (!std::filesystem::exists(pluginsDir))
		return;

	for (const auto& entry : std::filesystem::directory_iterator(pluginsDir))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != ".lua") continue;

		auto plugin = std::make_unique<LuaPlugin>(luaState, entry.path());
		plugin->Init();
		plugins.push_back(std::move(plugin));
	}
}

void LuaSystem::CheckHotReload()
{
	if (!std::filesystem::exists(pluginsDir))
		return;

	try
	{
		std::unordered_set<std::string> foundFiles;

		for (const auto& entry : std::filesystem::directory_iterator(pluginsDir))
		{
			if (!entry.is_regular_file() || entry.path().extension() != ".lua")
				continue;

			std::string pathStr = entry.path().string();
			foundFiles.insert(pathStr);

			auto it = std::ranges::find_if(plugins, [&](const auto& p)
			{
				return p->GetFilePath() == entry.path();
			});

			if (it != plugins.end())
			{
				try
				{
					auto currentWriteTime = std::filesystem::last_write_time(entry.path());

					if (auto storedWriteTime = (*it)->GetStoredWriteTime(); currentWriteTime != storedWriteTime)
					{
						LOG_INFO("Hot-reloading plugin: {}", (*it)->GetName());
						(*it)->Reload();
					}
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Failed to check write time for {}: {}", entry.path().string(), e.what());
				}
			}
			else
			{
				auto plugin = std::make_unique<LuaPlugin>(luaState, entry.path());
				plugin->Init();
				LOG_INFO("Loaded new plugin: {}", plugin->GetName());
				plugins.push_back(std::move(plugin));
			}
		}

		std::erase_if(plugins, [&](const auto& p)
		{
			if (std::string pathStr = p->GetFilePath().string(); !foundFiles.contains(pathStr))
			{
				LOG_INFO("Unloaded removed plugin: {}", p->GetName());
				return true;
			}
			return false;
		});
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("CheckHotReload failed: {}", e.what());
	}
}

void LuaSystem::Update(float deltaTime)
{
	if (!initialized) return;

	reloadTimer += deltaTime;
	if (reloadTimer >= RELOAD_INTERVAL)
	{
		reloadTimer -= RELOAD_INTERVAL;
		CheckHotReload();
	}

	UR::ThreadAttach();
	for (auto& plugin : plugins)
	{
		if (plugin->IsEnabled())
			plugin->Update(deltaTime);
	}
}

void LuaSystem::Render()
{
	if (!initialized) return;

	UR::ThreadAttach();
	for (auto& plugin : plugins)
	{
		if (plugin->IsEnabled())
			plugin->Render();
	}

	if (showConsole)
		RenderLuaConsole();
}

void LuaSystem::RenderLuaConsole()
{
	ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Lua Console", &showConsole))
	{
		if (ImGui::Button("Execute", ImVec2(100, 0)))
		{
			if (std::string code = textEditor.GetText(); !code.empty())
			{
				ExecuteString(code);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear", ImVec2(100, 0)))
		{
			textEditor.SetText("");
		}

		ImGui::Spacing();

		textEditor.Render("##LuaConsole", ImVec2(-1, ImGui::GetContentRegionAvail().y), true);
	}
	ImGui::End();
}

void LuaSystem::ReloadAll()
{
	if (!initialized) return;
	ScanPlugins();
}

void LuaSystem::ExecuteString(const std::string& code)
{
	if (!initialized) return;

	try
	{
		UR::ThreadAttach();
		if (sol::protected_function_result result = luaState.safe_script(code); !result.valid())
		{
			sol::error err = result;
			DebugConsole::AddLog(std::format("Lua console error: {}", err.what()), LogType::Error);
		}
	}
	catch (const std::exception& e)
	{
		DebugConsole::AddLog(std::format("Lua console exception: {}", e.what()), LogType::Error);
	}
}
