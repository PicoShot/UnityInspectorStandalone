#include "pch.h"
#include "core.h"

#include "features/inspector/inspector.h"
#include "features/assembly_explorer/assembly_explorer.h"
#include "features/debug_console/debug_console.h"
#include "features/tests/tests.h"

std::vector<std::unique_ptr<IFeature>> Core::features = [] {
    std::vector<std::unique_ptr<IFeature>> vec;

    vec.push_back(std::make_unique<Inspector>());
    vec.push_back(std::make_unique<AssemblyExplorer>());
    vec.push_back(std::make_unique<DebugConsole>());
    vec.push_back(std::make_unique<Tests>());
    return vec;
    }();