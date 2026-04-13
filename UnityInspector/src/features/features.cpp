#include "pch.h"
#include "features.h"

namespace Features
{
	static std::vector<std::unique_ptr<IFeature>> features;

	void Init()
	{
		for (auto& factory : GetRegistry())
			features.push_back(factory());
	}

	void Update(float deltaTime)
	{
		for (auto& feature : features)
			feature->Update(deltaTime);
	}

	void Render()
	{
		for (auto& feature : features)
			feature->Render();
	}
}
