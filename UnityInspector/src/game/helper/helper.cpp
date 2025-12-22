#include "pch.h"
#include "helper.h"
#include "game/core/core.h"


bool Helper::SafeIsAlive(UT::UnityObject* obj)
{
    if (!obj) return false;

    __try
    {
        return obj->IsAlive();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::SafeGetTag(UT::GameObject* obj, UT::String*& outTag)
{
    if (!obj) return false;

    __try
    {
        outTag = obj->GetTag();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::SafeGetActiveSelf(UT::GameObject* obj, bool& outActive)
{
    if (!obj) return false;

    __try
    {
        outActive = obj->GetActiveSelf();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::SafeGetIsStatic(UT::GameObject* obj, bool& outStatic)
{
    if (!obj) return false;

    __try
    {
        outStatic = obj->GetIsStatic();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::SafeReadInt(void* ptr, const int offset, int& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteInt(void* ptr, const int offset, const int value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadFloat(void* ptr, const int offset, float& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteFloat(void* ptr, const int offset, const float value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadDouble(void* ptr, const int offset, double& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteDouble(void* ptr, const int offset, double value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadBool(void* ptr, const int offset, bool& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteBool(void* ptr, const int offset, const bool value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadVector2(void* ptr, const int offset, UT::Vector2& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::Vector2*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteVector2(void* ptr, const int offset, const UT::Vector2& value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<UT::Vector2*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadVector3(void* ptr, const int offset, UT::Vector3& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::Vector3*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteVector3(void* ptr, const int offset, const UT::Vector3& value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<UT::Vector3*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadVector4(void* ptr, const int offset, UT::Vector4& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::Vector4*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteVector4(void* ptr, const int offset, const UT::Vector4& value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<UT::Vector4*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadQuaternion(void* ptr, const int offset, UT::Quaternion& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::Quaternion*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteQuaternion(void* ptr, const int offset, const UT::Quaternion& value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<UT::Quaternion*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadColor(void* ptr, const int offset, UT::Color& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::Color*>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeWriteColor(void* ptr, const int offset, const UT::Color& value)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        *reinterpret_cast<UT::Color*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeReadStringPtr(void* ptr, const int offset, UT::String*& outValue)
{
    if (!ptr || offset < 0) return false;
    __try
    {
        outValue = *reinterpret_cast<UT::String**>(reinterpret_cast<uintptr_t>(ptr) + offset);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool Helper::SafeGetStaticFieldInt(void* fieldHandle, int& outValue)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, int*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
        }
        else
        {
            UR::Invoke<void, void*, int*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeSetStaticFieldInt(void* fieldHandle, int value)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, int*>("mono_field_static_set_value", vTable, fieldHandle, &value);
        }
        else
        {
            UR::Invoke<void, void*, int*>("il2cpp_field_static_set_value", fieldHandle, &value);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeGetStaticFieldFloat(void* fieldHandle, float& outValue)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, float*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
        }
        else
        {
            UR::Invoke<void, void*, float*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeSetStaticFieldFloat(void* fieldHandle, float value)
{
    if (!fieldHandle) return false;
    try
    {

        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, float*>("mono_field_static_set_value", vTable, fieldHandle, &value);
        }
        else
        {
            UR::Invoke<void, void*, float*>("il2cpp_field_static_set_value", fieldHandle, &value);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Helper::SafeGetStaticFieldBool(void* fieldHandle, bool& outValue)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, bool*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
        }
        else
        {
            UR::Invoke<void, void*, bool*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeSetStaticFieldBool(void* fieldHandle, bool value)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, bool*>("mono_field_static_set_value", vTable, fieldHandle, &value);
        }
        else
        {
            UR::Invoke<void, void*, bool*>("il2cpp_field_static_set_value", fieldHandle, &value);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeGetStaticFieldDouble(void* fieldHandle, double& outValue)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, double*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
        }
        else
        {
            UR::Invoke<void, void*, double*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeSetStaticFieldDouble(void* fieldHandle, double value)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, double*>("mono_field_static_set_value", vTable, fieldHandle, &value);
        }
        else
        {
            UR::Invoke<void, void*, double*>("il2cpp_field_static_set_value", fieldHandle, &value);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeGetStaticFieldVector3(void* fieldHandle, UT::Vector3& outValue)
{
    if (!fieldHandle) return false;
    try
    {
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
                UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, UT::Vector3*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
        }
        else
        {
            UR::Invoke<void, void*, UT::Vector3*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeSetStaticFieldVector3(void* fieldHandle, const UT::Vector3& value)
{
    if (!fieldHandle) return false;
    try
    {
        UT::Vector3 v = value;
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
            UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
            UR::Invoke<void, void*, void*, UT::Vector3*>("mono_field_static_set_value", vTable, fieldHandle, &v);
        }
        else
        {
            UR::Invoke<void, void*, UT::Vector3*>("il2cpp_field_static_set_value", fieldHandle, &v);
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeInvokeGetter(void* obj, void* methodHandle, void* outValue, int valueSize)
{
    if (!methodHandle) return false;
    try
    {
        void* result;
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            result = UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, nullptr, nullptr);
        }
        else
        {
            result = UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, nullptr, nullptr);
        }
        if (result && outValue)
        {
            void* unboxed = UR::Invoke<void*, void*>(
                Core::config->gameMode == UnityResolve::Mode::Mono ? "mono_object_unbox" : "il2cpp_object_unbox", result);
            if (unboxed)
            {
                memcpy(outValue, unboxed, valueSize);
            }
        }
        return true;
    }
    catch (...) { return false; }
}

bool Helper::SafeInvokeSetter(void* obj, void* methodHandle, void* value)
{
    if (!methodHandle || !value) return false;
    try
    {
        void* params[1] = { value };
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, params, nullptr);
        }
        else
        {
            UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, params, nullptr);
        }
        return true;
    }
    catch (...) { return false; }
}

void* Helper::SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success)
{
    success = false;
    if (!methodHandle) return nullptr;
    try
    {
        void* result;
        if (Core::config->gameMode == UnityResolve::Mode::Mono)
        {
            result = UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, params, nullptr);
        }
        else
        {
            result = UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, params, nullptr);
        }
        success = true;
        return result;
    }
    catch (...)
    {
        success = false;
        return nullptr;
    }
}

bool Helper::SafeGetGameObject(UT::Transform* transform, UT::GameObject*& outGameObject)
{
    if (!transform) return false;
    __try
    {
        outGameObject = transform->GetGameObject();
        return outGameObject != nullptr;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outGameObject = nullptr;
        return false;
    }
}

bool Helper::SafeGetTransform(UT::GameObject* gameObject, UT::Transform*& outTransform)
{
    if (!gameObject) return false;
    __try
    {
        outTransform = gameObject->GetTransform();
        return outTransform != nullptr;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outTransform = nullptr;
        return false;
    }
}

bool Helper::SafeGetParent(UT::Transform* transform, UT::Transform*& outParent)
{
    if (!transform) return false;
    __try
    {
        outParent = transform->GetParent();
        return true; // Parent can be null for root objects
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outParent = nullptr;
        return false;
    }
}

bool Helper::SafeGetName(UT::UnityObject* obj, UT::String*& outName)
{
    if (!obj) return false;
    __try
    {
        outName = obj->GetName();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outName = nullptr;
        return false;
    }
}

bool Helper::SafeGetChildCount(UT::Transform* transform, int& outCount)
{
    if (!transform) return false;
    __try
    {
        outCount = transform->GetChildCount();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outCount = 0;
        return false;
    }
}

bool Helper::SafeGetChild(UT::Transform* transform, int index, UT::Transform*& outChild)
{
    if (!transform) return false;
    __try
    {
        outChild = transform->GetChild(index);
        return outChild != nullptr;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        outChild = nullptr;
        return false;
    }
}

Vec3 Helper::TryGetPosition(Transform* go)
{
    __try
    {
        return go->GetPosition();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return {};
	}
}

Camera* Helper::GetMainCamera()
{
    __try
    {
        return Camera::GetMain();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return nullptr;
    }
}

bool Helper::RayCast(const Vec3& origin, const Vec3& direction, float maxDistance)
{
    __try
    {
        return Physics::Raycast(origin, direction, maxDistance);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::RayCastHit(const Vec3& origin, const Vec3& direction, const RaycastHit& hitInfo, float maxDistance)
{
    __try
    {
        Ray ray;
        ray.m_vOrigin = origin;
        ray.m_vDirection = direction;
        return Physics::Raycast(ray, &hitInfo, maxDistance);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

bool Helper::WorldToScreen(const Vec3 worldPos, Vec2& screenPos)
{
    __try
    {
        const auto camera = GetMainCamera();
        if (!camera) return false;

        const auto pos = camera->WorldToScreenPoint(worldPos);
        if (pos.z < 0.f) return false;

        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        if (screen_size.x <= 0 || screen_size.y <= 0) return false;

        screenPos = Vec2(pos.x, screen_size.y - pos.y);

        return true;//(screenPos.x >= 0 && screenPos.x <= screen_size.x && screenPos.y >= 0 && screenPos.y <= screen_size.y);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}