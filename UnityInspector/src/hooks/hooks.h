#pragma once
#include "pch.h"
#include <vector>
#include <memory>

class IHook
{
public:
	virtual ~IHook() = default;
	virtual void Install() = 0;
};

class Hooks
{
public:
	static void Init();

private:
	static inline std::vector<std::unique_ptr<IHook>> s_Hooks;
};