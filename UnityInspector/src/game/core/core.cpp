#include "pch.h"
#include "core.h"

#include "game/features/inspector/inspector.h"
#include "game/features/assembly_explorer/assembly_explorer.h"
#include "game/features/debug_console/debug_console.h"
#include "game/features/tests/tests.h"

std::vector<std::unique_ptr<Core::Feature>> Core::features = [] {
    std::vector<std::unique_ptr<Feature>> vec;

    vec.push_back(std::make_unique<Inspector>());
    vec.push_back(std::make_unique<AssemblyExplorer>());
    vec.push_back(std::make_unique<DebugConsole>());
    vec.push_back(std::make_unique<Tests>());
    return vec;
    }();