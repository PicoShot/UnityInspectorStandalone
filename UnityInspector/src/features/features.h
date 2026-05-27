#pragma once
#include "config/config.h"

class IFeature
{
public:
	virtual ~IFeature() = default;
	virtual void Init();
	virtual void Update(float deltaTime) = 0;
	virtual void Render();
};

namespace Features
{
	using Factory = std::function<std::unique_ptr<IFeature>()>;

	inline std::vector<Factory>& GetRegistry()
	{
		static std::vector<Factory> registry;
		return registry;
	}

	void Init();
	void Update(float deltaTime);
	void Render();
}

#define REGISTER_FEATURE(FeatureClass)                                    \
    static struct FeatureClass##_Registrar {                              \
        FeatureClass##_Registrar() {                                      \
            Features::GetRegistry().push_back(                            \
                []() -> std::unique_ptr<IFeature> {                       \
                    return std::make_unique<FeatureClass>();               \
                });                                                       \
        }                                                                 \
    } s_##FeatureClass##_registrar;
