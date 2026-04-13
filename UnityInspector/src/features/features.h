#pragma once
#include "config/config.h"
#include "helper/helper.h"

#include <vector>
#include <memory>
#include <functional>

class IFeature
{
public:
	virtual ~IFeature() = default;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
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
