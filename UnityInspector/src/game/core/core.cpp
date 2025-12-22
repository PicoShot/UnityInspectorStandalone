#include "pch.h"
#include "core.h"

#include "game/features/inspector/inspector.h"
#include "game/features/Tests/Tests.h"

std::vector<std::unique_ptr<Core::Feature>> Core::features = [] {
    std::vector<std::unique_ptr<Feature>> vec;

    vec.push_back(std::make_unique<Inspector>());
    vec.push_back(std::make_unique<Tests>());
    return vec;
    }();