#include "pch.h"
#include "features.h"

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
