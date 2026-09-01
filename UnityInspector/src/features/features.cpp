#include "pch.h"
#include "features.h"
#ifdef LUA_PLUGINS
#include "lua_system/lua_system.h"
#endif

void IFeature::Init()
{
}

void IFeature::Render()
{
}

namespace Features
{
	static std::vector<std::unique_ptr<IFeature>> features;

	void Init()
	{
		for (auto& factory : GetRegistry())
			features.push_back(factory());

#ifdef LUA_PLUGINS
		features.push_back(std::make_unique<LuaSystem>());
#endif

		for (const auto& feature : features)
			feature->Init();
	}

	void Update(float deltaTime)
	{
		for (const auto& feature : features)
			feature->Update(deltaTime);
	}

	void Render()
	{
		for (const auto& feature : features)
			feature->Render();
	}
}
