#pragma once
#include "pch.h"
#include "menu/menu.h"

class DebugTab final : public ITab
{
public:
	std::string GetName() const override { return name; }
	void Render() override;

private:
	std::string name = "Debug";
};
