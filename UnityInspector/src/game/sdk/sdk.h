#pragma once
#include "pch.h"

struct SDK 
{

    struct RaycastDebugInfo {
        Vec3 origin;
        Vec3 direction;
        Vec3 hitPoint;
        GameObject* object;
        bool hit = false;
        float distance = 0.0f;
        bool active = false;
        char hitObjectName[128] = "Unknown";
        char hitObjectTag[128] = "Unknown";
    };

    RaycastDebugInfo raycastDebug;
    RaycastHit globalHitInfo;
};
