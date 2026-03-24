#pragma once
#include "core/core.h"

struct Tests : IFeature
{
	void Update(float deltaTime) override;
	void Render() override;
};