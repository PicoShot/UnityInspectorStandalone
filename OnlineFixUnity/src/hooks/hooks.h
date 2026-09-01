#pragma once
#include "pch.h"

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

	using Factory = std::function<std::unique_ptr<IHook>()>;

	static std::vector<Factory>& GetRegistry()
	{
		static std::vector<Factory> registry;
		return registry;
	}
};

#define REGISTER_HOOK(HookClass)                                          \
    static struct HookClass##_Registrar {                                 \
        HookClass##_Registrar() {                                         \
            Hooks::GetRegistry().push_back(                               \
                []() -> std::unique_ptr<IHook> {                          \
                    return std::make_unique<HookClass>();                 \
                });                                                       \
        }                                                                 \
    } s_##HookClass##_registrar;
