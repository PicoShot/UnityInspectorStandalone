#pragma once
#include "features/features.h"

class Tests : public IFeature
{
public:
	void Update(float deltaTime) override;
	void Render() override;
};