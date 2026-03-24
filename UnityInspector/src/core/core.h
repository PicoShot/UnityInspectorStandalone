#pragma once
#include "config/config.h"
#include "helper/helper.h"

struct IFeature
{
	virtual ~IFeature() = default;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
};

class Core 
{
public:
	inline static auto config = std::make_unique<Config>();
	static std::vector<std::unique_ptr<IFeature>> features;
};
