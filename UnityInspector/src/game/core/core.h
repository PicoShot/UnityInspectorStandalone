#pragma once
#include "game/sdk/sdk.h"
#include "game/config/config.h"
#include "game/helper/helper.h"

struct Core 
{
	struct Feature
	{
		bool enabled = false;
		virtual ~Feature() = default;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;
	};

	inline static auto sdk = std::make_unique<SDK>();
	inline static auto config = std::make_unique<Config>();
	inline static auto helper = std::make_unique<Helper>();
	static std::vector<std::unique_ptr<Feature>> features;
};
