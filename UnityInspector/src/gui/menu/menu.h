#pragma once
#include "pch.h"
#include <vector>
#include <memory>

class ITab
{
public:
	virtual ~ITab() = default;
	virtual std::string GetName() const = 0;
	virtual void Render() = 0;
};

class Menu 
{
public:
	static void Init();
	static void Render();
private:
	static inline std::vector<std::unique_ptr<ITab>> s_Tabs;
	static inline bool s_Initialized = false;
};
