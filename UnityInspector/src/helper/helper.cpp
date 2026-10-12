#include "pch.h"
#include "config/config.h"
#include "helper.h"

namespace Helper
{
	template <typename T> bool SafeRead(void* ptr, const int offset, T& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	template <typename T> bool SafeWrite(void* ptr, const int offset, const T& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	template <typename T> bool StaticFieldAccess(void* fieldHandle, T* value, const bool set)
	{
		if (!fieldHandle) return false;
		try
		{
			if (Config::state.unityMode == UnityResolve::Mode::Mono)
			{
				void* vTable = Helper::GetMonoStaticVTable(fieldHandle);
				UR::Invoke<void, void*, void*, T*>(set ? "mono_field_static_set_value" : "mono_field_static_get_value",
				                                   vTable, fieldHandle, value);
			}
			else
			{
				UR::Invoke<void, void*, T*>(set ? "il2cpp_field_static_set_value" : "il2cpp_field_static_get_value",
				                            fieldHandle, value);
			}
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	template <typename T> bool StaticFieldGet(void* fieldHandle, T& outValue)
	{
		return StaticFieldAccess(fieldHandle, &outValue, false);
	}

	template <typename T> bool StaticFieldSet(void* fieldHandle, T value)
	{
		return StaticFieldAccess(fieldHandle, &value, true);
	}

	bool IsValidUserPointer(void* ptr)
	{
		if (!ptr) return false;
		const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
		if (addr < 0x10000) return false;
		if (addr > 0x7FFFFFFFFFFF) return false;
		if (addr == 0xCCCCCCCCCCCCCCCCull || addr == 0xDDDDDDDDDDDDDDDDull || addr == 0xFEEEFEEEFEEEFEEEull ||
		    addr == 0xBAADF00DBAADF00Dull)
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
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteInt(void* ptr, const int offset, const int value)
	{
		return SafeWrite<int>(ptr, offset, value);
	}

	bool SafeReadInt64(void* ptr, const int offset, int64_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteInt64(void* ptr, const int offset, const int64_t value)
	{
		return SafeWrite<int64_t>(ptr, offset, value);
	}

	bool SafeReadUInt64(void* ptr, const int offset, uint64_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteUInt64(void* ptr, const int offset, const uint64_t value)
	{
		return SafeWrite<uint64_t>(ptr, offset, value);
	}

	bool SafeReadByte(void* ptr, const int offset, uint8_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteByte(void* ptr, const int offset, const uint8_t value)
	{
		return SafeWrite<uint8_t>(ptr, offset, value);
	}

	bool SafeReadSByte(void* ptr, const int offset, int8_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteSByte(void* ptr, const int offset, const int8_t value)
	{
		return SafeWrite<int8_t>(ptr, offset, value);
	}

	bool SafeReadInt16(void* ptr, const int offset, int16_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteInt16(void* ptr, const int offset, const int16_t value)
	{
		return SafeWrite<int16_t>(ptr, offset, value);
	}

	bool SafeReadUInt16(void* ptr, const int offset, uint16_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteUInt16(void* ptr, const int offset, const uint16_t value)
	{
		return SafeWrite<uint16_t>(ptr, offset, value);
	}

	bool SafeReadChar(void* ptr, const int offset, char16_t& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteChar(void* ptr, const int offset, const char16_t value)
	{
		return SafeWrite<char16_t>(ptr, offset, value);
	}

	bool SafeReadFloat(void* ptr, const int offset, float& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteFloat(void* ptr, const int offset, const float value)
	{
		return SafeWrite<float>(ptr, offset, value);
	}

	bool SafeReadDouble(void* ptr, const int offset, double& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteDouble(void* ptr, const int offset, double value)
	{
		return SafeWrite<double>(ptr, offset, value);
	}

	bool SafeReadBool(void* ptr, const int offset, bool& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteBool(void* ptr, const int offset, const bool value)
	{
		return SafeWrite<bool>(ptr, offset, value);
	}

	bool SafeReadVector2(void* ptr, const int offset, Vec2& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteVector2(void* ptr, const int offset, const Vec2& value)
	{
		return SafeWrite<Vec2>(ptr, offset, value);
	}

	bool SafeReadVector3(void* ptr, const int offset, Vec3& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteVector3(void* ptr, const int offset, const Vec3& value)
	{
		return SafeWrite<Vec3>(ptr, offset, value);
	}

	bool SafeReadVector4(void* ptr, const int offset, Vec4& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteVector4(void* ptr, const int offset, const Vec4& value)
	{
		return SafeWrite<Vec4>(ptr, offset, value);
	}

	bool SafeReadQuaternion(void* ptr, const int offset, Quat& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteQuaternion(void* ptr, const int offset, const Quat& value)
	{
		return SafeWrite<Quat>(ptr, offset, value);
	}

	bool SafeReadColor(void* ptr, const int offset, Color& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeWriteColor(void* ptr, const int offset, const Color& value)
	{
		return SafeWrite<Color>(ptr, offset, value);
	}

	bool SafeReadStringPtr(void* ptr, const int offset, UT::String*& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeReadPointer(void* ptr, const int offset, void*& outValue)
	{
		return SafeRead(ptr, offset, outValue);
	}

	bool SafeGetStaticFieldInt(void* fieldHandle, int& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldInt(void* fieldHandle, int value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldInt64(void* fieldHandle, int64_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldInt64(void* fieldHandle, int64_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldUInt64(void* fieldHandle, uint64_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldUInt64(void* fieldHandle, uint64_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldByte(void* fieldHandle, uint8_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldByte(void* fieldHandle, uint8_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldSByte(void* fieldHandle, int8_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldSByte(void* fieldHandle, int8_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldInt16(void* fieldHandle, int16_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldInt16(void* fieldHandle, int16_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldUInt16(void* fieldHandle, uint16_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldUInt16(void* fieldHandle, uint16_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldChar(void* fieldHandle, char16_t& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldChar(void* fieldHandle, char16_t value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldFloat(void* fieldHandle, float& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldFloat(void* fieldHandle, float value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldBool(void* fieldHandle, bool& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldBool(void* fieldHandle, bool value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldDouble(void* fieldHandle, double& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldDouble(void* fieldHandle, double value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldVector3(void* fieldHandle, Vec3& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldVector3(void* fieldHandle, const Vec3& value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldVector2(void* fieldHandle, Vec2& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldVector2(void* fieldHandle, const Vec2& value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldVector4(void* fieldHandle, Vec4& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldVector4(void* fieldHandle, const Vec4& value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldQuaternion(void* fieldHandle, Quat& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldQuaternion(void* fieldHandle, const Quat& value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool SafeGetStaticFieldColor(void* fieldHandle, Color& outValue)
	{
		return StaticFieldGet(fieldHandle, outValue);
	}

	bool SafeSetStaticFieldColor(void* fieldHandle, const Color& value)
	{
		return StaticFieldSet(fieldHandle, value);
	}

	bool IsValidReadPtr(const void* ptr)
	{
		if (!ptr) return false;

		struct Region
		{
			uintptr_t begin = 0, end = 0;
		};
		thread_local std::array<Region, 8> recent{};
		thread_local size_t nextSlot = 0;

		const auto addr = reinterpret_cast<uintptr_t>(ptr);
		for (const auto& [begin, end] : recent)
		{
			if (addr >= begin && addr < end) return true;
		}

		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) return false;
		if (mbi.State != MEM_COMMIT) return false;
		if (mbi.Protect == PAGE_NOACCESS || (mbi.Protect & PAGE_GUARD)) return false;

		const auto begin = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
		recent[nextSlot++ % recent.size()] = {begin, begin + mbi.RegionSize};
		return true;
	}

	void* GetMonoStaticVTable(void* fieldHandle)
	{
		thread_local std::unordered_map<void*, void*> cache;
		if (const auto it = cache.find(fieldHandle); it != cache.end()) return it->second;

		void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
		                                               UR::Invoke<void*, void*>("mono_field_get_parent", fieldHandle));
		if (vTable) cache.emplace(fieldHandle, vTable);
		return vTable;
	}

	__declspec(noinline) void DoSafeGetStaticFieldPointer(void* fieldHandle, void*& outValue)
	{
		alignas(16) char safeBuffer[8192] = {};

		if (Config::state.unityMode == UnityResolve::Mode::Mono)
		{
			void* vTable = Helper::GetMonoStaticVTable(fieldHandle);
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
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
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
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return nullptr;
		}
	}

	__declspec(noinline) void* DoRuntimeInvoke(void* obj, void* methodHandle, void** params, bool& threw)
	{
		void* exc = nullptr;
		void* result = UR::Invoke<void*, void*, void*, void**, void**>(
		    Config::state.unityMode == UnityResolve::Mode::Mono ? "mono_runtime_invoke" : "il2cpp_runtime_invoke",
		    methodHandle, obj, params, &exc);
		threw = exc != nullptr;
		return threw ? nullptr : result;
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
			bool threw = false;
			void* result = DoRuntimeInvoke(obj, methodHandle, nullptr, threw);
			if (threw) return false;
			if (result && outValue)
			{
				if (void* unboxed = DoSafeUnbox(result)) memcpy(outValue, unboxed, valueSize);
			}
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	bool SafeInvokeGetterPointer(void* obj, void* methodHandle, void*& outPointer)
	{
		if (!methodHandle) return false;
		__try
		{
			bool threw = false;
			outPointer = DoRuntimeInvoke(obj, methodHandle, nullptr, threw);
			return !threw;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	bool SafeInvokeSetter(void* obj, void* methodHandle, void* value)
	{
		if (!methodHandle || !value) return false;
		__try
		{
			void* params[1] = {value};
			bool threw = false;
			DoRuntimeInvoke(obj, methodHandle, params, threw);
			return !threw;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	void* SafeInvokeMethod(void* obj, void* methodHandle, void** params, bool& success)
	{
		success = false;
		if (!methodHandle) return nullptr;
		__try
		{
			bool threw = false;
			void* result = DoRuntimeInvoke(obj, methodHandle, params, threw);
			success = !threw;
			return result;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			success = false;
			return nullptr;
		}
	}

	std::string GetTypeName(void* type, const char* fallback)
	{
		if (!type) return fallback;
		const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
		char* name = UR::Invoke<char*, void*>(mono ? "mono_type_get_name" : "il2cpp_type_get_name", type);
		if (!name) return fallback;
		std::string result = name;
		UR::Invoke<void, void*>(mono ? "mono_free" : "il2cpp_free", name);
		return result;
	}

	std::string SafeObjectToString(void* obj)
	{
		void* klass = SafeGetObjectClass(obj);
		if (!klass) return "";

		const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
		const char* className =
		    UR::Invoke<const char*, void*>(mono ? "mono_class_get_name" : "il2cpp_class_get_name", klass);
		const char* classNamespace =
		    UR::Invoke<const char*, void*>(mono ? "mono_class_get_namespace" : "il2cpp_class_get_namespace", klass);
		if (className && classNamespace && strcmp(className, "String") == 0 && strcmp(classNamespace, "System") == 0)
			return static_cast<UT::String*>(obj)->ToString();

		static void* baseToString = []() -> void*
		{
			const auto* corlib = UR::Get("mscorlib.dll");
			auto* objectClass = corlib ? corlib->Get("Object", "System") : nullptr;
			auto* method = objectClass ? objectClass->Get<UR::Method>("ToString") : nullptr;
			return method ? method->address : nullptr;
		}();

		if (baseToString)
		{
			void* method = UR::Invoke<void*, void*, void*>(
			    mono ? "mono_object_get_virtual_method" : "il2cpp_object_get_virtual_method", obj, baseToString);
			if (void* str = nullptr; method && SafeInvokeGetterPointer(obj, method, str) && str)
				return static_cast<UT::String*>(str)->ToString();
		}
		return className ? className : "";
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
				case EditableType::Enum:
				{
					auto buf = std::make_unique<char[]>(sizeof(int));
					*reinterpret_cast<int*>(buf.get()) = std::stoi(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
				case EditableType::Decimal:
				{
					auto buf = std::make_unique<char[]>(16);
					int32_t parts[4] = {};
					const double val = std::stod(valueStr);
					const bool negative = val < 0;
					const double absVal = negative ? -val : val;
					const uint64_t scaled = static_cast<uint64_t>(absVal * 1.0);
					parts[0] = negative ? 0x80000000 : 0;
					parts[1] = static_cast<int32_t>(scaled >> 32);
					parts[2] = static_cast<int32_t>(scaled & 0xFFFFFFFF);
					parts[3] = 0;
					std::memcpy(buf.get(), parts, 16);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
				case EditableType::Vector2:
				{
					auto buf = std::make_unique<char[]>(sizeof(Vec2));
					auto& v = *reinterpret_cast<Vec2*>(buf.get());
					v.x = v.y = std::stof(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
				case EditableType::Vector3:
				{
					auto buf = std::make_unique<char[]>(sizeof(Vec3));
					auto& v = *reinterpret_cast<Vec3*>(buf.get());
					v.x = v.y = v.z = std::stof(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
				case EditableType::Vector4:
				case EditableType::Quaternion:
				case EditableType::Color:
				{
					auto buf = std::make_unique<char[]>(16);
					auto* f = reinterpret_cast<float*>(buf.get());
					f[0] = f[1] = f[2] = f[3] = std::stof(valueStr);
					result.params.push_back(buf.get());
					result.buffers.push_back(std::move(buf));
					break;
				}
				default: result.params.push_back(nullptr); break;
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
			return true;
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
				const char* name =
				    UR::Invoke<const char*, void*>(mono ? "mono_method_get_name" : "il2cpp_method_get_name", method);
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
			currentClass =
			    UR::Invoke<void*, void*>(mono ? "mono_class_get_parent" : "il2cpp_class_get_parent", currentClass);
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

Helper::GcPin::GcPin(void* obj)
{
	if (!SafeGetObjectClass(obj)) return;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
	handle = UR::Invoke<uintptr_t, void*, int>(mono ? "mono_gchandle_new" : "il2cpp_gchandle_new", obj, 0);
	if (handle) target = obj;
}

Helper::GcPin::~GcPin()
{
	if (!handle || g_ShuttingDown) return;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
	UR::Invoke<void, uintptr_t>(mono ? "mono_gchandle_free" : "il2cpp_gchandle_free", handle);
}
