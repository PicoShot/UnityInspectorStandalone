#pragma once
#include "pch.h"
#include "gui/menu/menu.h"

class DebugTab : public ITab
{
public:
	std::string GetName() const override { return name; }
	void Render() override;

private:
	std::string name = X("Debug");
};
