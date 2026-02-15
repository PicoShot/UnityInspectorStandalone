#pragma once
#include "pch.h"

struct Helper 
{
    struct BoundingBox2D
    {
        float minX = FLT_MAX, minY = FLT_MAX;
        float maxX = -FLT_MAX, maxY = -FLT_MAX;
        bool valid = false;
    };
    static bool SafeIsAlive(UnityObject* obj);
    static bool SafeGetTag(GameObject* obj, UT::String*& outTag);
    static bool SafeGetActiveSelf(GameObject* obj, bool& outActive);
    static bool SafeSetActive(GameObject* obj, bool value);
    static bool SafeGetIsStatic(GameObject* obj, bool& outStatic);
	static bool TryGetPosition(Transform* go, Vec3& outPos);
    static Camera* GetMainCamera();

    static bool SafeReadInt(void* ptr, int offset, int& outValue);
    static bool SafeWriteInt(void* ptr, int offset, int value);
    static bool SafeReadFloat(void* ptr, int offset, float& outValue);
    static bool SafeWriteFloat(void* ptr, int offset, float value);
    static bool SafeReadDouble(void* ptr, int offset, double& outValue);
    static bool SafeWriteDouble(void* ptr, int offset, double value);
    static bool SafeReadBool(void* ptr, int offset, bool& outValue);
    static bool SafeWriteBool(void* ptr, int offset, bool value);
    static bool SafeReadVector2(void* ptr, int offset, Vec2& outValue);
    static bool SafeWriteVector2(void* ptr, int offset, const Vec2& value);
    static bool SafeReadVector3(void* ptr, int offset, Vec3& outValue);
    static bool SafeWriteVector3(void* ptr, int offset, const Vec3& value);
    static bool SafeReadVector4(void* ptr, int offset, Vec4& outValue);
    static bool SafeWriteVector4(void* ptr, int offset, const Vec4& value);
    static bool SafeReadQuaternion(void* ptr, int offset, Quat& outValue);
    static bool SafeWriteQuaternion(void* ptr, int offset, const Quat& value);
    static bool SafeReadColor(void* ptr, int offset, Color& outValue);
    static bool SafeWriteColor(void* ptr, int offset, const Color& value);
    static bool SafeReadStringPtr(void* ptr, int offset, UT::String*& outValue);

    static bool SafeGetStaticFieldInt(void* fieldHandle, int& outValue);
    static bool SafeSetStaticFieldInt(void* fieldHandle, int value);
    static bool SafeGetStaticFieldFloat(void* fieldHandle, float& outValue);
    static bool SafeSetStaticFieldFloat(void* fieldHandle, float value);
    static bool SafeGetStaticFieldBool(void* fieldHandle, bool& outValue);
    static bool SafeSetStaticFieldBool(void* fieldHandle, bool value);
    static bool SafeGetStaticFieldDouble(void* fieldHandle, double& outValue);
    static bool SafeSetStaticFieldDouble(void* fieldHandle, double value);
    static bool SafeGetStaticFieldVector3(void* fieldHandle, Vec3& outValue);
    static bool SafeSetStaticFieldVector3(void* fieldHandle, const Vec3& value);

    static bool SafeInvokeGetter(void* obj, void* methodHandle, void* outValue, int valueSize);
    static bool SafeInvokeSetter(void* obj, void* methodHandle, void* value);

    static void* SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success);
    static bool SafeGetGameObject(Rigidbody* rb, GameObject*& outGameObject);
    static bool SafeGetGameObject(Transform* transform, GameObject*& outGameObject);
    static bool SafeGetGameObject(MonoBehaviour* mb, GameObject*& outGameObject);
    static bool SafeGetTransform(GameObject* gameObject, Transform*& outTransform);
    static bool SafeGetTransform(Rigidbody* gameObject, Transform*& outTransform);
    static bool SafeGetParent(Transform* transform, Transform*& outParent);
    static bool SafeGetName(UnityObject* obj, UT::String*& outName);
    static bool SafeGetChildCount(Transform* transform, int& outCount);
    static bool SafeGetChild(Transform* transform, int index, Transform*& outChild);

    static bool RayCast(const Vec3& origin, const Vec3& direction, float maxDistance = 1000.0f);
    static bool RayCastHit(const Vec3& origin, const Vec3& direction, const RaycastHit& hitInfo, float maxDistance = 1000.0f);
    static bool WorldToScreen(const Vec3 worldPos, Vec2& screenPos);
};
