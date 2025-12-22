#pragma once
#include "game/core/core.h"

struct Tests : Core::Feature
{
	void Update(float deltaTime) override;
	void Render() override;
};