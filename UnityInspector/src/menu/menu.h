#pragma once
#include "pch.h"

class ITab
{
public:
	virtual ~ITab() = default;
	virtual std::string GetName() const = 0;
	virtual void Render() = 0;
};

namespace Menu
{
	void Init();
	void Render();
}
