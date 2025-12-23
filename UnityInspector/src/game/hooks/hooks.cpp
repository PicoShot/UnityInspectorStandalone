#include "pch.h"
#include "hooks.h"
//#include "game/core/core.h"

//UR::Method* Hooks::pSetFov = nullptr;
//UR::Method* Hooks::pGetFov = nullptr;

void Hooks::Init()
{
    /*std::cout << "Installing Hooks" << std::endl;

    if (const auto pCameraClass = UR::Get("UnityEngine.CoreModule.dll")->Get("Camera"))
    {
        std::cout << "Found Camera Class at " << pCameraClass->address << std::endl;

        pSetFov = pCameraClass->Get<UR::Method>("set_fieldOfView");
        if (pSetFov)
        {
            std::cout << "Found SetFov Method at " << pSetFov->address << std::endl;
        }

        pGetFov = pCameraClass->Get<UR::Method>("get_fieldOfView");
        if (pGetFov)
        {
            std::cout << "Found GetFov Method at " << pGetFov->address << std::endl;
        }
    }*/
}

//void UNITY_CALLING_CONVENTION Hooks::HSetFov(void* _this, float value)
//{
//    Core::config->fovHack.Fov, Core::config->fovHack.BaseFov = value;
//
//    if (!Core::config->fovHack.Enabled) {
//        value = Core::config->fovHack.BaseFov;
//    }
//    else {
//        value = Core::config->fovHack.Fov;
//    }
//
//    if (Core::config->zoom.Enabled && Core::config->zoom.KeyPressed)
//        value = 30.f;
//
//    if (pSetFov)
//    {
//        pSetFov->RuntimeInvoke<void>(_this, value);
//    }
//}
//
//float UNITY_CALLING_CONVENTION Hooks::HGetFov(void* _this)
//{
//    if (Core::config->fovHack.Bypass)
//    {
//        if (Core::config->fovHack.BaseFov != 0.f)
//            return Core::config->fovHack.BaseFov;
//    }
//
//    if (pGetFov)
//    {
//       // return pGetFov->RuntimeInvoke<float>(_this);
//    }
//
//    return 0.f;
//}
