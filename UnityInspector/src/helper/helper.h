#pragma once
#include "pch.h"

namespace Helper
{
	struct BoundingBox2D
	{
		float minX = FLT_MAX, minY = FLT_MAX;
		float maxX = -FLT_MAX, maxY = -FLT_MAX;
		bool valid = false;
	};

	bool SafeIsAlive(UnityObject* obj);
	bool SafeGetTag(GameObject* obj, UT::String*& outTag);
	bool SafeGetActiveSelf(GameObject* obj, bool& outActive);
	bool SafeSetActive(GameObject* obj, bool value);
	bool SafeGetIsStatic(GameObject* obj, bool& outStatic);
	bool TryGetPosition(Transform* go, Vec3& outPos);
	Camera* GetMainCamera();

	bool SafeReadInt(void* ptr, int offset, int& outValue);
	bool SafeWriteInt(void* ptr, int offset, int value);
	bool SafeReadFloat(void* ptr, int offset, float& outValue);
	bool SafeWriteFloat(void* ptr, int offset, float value);
	bool SafeReadDouble(void* ptr, int offset, double& outValue);
	bool SafeWriteDouble(void* ptr, int offset, double value);
	bool SafeReadBool(void* ptr, int offset, bool& outValue);
	bool SafeWriteBool(void* ptr, int offset, bool value);
	bool SafeReadVector2(void* ptr, int offset, Vec2& outValue);
	bool SafeWriteVector2(void* ptr, int offset, const Vec2& value);
	bool SafeReadVector3(void* ptr, int offset, Vec3& outValue);
	bool SafeWriteVector3(void* ptr, int offset, const Vec3& value);
	bool SafeReadVector4(void* ptr, int offset, Vec4& outValue);
	bool SafeWriteVector4(void* ptr, int offset, const Vec4& value);
	bool SafeReadQuaternion(void* ptr, int offset, Quat& outValue);
	bool SafeWriteQuaternion(void* ptr, int offset, const Quat& value);
	bool SafeReadColor(void* ptr, int offset, Color& outValue);
	bool SafeWriteColor(void* ptr, int offset, const Color& value);
	bool SafeReadStringPtr(void* ptr, int offset, UT::String*& outValue);

	bool SafeGetStaticFieldInt(void* fieldHandle, int& outValue);
	bool SafeSetStaticFieldInt(void* fieldHandle, int value);
	bool SafeGetStaticFieldFloat(void* fieldHandle, float& outValue);
	bool SafeSetStaticFieldFloat(void* fieldHandle, float value);
	bool SafeGetStaticFieldBool(void* fieldHandle, bool& outValue);
	bool SafeSetStaticFieldBool(void* fieldHandle, bool value);
	bool SafeGetStaticFieldDouble(void* fieldHandle, double& outValue);
	bool SafeSetStaticFieldDouble(void* fieldHandle, double value);
	bool SafeGetStaticFieldVector3(void* fieldHandle, Vec3& outValue);
	bool SafeSetStaticFieldVector3(void* fieldHandle, const Vec3& value);

	bool SafeInvokeGetter(void* obj, void* methodHandle, void* outValue, int valueSize);
	bool SafeInvokeSetter(void* obj, void* methodHandle, void* value);

	void* SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success);
	bool SafeGetGameObject(Rigidbody* rb, GameObject*& outGameObject);
	bool SafeGetGameObject(Transform* transform, GameObject*& outGameObject);
	bool SafeGetGameObject(MonoBehaviour* mb, GameObject*& outGameObject);
	bool SafeGetTransform(GameObject* gameObject, Transform*& outTransform);
	bool SafeGetTransform(Rigidbody* gameObject, Transform*& outTransform);
	bool SafeGetParent(Transform* transform, Transform*& outParent);
	bool SafeGetName(UnityObject* obj, UT::String*& outName);
	bool SafeGetChildCount(Transform* transform, int& outCount);
	bool SafeGetChild(Transform* transform, int index, Transform*& outChild);

	bool RayCast(const Vec3& origin, const Vec3& direction, float maxDistance = 1000.0f);
	bool RayCastHit(const Vec3& origin, const Vec3& direction, const RaycastHit& hitInfo, float maxDistance = 1000.0f);
	bool WorldToScreen(const Vec3 worldPos, Vec2& screenPos);
}