#pragma once
#include "pch.h"
#include "features/inspector/editable_type.h"

namespace Helper
{
	struct BoundingBox2D
	{
		float minX = FLT_MAX, minY = FLT_MAX;
		float maxX = -FLT_MAX, maxY = -FLT_MAX;
		bool valid = false;
	};

	bool IsValidUserPointer(void* ptr);
	bool SafeIsAlive(UnityObject* obj);
	bool SafeGetTag(GameObject* obj, UT::String*& outTag);
	bool SafeGetActiveSelf(GameObject* obj, bool& outActive);
	bool SafeSetActive(const GameObject* obj, bool value);
	bool SafeGetIsStatic(GameObject* obj, bool& outStatic);
	bool TryGetPosition(Transform* go, Vec3& outPos);
	Camera* GetMainCamera();

	bool SafeReadInt(void* ptr, int offset, int& outValue);
	bool SafeWriteInt(void* ptr, int offset, int value);
	bool SafeReadInt64(void* ptr, int offset, int64_t& outValue);
	bool SafeWriteInt64(void* ptr, int offset, int64_t value);
	bool SafeReadUInt64(void* ptr, int offset, uint64_t& outValue);
	bool SafeWriteUInt64(void* ptr, int offset, uint64_t value);
	bool SafeReadByte(void* ptr, int offset, uint8_t& outValue);
	bool SafeWriteByte(void* ptr, int offset, uint8_t value);
	bool SafeReadSByte(void* ptr, int offset, int8_t& outValue);
	bool SafeWriteSByte(void* ptr, int offset, int8_t value);
	bool SafeReadInt16(void* ptr, int offset, int16_t& outValue);
	bool SafeWriteInt16(void* ptr, int offset, int16_t value);
	bool SafeReadUInt16(void* ptr, int offset, uint16_t& outValue);
	bool SafeWriteUInt16(void* ptr, int offset, uint16_t value);
	bool SafeReadChar(void* ptr, int offset, char16_t& outValue);
	bool SafeWriteChar(void* ptr, int offset, char16_t value);
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

	bool SafeReadPointer(void* ptr, int offset, void*& outValue);

	bool SafeGetStaticFieldInt(void* fieldHandle, int& outValue);
	bool SafeSetStaticFieldInt(void* fieldHandle, int value);
	bool SafeGetStaticFieldInt64(void* fieldHandle, int64_t& outValue);
	bool SafeSetStaticFieldInt64(void* fieldHandle, int64_t value);
	bool SafeGetStaticFieldUInt64(void* fieldHandle, uint64_t& outValue);
	bool SafeSetStaticFieldUInt64(void* fieldHandle, uint64_t value);
	bool SafeGetStaticFieldByte(void* fieldHandle, uint8_t& outValue);
	bool SafeSetStaticFieldByte(void* fieldHandle, uint8_t value);
	bool SafeGetStaticFieldSByte(void* fieldHandle, int8_t& outValue);
	bool SafeSetStaticFieldSByte(void* fieldHandle, int8_t value);
	bool SafeGetStaticFieldInt16(void* fieldHandle, int16_t& outValue);
	bool SafeSetStaticFieldInt16(void* fieldHandle, int16_t value);
	bool SafeGetStaticFieldUInt16(void* fieldHandle, uint16_t& outValue);
	bool SafeSetStaticFieldUInt16(void* fieldHandle, uint16_t value);
	bool SafeGetStaticFieldChar(void* fieldHandle, char16_t& outValue);
	bool SafeSetStaticFieldChar(void* fieldHandle, char16_t value);
	bool SafeGetStaticFieldFloat(void* fieldHandle, float& outValue);
	bool SafeSetStaticFieldFloat(void* fieldHandle, float value);
	bool SafeGetStaticFieldBool(void* fieldHandle, bool& outValue);
	bool SafeSetStaticFieldBool(void* fieldHandle, bool value);
	bool SafeGetStaticFieldDouble(void* fieldHandle, double& outValue);
	bool SafeSetStaticFieldDouble(void* fieldHandle, double value);
	bool SafeGetStaticFieldVector2(void* fieldHandle, Vec2& outValue);
	bool SafeSetStaticFieldVector2(void* fieldHandle, const Vec2& value);
	bool SafeGetStaticFieldVector3(void* fieldHandle, Vec3& outValue);
	bool SafeSetStaticFieldVector3(void* fieldHandle, const Vec3& value);
	bool SafeGetStaticFieldVector4(void* fieldHandle, Vec4& outValue);
	bool SafeSetStaticFieldVector4(void* fieldHandle, const Vec4& value);
	bool SafeGetStaticFieldQuaternion(void* fieldHandle, Quat& outValue);
	bool SafeSetStaticFieldQuaternion(void* fieldHandle, const Quat& value);
	bool SafeGetStaticFieldColor(void* fieldHandle, Color& outValue);
	bool SafeSetStaticFieldColor(void* fieldHandle, const Color& value);
	bool SafeGetStaticFieldPointer(void* fieldHandle, void*& outValue);

	void* SafeGetObjectClass(void* obj);

	bool SafeInvokeGetter(void* obj, void* methodHandle, void* outValue, int valueSize);
	bool SafeInvokeGetterPointer(void* obj, void* methodHandle, void*& outPointer);
	bool SafeInvokeSetter(void* obj, void* methodHandle, void* value);

	void* SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success);

	struct InvokeParamBuffers
	{
		std::vector<void*> params;
		std::vector<std::unique_ptr<char[]>> buffers;
	};

	InvokeParamBuffers BuildInvokeParams(const std::vector<std::string>& paramValues,
	                                     const std::vector<EditableType>& paramTypes);

	bool SafeGetComponents(GameObject* obj, UR::Class* componentClass, std::vector<UT::Component*>& outComponents);
	bool SafeGetGameObject(Rigidbody* rb, GameObject*& outGameObject);
	bool SafeGetGameObject(Transform* transform, GameObject*& outGameObject);
	bool SafeGetGameObject(MonoBehaviour* mb, GameObject*& outGameObject);
	bool SafeGetGameObject(Component* comp, GameObject*& outGameObject);
	bool SafeGetTransform(GameObject* gameObject, Transform*& outTransform);
	bool SafeGetTransform(Rigidbody* gameObject, Transform*& outTransform);
	bool SafeGetParent(Transform* transform, Transform*& outParent);
	bool SafeGetName(UnityObject* obj, UT::String*& outName);
	bool SafeGetChildCount(Transform* transform, int& outCount);
	bool SafeGetChild(Transform* transform, int index, Transform*& outChild);

	bool RayCast(const Vec3& origin, const Vec3& direction, float maxDistance = 1000.0f);
	bool RayCastHit(const Vec3& origin, const Vec3& direction, const RaycastHit& hitInfo, float maxDistance = 1000.0f);
	bool WorldToScreen(Vec3 worldPos, Vec2& screenPos);
	GameObject* RaycastPick(const Vec2& screenPos);

	bool SafeGetComponentEnabled(UT::Component* comp, bool& outEnabled);
	bool SafeSetComponentEnabled(UT::Component* comp, bool value);
	bool CaseInsensitiveFind(std::string_view haystack, std::string_view lowerNeedle);
}
