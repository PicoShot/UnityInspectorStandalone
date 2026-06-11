#include "pch.h"
#include "helper.h"
#include "config/config.h"

namespace Helper
{
	bool IsValidUserPointer(void* ptr)
	{
		if (!ptr) return false;
		const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
		if (addr < 0x10000) return false;
		if (addr > 0x7FFFFFFFFFFF) return false;
		if (addr == 0xCCCCCCCCCCCCCCCCull || addr == 0xDDDDDDDDDDDDDDDDull ||
		    addr == 0xFEEEFEEEFEEEFEEEull || addr == 0xBAADF00DBAADF00Dull)
			return false;
		return true;
	}

	bool SafeIsAlive(UnityObject* obj)
	{
		if (!IsValidUserPointer(obj)) return false;

		__try
		{
			return obj->IsAlive();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	bool SafeGetTag(GameObject* obj, UT::String*& outTag)
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

	bool SafeGetActiveSelf(GameObject* obj, bool& outActive)
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

	bool SafeSetActive(const GameObject* obj, bool value)
	{
		if (!obj) return false;

		__try
		{
			obj->SetActive(value);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	bool SafeGetIsStatic(GameObject* obj, bool& outStatic)
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

	bool SafeReadInt(void* ptr, const int offset, int& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteInt(void* ptr, const int offset, const int value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadInt64(void* ptr, const int offset, int64_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<int64_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteInt64(void* ptr, const int offset, const int64_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<int64_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadUInt64(void* ptr, const int offset, uint64_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteUInt64(void* ptr, const int offset, const uint64_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadByte(void* ptr, const int offset, uint8_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteByte(void* ptr, const int offset, const uint8_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadSByte(void* ptr, const int offset, int8_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<int8_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteSByte(void* ptr, const int offset, const int8_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<int8_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadInt16(void* ptr, const int offset, int16_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<int16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteInt16(void* ptr, const int offset, const int16_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<int16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadUInt16(void* ptr, const int offset, uint16_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteUInt16(void* ptr, const int offset, const uint16_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadChar(void* ptr, const int offset, char16_t& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<char16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteChar(void* ptr, const int offset, const char16_t value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<char16_t*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadFloat(void* ptr, const int offset, float& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteFloat(void* ptr, const int offset, const float value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadDouble(void* ptr, const int offset, double& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteDouble(void* ptr, const int offset, double value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadBool(void* ptr, const int offset, bool& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteBool(void* ptr, const int offset, const bool value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadVector2(void* ptr, const int offset, Vec2& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<Vec2*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteVector2(void* ptr, const int offset, const Vec2& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<Vec2*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadVector3(void* ptr, const int offset, Vec3& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteVector3(void* ptr, const int offset, const Vec3& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadVector4(void* ptr, const int offset, Vec4& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<Vec4*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteVector4(void* ptr, const int offset, const Vec4& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<Vec4*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadQuaternion(void* ptr, const int offset, Quat& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<Quat*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteQuaternion(void* ptr, const int offset, const Quat& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<Quat*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadColor(void* ptr, const int offset, Color& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<Color*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeWriteColor(void* ptr, const int offset, const Color& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<Color*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadStringPtr(void* ptr, const int offset, UT::String*& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<UT::String**>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadPointer(void* ptr, const int offset, void*& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeGetStaticFieldInt(void* fieldHandle, int& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeSetStaticFieldInt(void* fieldHandle, int value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeGetStaticFieldInt64(void* fieldHandle, int64_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int64_t*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, int64_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldInt64(void* fieldHandle, int64_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int64_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, int64_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldUInt64(void* fieldHandle, uint64_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint64_t
				           *>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, uint64_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldUInt64(void* fieldHandle, uint64_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint64_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, uint64_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldByte(void* fieldHandle, uint8_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint8_t*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, uint8_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldByte(void* fieldHandle, uint8_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint8_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, uint8_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldSByte(void* fieldHandle, int8_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int8_t*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, int8_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldSByte(void* fieldHandle, int8_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int8_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, int8_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldInt16(void* fieldHandle, int16_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int16_t*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, int16_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldInt16(void* fieldHandle, int16_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, int16_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, int16_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldUInt16(void* fieldHandle, uint16_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint16_t
				           *>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, uint16_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldUInt16(void* fieldHandle, uint16_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, uint16_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, uint16_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldChar(void* fieldHandle, char16_t& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, char16_t
				           *>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, char16_t*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldChar(void* fieldHandle, char16_t value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, char16_t*>("mono_field_static_set_value", vTable, fieldHandle, &value);
			}
			else
			{
				UR::Invoke<void, void*, char16_t*>("il2cpp_field_static_set_value", fieldHandle, &value);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldFloat(void* fieldHandle, float& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeSetStaticFieldFloat(void* fieldHandle, float value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeGetStaticFieldBool(void* fieldHandle, bool& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeSetStaticFieldBool(void* fieldHandle, bool value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeGetStaticFieldDouble(void* fieldHandle, double& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeSetStaticFieldDouble(void* fieldHandle, double value)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
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

	bool SafeGetStaticFieldVector3(void* fieldHandle, Vec3& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec3*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, Vec3*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldVector3(void* fieldHandle, const Vec3& value)
	{
		if (!fieldHandle) return false;
		try
		{
			Vec3 v = value;
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec3*>("mono_field_static_set_value", vTable, fieldHandle, &v);
			}
			else
			{
				UR::Invoke<void, void*, Vec3*>("il2cpp_field_static_set_value", fieldHandle, &v);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldVector2(void* fieldHandle, Vec2& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec2*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, Vec2*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldVector2(void* fieldHandle, const Vec2& value)
	{
		if (!fieldHandle) return false;
		try
		{
			Vec2 v = value;
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec2*>("mono_field_static_set_value", vTable, fieldHandle, &v);
			}
			else
			{
				UR::Invoke<void, void*, Vec2*>("il2cpp_field_static_set_value", fieldHandle, &v);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldVector4(void* fieldHandle, Vec4& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec4*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, Vec4*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldVector4(void* fieldHandle, const Vec4& value)
	{
		if (!fieldHandle) return false;
		try
		{
			Vec4 v = value;
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Vec4*>("mono_field_static_set_value", vTable, fieldHandle, &v);
			}
			else
			{
				UR::Invoke<void, void*, Vec4*>("il2cpp_field_static_set_value", fieldHandle, &v);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldQuaternion(void* fieldHandle, Quat& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Quat*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, Quat*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldQuaternion(void* fieldHandle, const Quat& value)
	{
		if (!fieldHandle) return false;
		try
		{
			Quat v = value;
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Quat*>("mono_field_static_set_value", vTable, fieldHandle, &v);
			}
			else
			{
				UR::Invoke<void, void*, Quat*>("il2cpp_field_static_set_value", fieldHandle, &v);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeGetStaticFieldColor(void* fieldHandle, Color& outValue)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Color*>("mono_field_static_get_value", vTable, fieldHandle, &outValue);
			}
			else
			{
				UR::Invoke<void, void*, Color*>("il2cpp_field_static_get_value", fieldHandle, &outValue);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool SafeSetStaticFieldColor(void* fieldHandle, const Color& value)
	{
		if (!fieldHandle) return false;
		try
		{
			Color v = value;
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", fieldHandle));
				UR::Invoke<void, void*, void*, Color*>("mono_field_static_set_value", vTable, fieldHandle, &v);
			}
			else
			{
				UR::Invoke<void, void*, Color*>("il2cpp_field_static_set_value", fieldHandle, &v);
			}
			return true;
		}
		catch (...) { return false; }
	}

	bool IsValidReadPtr(const void* ptr)
	{
		if (!ptr) return false;
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) return false;
		if (mbi.State != MEM_COMMIT) return false;
		if (mbi.Protect == PAGE_NOACCESS || (mbi.Protect & PAGE_GUARD)) return false;
		return true;
	}

	__declspec(noinline) void DoSafeGetStaticFieldPointer(void* fieldHandle, void*& outValue)
	{
		// Allocate a large buffer on the stack to prevent buffer overflows if the static field is a large struct (Value Type).
		// il2cpp_field_static_get_value writes the entire struct into the provided pointer. If it's an object (Reference Type), it writes 8 bytes.
		alignas(16) char safeBuffer[8192] = {};

		if (Config::state.unityMode == UnityResolve::Mode::Mono)
		{
			void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
			                                               UR::Invoke<void*, void*>(
				                                               "mono_field_get_parent", fieldHandle));
			UR::Invoke<void, void*, void*, void*>("mono_field_static_get_value", vTable, fieldHandle, safeBuffer);
		}
		else
		{
			UR::Invoke<void, void*, void*>("il2cpp_field_static_get_value", fieldHandle, safeBuffer);
		}

		outValue = *reinterpret_cast<void**>(safeBuffer);
	}

	bool SafeGetStaticFieldPointer(void* fieldHandle, void*& outValue)
	{
		if (!fieldHandle) return false;
		__try
		{
			DoSafeGetStaticFieldPointer(fieldHandle, outValue);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	__declspec(noinline) void* DoGetObjectClass(void* obj, bool isMono)
	{
		if (isMono)
		{
			return UR::Invoke<void*, void*>("mono_object_get_class", obj);
		}
		return UR::Invoke<void*, void*>("il2cpp_object_get_class", obj);
	}

	void* SafeGetObjectClass(void* obj)
	{
		if (!IsValidReadPtr(obj)) return nullptr;
		__try
		{
			return DoGetObjectClass(obj, Config::state.unityMode == UnityResolve::Mode::Mono);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
	}

	__declspec(noinline) void* DoSafeInvokeGetter(void* obj, void* methodHandle)
	{
		if (Config::state.unityMode == UnityResolve::Mode::Mono)
		{
			return UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, nullptr,
			                                                      nullptr);
		}
		return UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, nullptr,
		                                                      nullptr);
	}

	__declspec(noinline) void* DoSafeUnbox(void* result)
	{
		return UR::Invoke<void*, void*>(
			Config::state.unityMode == UnityResolve::Mode::Mono ? "mono_object_unbox" : "il2cpp_object_unbox", result);
	}

	bool SafeInvokeGetter(void* obj, void* methodHandle, void* outValue, int valueSize)
	{
		if (!methodHandle) return false;
		__try
		{
			if (void* result = DoSafeInvokeGetter(obj, methodHandle); result && outValue)
			{
				if (void* unboxed = DoSafeUnbox(result))
				{
					memcpy(outValue, unboxed, valueSize);
				}
			}
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeInvokeGetterPointer(void* obj, void* methodHandle, void*& outPointer)
	{
		if (!methodHandle) return false;
		__try
		{
			outPointer = DoSafeInvokeGetter(obj, methodHandle);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	__declspec(noinline) void DoSafeInvokeSetter(void* obj, void* methodHandle, void* value)
	{
		void* params[1] = {value};
		if (Config::state.unityMode == UnityResolve::Mode::Mono)
		{
			UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, params, nullptr);
		}
		else
		{
			UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, params, nullptr);
		}
	}

	bool SafeInvokeSetter(void* obj, void* methodHandle, void* value)
	{
		if (!methodHandle || !value) return false;
		__try
		{
			DoSafeInvokeSetter(obj, methodHandle, value);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	__declspec(noinline) void* DoSafeInvokeMethod(void* obj, void* methodHandle, void** params)
	{
		if (Config::state.unityMode == UnityResolve::Mode::Mono)
		{
			return UR::Invoke<void*, void*, void*, void**, void*>("mono_runtime_invoke", methodHandle, obj, params,
			                                                      nullptr);
		}
		return UR::Invoke<void*, void*, void*, void**, void*>("il2cpp_runtime_invoke", methodHandle, obj, params,
		                                                      nullptr);
	}

	void* SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success)
	{
		success = false;
		if (!methodHandle) return nullptr;
		__try
		{
			void* result = DoSafeInvokeMethod(obj, methodHandle, params);
			success = true;
			return result;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			success = false;
			return nullptr;
		}
	}

	InvokeParamBuffers BuildInvokeParams(const std::vector<std::string>& paramValues,
	                                     const std::vector<EditableType>& paramTypes)
	{
		InvokeParamBuffers result;
		for (size_t i = 0; i < paramValues.size() && i < paramTypes.size(); i++)
		{
			const std::string& valueStr = paramValues[i];
			switch (paramTypes[i])
			{
			case EditableType::Int:
				{
					auto buf = std::make_unique<char[]>(sizeof(int));
					*reinterpret_cast<int*>(buf.get()) = std::stoi(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
			case EditableType::Float:
				{
					auto buf = std::make_unique<char[]>(sizeof(float));
					*reinterpret_cast<float*>(buf.get()) = std::stof(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
			case EditableType::Double:
				{
					auto buf = std::make_unique<char[]>(sizeof(double));
					*reinterpret_cast<double*>(buf.get()) = std::stod(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
			case EditableType::Bool:
				{
					auto buf = std::make_unique<char[]>(sizeof(bool));
					*reinterpret_cast<bool*>(buf.get()) = (valueStr == "true" || valueStr == "1");
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
			case EditableType::String:
				{
					UT::String* str = UT::String::New(valueStr);
					auto buf = std::make_unique<char[]>(sizeof(void*));
					*reinterpret_cast<void**>(buf.get()) = str;
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
			default:
				result.params.push_back(nullptr);
				break;
			}
		}
		return result;
	}

	bool SafeGetComponents(GameObject* obj, UR::Class* componentClass, std::vector<UT::Component*>& outComponents)
	{
		outComponents.clear();
		if (!obj || !componentClass) return false;
		try
		{
			outComponents = obj->GetComponents<UT::Component*>(componentClass);
			return true;
		}
		catch (...)
		{
			outComponents.clear();
			return false;
		}
	}

	bool SafeGetGameObject(Transform* transform, GameObject*& outGameObject)
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

	bool SafeGetGameObject(MonoBehaviour* mb, GameObject*& outGameObject)
	{
		if (!mb) return false;
		__try
		{
			outGameObject = mb->GetGameObject();
			return outGameObject != nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			outGameObject = nullptr;
			return false;
		}
	}

	bool SafeGetGameObject(Component* comp, GameObject*& outGameObject)
	{
		if (!comp) return false;
		__try
		{
			outGameObject = comp->GetGameObject();
			return outGameObject != nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			outGameObject = nullptr;
			return false;
		}
	}

	bool SafeGetGameObject(Rigidbody* rb, GameObject*& outGameObject)
	{
		if (!rb) return false;
		__try
		{
			outGameObject = rb->GetGameObject();
			return outGameObject != nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			outGameObject = nullptr;
			return false;
		}
	}

	bool SafeGetTransform(GameObject* gameObject, Transform*& outTransform)
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

	bool SafeGetTransform(Rigidbody* gameObject, Transform*& outTransform)
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

	bool SafeGetParent(Transform* transform, Transform*& outParent)
	{
		if (!IsValidUserPointer(transform)) return false;
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

	bool SafeGetName(UnityObject* obj, UT::String*& outName)
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

	bool SafeGetChildCount(Transform* transform, int& outCount)
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

	bool SafeGetChild(Transform* transform, int index, Transform*& outChild)
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

	bool TryGetPosition(Transform* go, Vec3& outPos)
	{
		__try
		{
			outPos = go->GetPosition();
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return {};
		}
	}

	Camera* GetMainCamera()
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

	bool RayCast(const Vec3& origin, const Vec3& direction, float maxDistance)
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

	bool RayCastHit(const Vec3& origin, const Vec3& direction, const RaycastHit& hitInfo, float maxDistance)
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

	bool WorldToScreen(const Vec3 worldPos, Vec2& screenPos)
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

			return true;
			//(screenPos.x >= 0 && screenPos.x <= screen_size.x && screenPos.y >= 0 && screenPos.y <= screen_size.y);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	static Camera* SafeGetMainCamera()
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

	static bool SafePhysicsRaycast(const Ray& ray, const RaycastHit* hit)
	{
		__try
		{
			return Physics::Raycast(ray, hit, 1000000000.f);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	static UnityObject* SafeFindObjectFromInstanceID(int32_t instanceID)
	{
		__try
		{
			return UnityObject::FindObjectFromInstanceID(instanceID);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return nullptr;
		}
	}

	GameObject* RaycastPick(const Vec2& screenPos)
	{
		const auto camera = SafeGetMainCamera();
		if (!camera) return nullptr;

		const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		const Vec2 unityScreenPos = {screenPos.x, screenSize.y - screenPos.y};

		const Ray ray = camera->ScreenPointToRay(unityScreenPos);

		RaycastHit hit{};
		if (!SafePhysicsRaycast(ray, &hit)) return nullptr;
		if (hit.m_Collider == 0) return nullptr;

		auto* obj = SafeFindObjectFromInstanceID(hit.m_Collider);
		if (!obj) return nullptr;

		GameObject* go = nullptr;
		if (auto* collider = reinterpret_cast<Collider*>(obj); SafeGetGameObject(collider, go) && go) return go;

		if (auto* comp = reinterpret_cast<Component*>(obj); SafeGetGameObject(comp, go) && go) return go;

		return nullptr;
	}

	static void* FindMethodInHierarchy(void* obj, const char* methodName, int paramCount)
	{
		if (!obj) return nullptr;

		const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
		void* klass = SafeGetObjectClass(obj);
		if (!klass) return nullptr;

		void* currentClass = klass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* method;
			while ((method = UR::Invoke<void*, void*, void*>(
				mono ? "mono_class_get_methods" : "il2cpp_class_get_methods", currentClass, &iter)))
			{
				const char* name = UR::Invoke<const char*, void*>(
					mono ? "mono_method_get_name" : "il2cpp_method_get_name", method);
				if (name && strcmp(name, methodName) == 0)
				{
					if (paramCount < 0) return method;
					int actualParamCount = 0;
					if (mono)
					{
						if (void* sig = UR::Invoke<void*, void*>("mono_method_signature", method))
							actualParamCount = UR::Invoke<int, void*>("mono_signature_get_param_count", sig);
					}
					else
					{
						actualParamCount = UR::Invoke<int, void*>("il2cpp_method_get_param_count", method);
					}
					if (actualParamCount == paramCount) return method;
				}
			}
			currentClass = UR::Invoke<void*, void*>(mono ? "mono_class_get_parent" : "il2cpp_class_get_parent",
			                                        currentClass);
		}
		return nullptr;
	}

	bool SafeGetComponentEnabled(UT::Component* comp, bool& outEnabled)
	{
		outEnabled = true;
		if (!comp) return false;

		void* method = FindMethodInHierarchy(comp, "get_enabled", 0);
		if (!method) return false;

		return SafeInvokeGetter(comp, method, &outEnabled, sizeof(bool));
	}

	bool SafeSetComponentEnabled(UT::Component* comp, bool value)
	{
		if (!comp) return false;

		void* method = FindMethodInHierarchy(comp, "set_enabled", 1);
		if (!method) return false;

		return SafeInvokeSetter(comp, method, &value);
	}

	bool CaseInsensitiveFind(std::string_view haystack, std::string_view lowerNeedle)
	{
		if (lowerNeedle.empty()) return true;
		if (haystack.size() < lowerNeedle.size()) return false;

		for (size_t i = 0; i <= haystack.size() - lowerNeedle.size(); ++i)
		{
			bool match = true;
			for (size_t j = 0; j < lowerNeedle.size(); ++j)
			{
				if (static_cast<char>(std::tolower(static_cast<unsigned char>(haystack[i + j]))) != lowerNeedle[j])
				{
					match = false;
					break;
				}
			}
			if (match) return true;
		}
		return false;
	}
}
