#include "pch.h"
#include "lua_bindings.h"
#include "features/debug_console/debug_console.h"

using UT = UnityResolve::UnityType;

namespace LuaBindings
{
	static void RegisterMathTypes(sol::state& lua)
	{
		lua.new_usertype<UT::Vector2>("Vector2",
		                              sol::constructors<UT::Vector2(), UT::Vector2(float, float)>(),
		                              "x", &UT::Vector2::x,
		                              "y", &UT::Vector2::y,
		                              "Distance", [](const UT::Vector2& a, const UT::Vector2& b)
		                              {
			                              return a.Distance(b);
		                              },
		                              sol::meta_function::addition, [](const UT::Vector2& a, const UT::Vector2& b)
		                              {
			                              return UT::Vector2(a.x + b.x, a.y + b.y);
		                              },
		                              sol::meta_function::subtraction, [](const UT::Vector2& a, const UT::Vector2& b)
		                              {
			                              return UT::Vector2(a.x - b.x, a.y - b.y);
		                              },
		                              sol::meta_function::multiplication, sol::overload(
			                              [](const UT::Vector2& a, float s) { return UT::Vector2(a.x * s, a.y * s); },
			                              [](const UT::Vector2& a, const UT::Vector2& b)
			                              {
				                              return UT::Vector2(a.x * b.x, a.y * b.y);
			                              }
		                              ),
		                              sol::meta_function::division, [](const UT::Vector2& a, float s)
		                              {
			                              return UT::Vector2(a.x / s, a.y / s);
		                              },
		                              sol::meta_function::equal_to, [](const UT::Vector2& a, const UT::Vector2& b)
		                              {
			                              return a.x == b.x && a.y == b.y;
		                              }
		);

		lua.new_usertype<UT::Vector3>("Vector3",
		                              sol::constructors<UT::Vector3(), UT::Vector3(float, float, float)>(),
		                              "x", &UT::Vector3::x,
		                              "y", &UT::Vector3::y,
		                              "z", &UT::Vector3::z,
		                              "Length", [](const UT::Vector3& v) { return v.Length(); },
		                              "Dot", [](const UT::Vector3& a, const UT::Vector3& b) { return a.Dot(b); },
		                              "Normalize", [](const UT::Vector3& v) { return v.Normalize(); },
		                              "Distance", [](const UT::Vector3& a, const UT::Vector3& b)
		                              {
			                              return a.Distance(b);
		                              },
		                              sol::meta_function::addition, [](const UT::Vector3& a, const UT::Vector3& b)
		                              {
			                              return UT::Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
		                              },
		                              sol::meta_function::subtraction, [](const UT::Vector3& a, const UT::Vector3& b)
		                              {
			                              return UT::Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
		                              },
		                              sol::meta_function::multiplication, sol::overload(
			                              [](const UT::Vector3& a, float s)
			                              {
				                              return UT::Vector3(a.x * s, a.y * s, a.z * s);
			                              },
			                              [](const UT::Vector3& a, const UT::Vector3& b)
			                              {
				                              return UT::Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
			                              }
		                              ),
		                              sol::meta_function::division, [](const UT::Vector3& a, float s)
		                              {
			                              return UT::Vector3(a.x / s, a.y / s, a.z / s);
		                              },
		                              sol::meta_function::equal_to, [](const UT::Vector3& a, const UT::Vector3& b)
		                              {
			                              return a.x == b.x && a.y == b.y && a.z == b.z;
		                              }
		);

		lua.new_usertype<UT::Vector4>("Vector4",
		                              sol::constructors<UT::Vector4(), UT::Vector4(float, float, float, float)>(),
		                              "x", &UT::Vector4::x,
		                              "y", &UT::Vector4::y,
		                              "z", &UT::Vector4::z,
		                              "w", &UT::Vector4::w,
		                              sol::meta_function::addition, [](const UT::Vector4& a, const UT::Vector4& b)
		                              {
			                              return UT::Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		                              },
		                              sol::meta_function::subtraction, [](const UT::Vector4& a, const UT::Vector4& b)
		                              {
			                              return UT::Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		                              },
		                              sol::meta_function::multiplication, [](const UT::Vector4& a, float s)
		                              {
			                              return UT::Vector4(a.x * s, a.y * s, a.z * s, a.w * s);
		                              },
		                              sol::meta_function::division, [](const UT::Vector4& a, float s)
		                              {
			                              return UT::Vector4(a.x / s, a.y / s, a.z / s, a.w / s);
		                              }
		);

		lua.new_usertype<UT::Quaternion>("Quaternion",
		                                 sol::constructors<
			                                 UT::Quaternion(), UT::Quaternion(float, float, float, float)>(),
		                                 "x", &UT::Quaternion::x,
		                                 "y", &UT::Quaternion::y,
		                                 "z", &UT::Quaternion::z,
		                                 "w", &UT::Quaternion::w,
		                                 "Euler", [](float x, float y, float z)
		                                 {
			                                 return UT::Quaternion().Euler(x, y, z);
		                                 },
		                                 "ToEuler", [](const UT::Quaternion& q) { return q.ToEuler(); },
		                                 "LookRotation", [](const UT::Vector3& forward)
		                                 {
			                                 return UT::Quaternion::LookRotation(forward);
		                                 }
		);

		lua.new_usertype<UT::Color>("Color",
		                            sol::constructors<UT::Color(), UT::Color(float, float, float), UT::Color(
			                                              float, float, float, float)>(),
		                            "r", &UT::Color::r,
		                            "g", &UT::Color::g,
		                            "b", &UT::Color::b,
		                            "a", &UT::Color::a
		);

		lua.new_usertype<UT::Ray>("Ray",
		                          sol::constructors<UT::Ray()>(),
		                          "origin", &UT::Ray::m_vOrigin,
		                          "direction", &UT::Ray::m_vDirection
		);

		lua.new_usertype<UT::Matrix4x4>("Matrix4x4",
		                                sol::constructors<UT::Matrix4x4()>(),
		                                "Get", [](UT::Matrix4x4& m, int row, int col) -> float
		                                {
			                                if (row >= 0 && row < 4 && col >= 0 && col < 4) return m[row][col];
			                                return 0.0f;
		                                }
		);

		lua.new_usertype<UT::Bounds>("Bounds",
		                             "center", &UT::Bounds::m_vCenter,
		                             "extents", &UT::Bounds::m_vExtents
		);

		lua.new_usertype<UT::Plane>("Plane",
		                            "normal", &UT::Plane::m_vNormal,
		                            "distance", &UT::Plane::fDistance
		);

		lua.new_usertype<UT::Rect>("Rect",
		                           sol::constructors<UT::Rect(), UT::Rect(float, float, float, float)>(),
		                           "x", &UT::Rect::fX,
		                           "y", &UT::Rect::fY,
		                           "width", &UT::Rect::fWidth,
		                           "height", &UT::Rect::fHeight
		);

		lua.new_usertype<UT::RaycastHit>("RaycastHit",
		                                 "point", &UT::RaycastHit::m_Point,
		                                 "normal", &UT::RaycastHit::m_Normal,
		                                 "distance", &UT::RaycastHit::m_Distance,
		                                 "collider", &UT::RaycastHit::m_Collider
		);

		lua["MakeVector2"] = [](float x, float y) { return UT::Vector2(x, y); };
		lua["MakeVector3"] = [](float x, float y, float z) { return UT::Vector3(x, y, z); };
		lua["MakeVector4"] = [](float x, float y, float z, float w) { return UT::Vector4(x, y, z, w); };
		lua["MakeQuaternion"] = [](float x, float y, float z, float w) { return UT::Quaternion(x, y, z, w); };
		lua["MakeColor"] = [](float r, float g, float b, sol::optional<float> a)
		{
			return UT::Color(r, g, b, a.value_or(1.0f));
		};
		lua["MakeRay"] = [](const UT::Vector3& origin, const UT::Vector3& dir) { return UT::Ray{origin, dir}; };
		lua["MakeMatrix4x4"] = [] { return UT::Matrix4x4(); };
		lua["MakeBounds"] = [](const UT::Vector3& c, const UT::Vector3& e) { return UT::Bounds{c, e}; };
		lua["MakePlane"] = [](const UT::Vector3& n, float d) { return UT::Plane{n, d}; };
		lua["MakeRect"] = [](float x, float y, float w, float h) { return UT::Rect(x, y, w, h); };
	}

	static void RegisterUnityTypes(sol::state& lua)
	{
		auto unity = lua["Unity"].get_or_create<sol::table>();

		lua.new_usertype<UT::Object>("Object",
		                             "GetType", [](UT::Object* self) -> void*
		                             {
			                             if (!self) return nullptr;
			                             UR::ThreadAttach();
			                             return self->GetType();
		                             },
		                             "ToString", [](UT::Object* self) -> std::string
		                             {
			                             if (!self) return "";
			                             UR::ThreadAttach();
			                             if (auto* s = self->ToString()) return s->ToString();
			                             return "";
		                             },
		                             "GetHashCode", [](UT::Object* self) -> int
		                             {
			                             if (!self) return 0;
			                             UR::ThreadAttach();
			                             return self->GetHashCode();
		                             }
		);

		lua.new_usertype<UT::UnityObject>("UnityObject",
		                                  sol::base_classes, sol::bases<UT::Object>(),
		                                  "IsAlive", [](UT::UnityObject* self) -> bool
		                                  {
			                                  if (!self) return false;
			                                  UR::ThreadAttach();
			                                  return self->IsAlive();
		                                  },
		                                  "GetName", [](UT::UnityObject* self) -> std::string
		                                  {
			                                  if (!self) return "";
			                                  UR::ThreadAttach();
			                                  if (auto* s = self->GetName()) return s->ToString();
			                                  return "";
		                                  },
		                                  "ToString", [](UT::UnityObject* self) -> std::string
		                                  {
			                                  if (!self) return "";
			                                  UR::ThreadAttach();
			                                  if (auto* s = self->ToString()) return s->ToString();
			                                  return "";
		                                  },
		                                  "Destroy", [](UT::UnityObject* self)
		                                  {
			                                  if (!self) return;
			                                  UR::ThreadAttach();
			                                  UT::UnityObject::Destroy(self);
		                                  },
		                                  "DontDestroyOnLoad", [](UT::UnityObject* self)
		                                  {
			                                  if (!self) return;
			                                  UR::ThreadAttach();
			                                  UT::UnityObject::DontDestroyOnLoad(self);
		                                  }
		);

		lua.new_usertype<UT::String>("String",
		                             sol::base_classes, sol::bases<UT::Object>(),
		                             "ToString", [](const UT::String* self) -> std::string
		                             {
			                             if (!self) return "";
			                             return self->ToString();
		                             }
		);

		auto string_ns = unity["String"].get_or_create<sol::table>();
		string_ns["New"] = [](const std::string& str) -> UT::String*
		{
			UR::ThreadAttach();
			return UT::String::New(str);
		};

		lua.new_usertype<UT::GameObject>("GameObject",
		                                 sol::base_classes, sol::bases<UT::UnityObject, UT::Object>(),
		                                 "GetName", [](UT::GameObject* self) -> std::string
		                                 {
			                                 if (!self) return "";
			                                 UR::ThreadAttach();
			                                 if (auto* s = self->GetName()) return s->ToString();
			                                 return "";
		                                 },
		                                 "SetActive", [](const UT::GameObject* self, bool active)
		                                 {
			                                 if (!self) return;
			                                 UR::ThreadAttach();
			                                 self->SetActive(active);
		                                 },
		                                 "GetActiveSelf", [](UT::GameObject* self) -> bool
		                                 {
			                                 if (!self) return false;
			                                 UR::ThreadAttach();
			                                 return self->GetActiveSelf();
		                                 },
		                                 "GetActiveInHierarchy", [](UT::GameObject* self) -> bool
		                                 {
			                                 if (!self) return false;
			                                 UR::ThreadAttach();
			                                 return self->GetActiveInHierarchy();
		                                 },
		                                 "GetIsStatic", [](UT::GameObject* self) -> bool
		                                 {
			                                 if (!self) return false;
			                                 UR::ThreadAttach();
			                                 return self->GetIsStatic();
		                                 },
		                                 "GetTag", [](UT::GameObject* self) -> std::string
		                                 {
			                                 if (!self) return "";
			                                 UR::ThreadAttach();
			                                 if (auto* s = self->GetTag()) return s->ToString();
			                                 return "";
		                                 },
		                                 "GetTransform", [](UT::GameObject* self) -> UT::Transform*
		                                 {
			                                 if (!self) return nullptr;
			                                 UR::ThreadAttach();
			                                 return self->GetTransform();
		                                 },
		                                 "GetComponent", [](UT::GameObject* self, UR::Class* pClass) -> void*
		                                 {
			                                 if (!self || !pClass) return nullptr;
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponent", {"System.Type"});
			                                 if (method) return method->Invoke<void*>(self, pClass->GetType());
			                                 return nullptr;
		                                 },
		                                 "GetComponentInChildren", [](UT::GameObject* self, UR::Class* pClass) -> void*
		                                 {
			                                 if (!self || !pClass) return nullptr;
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponentInChildren", {"System.Type"});
			                                 if (method) return method->Invoke<void*>(self, pClass->GetType());
			                                 return nullptr;
		                                 },
		                                 "GetComponentInParent", [](UT::GameObject* self, UR::Class* pClass) -> void*
		                                 {
			                                 if (!self || !pClass) return nullptr;
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponentInParent", {"System.Type"});
			                                 if (method) return method->Invoke<void*>(self, pClass->GetType());
			                                 return nullptr;
		                                 },
		                                 "GetComponents",
		                                 [](UT::GameObject* self,
		                                    UR::Class* pClass) -> sol::as_table_t<std::vector<void*>>
		                                 {
			                                 if (!self || !pClass) return {};
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponentsInternal");
			                                 if (method)
			                                 {
				                                 auto* arr = method->Invoke<UT::Array<void*>*>(
					                                 self, pClass->GetType(), false, false, true, false, nullptr);
				                                 if (arr) return arr->ToVector();
			                                 }
			                                 return {};
		                                 },
		                                 "GetComponentsInChildren",
		                                 [](UT::GameObject* self, UR::Class* pClass,
		                                    sol::optional<bool> includeInactive) -> sol::as_table_t<std::vector<void*>>
		                                 {
			                                 if (!self || !pClass) return {};
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponentsInternal");
			                                 if (method)
			                                 {
				                                 auto* arr = method->Invoke<UT::Array<void*>*>(
					                                 self, pClass->GetType(), false, true,
					                                 includeInactive.value_or(false), false, nullptr);
				                                 if (arr) return arr->ToVector();
			                                 }
			                                 return {};
		                                 },
		                                 "GetComponentsInParent",
		                                 [](UT::GameObject* self, UR::Class* pClass,
		                                    sol::optional<bool> includeInactive) -> sol::as_table_t<std::vector<void*>>
		                                 {
			                                 if (!self || !pClass) return {};
			                                 UR::ThreadAttach();
			                                 static UR::Method* method;
			                                 if (!method)
				                                 method = UR::Get("UnityEngine.CoreModule.dll")->
				                                          Get("GameObject")->Get<UR::Method>(
					                                          "GetComponentsInternal");
			                                 if (method)
			                                 {
				                                 auto* arr = method->Invoke<UT::Array<void*>*>(
					                                 self, pClass->GetType(), false, true,
					                                 includeInactive.value_or(false), true, nullptr);
				                                 if (arr) return arr->ToVector();
			                                 }
			                                 return {};
		                                 }
		);

		auto go_ns = unity["GameObject"].get_or_create<sol::table>();
		go_ns["Find"] = [](const std::string& name) -> UT::GameObject*
		{
			UR::ThreadAttach();
			return UT::GameObject::Find(name);
		};
		go_ns["FindGameObjectsWithTag"] = [](const std::string& tag) -> sol::as_table_t<std::vector<UT::GameObject*>>
		{
			UR::ThreadAttach();
			return UT::GameObject::FindGameObjectsWithTag(tag);
		};
		go_ns["Create"] = [](const std::string& name) -> UT::GameObject*
		{
			UR::ThreadAttach();
			return UT::GameObject::Create(name);
		};

		lua.new_usertype<UT::Component>("Component",
		                                sol::base_classes, sol::bases<UT::UnityObject, UT::Object>(),
		                                "GetTransform", [](UT::Component* self) -> UT::Transform*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetTransform();
		                                },
		                                "GetGameObject", [](UT::Component* self) -> UT::GameObject*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetGameObject();
		                                },
		                                "GetTag", [](UT::Component* self) -> std::string
		                                {
			                                if (!self) return "";
			                                UR::ThreadAttach();
			                                if (auto* s = self->GetTag()) return s->ToString();
			                                return "";
		                                },
		                                "GetComponentInChildren", [](UT::Component* self, UR::Class* pClass) -> void*
		                                {
			                                if (!self || !pClass) return nullptr;
			                                UR::ThreadAttach();
			                                static UR::Method* method;
			                                if (!method)
				                                method = UR::Get("UnityEngine.CoreModule.dll")->
				                                         Get("Component")->Get<UR::Method>(
					                                         "GetComponentInChildren", {"System.Type"});
			                                if (method) return method->Invoke<void*>(self, pClass->GetType());
			                                return nullptr;
		                                },
		                                "GetComponentInParent", [](UT::Component* self, UR::Class* pClass) -> void*
		                                {
			                                if (!self || !pClass) return nullptr;
			                                UR::ThreadAttach();
			                                static UR::Method* method;
			                                if (!method)
				                                method = UR::Get("UnityEngine.CoreModule.dll")->
				                                         Get("Component")->Get<UR::Method>(
					                                         "GetComponentInParent", {"System.Type"});
			                                if (method) return method->Invoke<void*>(self, pClass->GetType());
			                                return nullptr;
		                                },
		                                "GetComponents",
		                                [](UT::Component* self,
		                                   UR::Class* pClass) -> sol::as_table_t<std::vector<void*>>
		                                {
			                                if (!self || !pClass) return {};
			                                UR::ThreadAttach();
			                                static UR::Method* method;
			                                if (!method)
				                                method = UR::Get("UnityEngine.CoreModule.dll")->
				                                         Get("Component")->Get<UR::Method>(
					                                         "GetComponents", {"System.Type"});
			                                if (method)
			                                {
				                                if (auto* arr = method->Invoke<UT::Array<void*>*>(
					                                self, pClass->GetType())) return arr->ToVector();
			                                }
			                                return {};
		                                },
		                                "GetComponentsInChildren",
		                                [](UT::Component* self,
		                                   UR::Class* pClass) -> sol::as_table_t<std::vector<void*>>
		                                {
			                                if (!self || !pClass) return {};
			                                UR::ThreadAttach();
			                                static UR::Method* method;
			                                if (!method)
				                                method = UR::Get("UnityEngine.CoreModule.dll")->
				                                         Get("Component")->Get<UR::Method>(
					                                         "GetComponentsInChildren", {"System.Type"});
			                                if (method)
			                                {
				                                if (auto* arr = method->Invoke<UT::Array<void*>*>(
					                                self, pClass->GetType())) return arr->ToVector();
			                                }
			                                return {};
		                                },
		                                "GetComponentsInParent",
		                                [](UT::Component* self,
		                                   UR::Class* pClass) -> sol::as_table_t<std::vector<void*>>
		                                {
			                                if (!self || !pClass) return {};
			                                UR::ThreadAttach();
			                                static UR::Method* method;
			                                if (!method)
				                                method = UR::Get("UnityEngine.CoreModule.dll")->
				                                         Get("Component")->Get<UR::Method>(
					                                         "GetComponentsInParent", {"System.Type"});
			                                if (method)
			                                {
				                                if (auto* arr = method->Invoke<UT::Array<void*>*>(
					                                self, pClass->GetType())) return arr->ToVector();
			                                }
			                                return {};
		                                }
		);

		lua.new_usertype<UT::Behaviour>("Behaviour",
		                                sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                                "GetEnabled", [](UT::Behaviour* self) -> bool
		                                {
			                                if (!self) return false;
			                                UR::ThreadAttach();
			                                return self->GetEnabled();
		                                },
		                                "SetEnabled", [](UT::Behaviour* self, bool value)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetEnabled(value);
		                                }
		);

		lua.new_usertype<UT::Transform>("Transform",
		                                sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                                "GetPosition", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetPosition();
		                                },
		                                "SetPosition", [](UT::Transform* self, const UT::Vector3& pos)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetPosition(pos);
		                                },
		                                "GetRotation", [](UT::Transform* self) -> UT::Quaternion
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetRotation();
		                                },
		                                "SetRotation", [](UT::Transform* self, const UT::Quaternion& q)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetRotation(q);
		                                },
		                                "GetLocalPosition", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetLocalPosition();
		                                },
		                                "SetLocalPosition", [](UT::Transform* self, const UT::Vector3& pos)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetLocalPosition(pos);
		                                },
		                                "GetLocalRotation", [](UT::Transform* self) -> UT::Quaternion
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetLocalRotation();
		                                },
		                                "SetLocalRotation", [](UT::Transform* self, const UT::Quaternion& q)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetLocalRotation(q);
		                                },
		                                "GetLocalScale", [](const UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetLocalScale();
		                                },
		                                "SetLocalScale", [](UT::Transform* self, const UT::Vector3& s)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetLocalScale(s);
		                                },
		                                "GetForward", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetForward();
		                                },
		                                "SetForward", [](UT::Transform* self, const UT::Vector3& v)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetForward(v);
		                                },
		                                "GetRight", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetRight();
		                                },
		                                "SetRight", [](UT::Transform* self, const UT::Vector3& v)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetRight(v);
		                                },
		                                "GetUp", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetUp();
		                                },
		                                "SetUp", [](UT::Transform* self, const UT::Vector3& v)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetUp(v);
		                                },
		                                "GetLossyScale", [](UT::Transform* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetLossyScale();
		                                },
		                                "TransformPoint", [](UT::Transform* self, const UT::Vector3& pos) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->TransformPoint(pos);
		                                },
		                                "LookAt", [](UT::Transform* self, const UT::Vector3& worldPosition)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->LookAt(worldPosition);
		                                },
		                                "Rotate", [](UT::Transform* self, const UT::Vector3& eulers)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->Rotate(eulers);
		                                },
		                                "GetChildCount", [](UT::Transform* self) -> int
		                                {
			                                if (!self) return 0;
			                                UR::ThreadAttach();
			                                return self->GetChildCount();
		                                },
		                                "GetChild", [](UT::Transform* self, int index) -> UT::Transform*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetChild(index);
		                                },
		                                "GetParent", [](UT::Transform* self) -> UT::Transform*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetParent();
		                                },
		                                "GetRoot", [](UT::Transform* self) -> UT::Transform*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetRoot();
		                                },
		                                "GetGameObject", [](UT::Transform* self) -> UT::GameObject*
		                                {
			                                if (!self) return nullptr;
			                                UR::ThreadAttach();
			                                return self->GetGameObject();
		                                }
		);

		lua.new_usertype<UT::Camera>("Camera",
		                             sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                             "GetDepth", [](UT::Camera* self) -> float
		                             {
			                             if (!self) return 0.0f;
			                             UR::ThreadAttach();
			                             return self->GetDepth();
		                             },
		                             "SetDepth", [](UT::Camera* self, float depth)
		                             {
			                             if (!self) return;
			                             UR::ThreadAttach();
			                             self->SetDepth(depth);
		                             },
		                             "GetFoV", [](UT::Camera* self) -> float
		                             {
			                             if (!self) return 0.0f;
			                             UR::ThreadAttach();
			                             return self->GetFoV();
		                             },
		                             "SetFoV", [](UT::Camera* self, float fov)
		                             {
			                             if (!self) return;
			                             UR::ThreadAttach();
			                             self->SetFoV(fov);
		                             },
		                             "GetAllCount", [](const UT::Camera* self) -> int
		                             {
			                             if (!self) return 0;
			                             UR::ThreadAttach();
			                             return UT::Camera::GetAllCount();
		                             },
		                             "WorldToScreenPoint", [](UT::Camera* self, const UT::Vector3& pos) -> UT::Vector3
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->WorldToScreenPoint(pos);
		                             },
		                             "ScreenToWorldPoint", [](UT::Camera* self, const UT::Vector3& pos) -> UT::Vector3
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->ScreenToWorldPoint(pos);
		                             },
		                             "ScreenPointToRay", [](UT::Camera* self, const UT::Vector2& pos) -> UT::Ray
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->ScreenPointToRay(pos);
		                             },
		                             "CameraToWorldMatrix", [](UT::Camera* self) -> UT::Matrix4x4
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->CameraToWorldMatrix();
		                             }
		);

		auto cam_ns = unity["Camera"].get_or_create<sol::table>();
		cam_ns["GetMain"] = []() -> UT::Camera*
		{
			UR::ThreadAttach();
			return UT::Camera::GetMain();
		};
		cam_ns["GetCurrent"] = []() -> UT::Camera*
		{
			UR::ThreadAttach();
			return UT::Camera::GetCurrent();
		};
		cam_ns["GetAllCamera"] = []() -> sol::as_table_t<std::vector<UT::Camera*>>
		{
			UR::ThreadAttach();
			return UT::Camera::GetAllCamera();
		};

		auto lightType = lua.create_table();
		lightType["Spot"] = UT::Light::LightType::Spot;
		lightType["Directional"] = UT::Light::LightType::Directional;
		lightType["Point"] = UT::Light::LightType::Point;
		lightType["Area"] = UT::Light::LightType::Area;
		lightType["Rectangle"] = UT::Light::LightType::Rectangle;
		lightType["Disc"] = UT::Light::LightType::Disc;
		lightType["Pyramid"] = UT::Light::LightType::Pyramid;
		lightType["Box"] = UT::Light::LightType::Box;
		lightType["Tube"] = UT::Light::LightType::Tube;
		unity["LightType"] = lightType;

		auto lightShadows = lua.create_table();
		lightShadows["None"] = UT::Light::LightShadows::None;
		lightShadows["Hard"] = UT::Light::LightShadows::Hard;
		lightShadows["Soft"] = UT::Light::LightShadows::Soft;
		unity["LightShadows"] = lightShadows;

		lua.new_usertype<UT::Light>("Light",
		                            sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                            "GetIntensity", [](UT::Light* self) -> float
		                            {
			                            if (!self) return 0.0f;
			                            UR::ThreadAttach();
			                            return self->GetIntensity();
		                            },
		                            "SetIntensity", [](UT::Light* self, float v)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetIntensity(v);
		                            },
		                            "GetColor", [](UT::Light* self) -> UT::Color
		                            {
			                            if (!self) return {};
			                            UR::ThreadAttach();
			                            return self->GetColor();
		                            },
		                            "SetColor", [](UT::Light* self, const UT::Color& c)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetColor(c);
		                            },
		                            "GetRange", [](UT::Light* self) -> float
		                            {
			                            if (!self) return 0.0f;
			                            UR::ThreadAttach();
			                            return self->GetRange();
		                            },
		                            "SetRange", [](UT::Light* self, float v)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetRange(v);
		                            },
		                            "GetLightType", [](UT::Light* self) -> UT::Light::LightType
		                            {
			                            if (!self) return UT::Light::LightType::Point;
			                            UR::ThreadAttach();
			                            return self->GetLightType();
		                            },
		                            "SetLightType", [](UT::Light* self, UT::Light::LightType type)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetLightType(type);
		                            },
		                            "GetSpotAngle", [](UT::Light* self) -> float
		                            {
			                            if (!self) return 0.0f;
			                            UR::ThreadAttach();
			                            return self->GetSpotAngle();
		                            },
		                            "SetSpotAngle", [](UT::Light* self, float angle)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetSpotAngle(angle);
		                            },
		                            "GetShadows", [](UT::Light* self) -> UT::Light::LightShadows
		                            {
			                            if (!self) return UT::Light::LightShadows::None;
			                            UR::ThreadAttach();
			                            return self->GetShadows();
		                            },
		                            "SetShadows", [](UT::Light* self, UT::Light::LightShadows shadowType)
		                            {
			                            if (!self) return;
			                            UR::ThreadAttach();
			                            self->SetShadows(shadowType);
		                            }
		);

		auto light_ns = unity["Light"].get_or_create<sol::table>();
		light_ns["FindAll"] = []() -> sol::as_table_t<std::vector<UT::Light*>>
		{
			UR::ThreadAttach();
			return sol::as_table(UT::Light::FindAll());
		};

		lua.new_usertype<UT::Rigidbody>("Rigidbody",
		                                sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                                "GetVelocity", [](UT::Rigidbody* self) -> UT::Vector3
		                                {
			                                if (!self) return {};
			                                UR::ThreadAttach();
			                                return self->GetVelocity();
		                                },
		                                "SetVelocity", [](UT::Rigidbody* self, const UT::Vector3& v)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetVelocity(v);
		                                },
		                                "GetDetectCollisions", [](UT::Rigidbody* self) -> bool
		                                {
			                                if (!self) return false;
			                                UR::ThreadAttach();
			                                return self->GetDetectCollisions();
		                                },
		                                "SetDetectCollisions", [](UT::Rigidbody* self, bool v)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->SetDetectCollisions(v);
		                                }
		);

		lua.new_usertype<UT::Collider>("Collider",
		                               sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                               "GetBounds", [](const UT::Collider* self) -> UT::Bounds
		                               {
			                               if (!self) return {};
			                               UR::ThreadAttach();
			                               return self->GetBounds();
		                               }
		);

		lua.new_usertype<UT::CapsuleCollider>("CapsuleCollider",
		                                      sol::base_classes,
		                                      sol::bases<UT::Collider, UT::Component, UT::UnityObject, UT::Object>(),
		                                      "GetCenter", [](UT::CapsuleCollider* self) -> UT::Vector3
		                                      {
			                                      if (!self) return {};
			                                      UR::ThreadAttach();
			                                      return self->GetCenter();
		                                      },
		                                      "GetDirection", [](UT::CapsuleCollider* self) -> UT::Vector3
		                                      {
			                                      if (!self) return {};
			                                      UR::ThreadAttach();
			                                      return self->GetDirection();
		                                      },
		                                      "GetHeight", [](UT::CapsuleCollider* self) -> UT::Vector3
		                                      {
			                                      if (!self) return {};
			                                      UR::ThreadAttach();
			                                      return self->GetHeightn();
		                                      },
		                                      "GetRadius", [](UT::CapsuleCollider* self) -> UT::Vector3
		                                      {
			                                      if (!self) return {};
			                                      UR::ThreadAttach();
			                                      return self->GetRadius();
		                                      }
		);

		lua.new_usertype<UT::BoxCollider>("BoxCollider",
		                                  sol::base_classes,
		                                  sol::bases<UT::Collider, UT::Component, UT::UnityObject, UT::Object>(),
		                                  "GetCenter", [](UT::BoxCollider* self) -> UT::Vector3
		                                  {
			                                  if (!self) return {};
			                                  UR::ThreadAttach();
			                                  return self->GetCenter();
		                                  },
		                                  "GetSize", [](UT::BoxCollider* self) -> UT::Vector3
		                                  {
			                                  if (!self) return {};
			                                  UR::ThreadAttach();
			                                  return self->GetSize();
		                                  }
		);

		lua.new_usertype<UT::Mesh>("Mesh",
		                           sol::base_classes, sol::bases<UT::UnityObject, UT::Object>(),
		                           "GetBounds", [](const UT::Mesh* self) -> UT::Bounds
		                           {
			                           if (!self) return {};
			                           UR::ThreadAttach();
			                           return self->GetBounds();
		                           }
		);

		lua.new_usertype<UT::Renderer>("Renderer",
		                               sol::base_classes, sol::bases<UT::Component, UT::UnityObject, UT::Object>(),
		                               "GetEnabled", [](UT::Renderer* self) -> bool
		                               {
			                               if (!self) return false;
			                               UR::ThreadAttach();
			                               return self->GetEnabled();
		                               },
		                               "SetEnabled", [](UT::Renderer* self, bool enable)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetEnabled(enable);
		                               },
		                               "GetMaterialCount", [](UT::Renderer* self) -> int
		                               {
			                               if (!self) return 0;
			                               UR::ThreadAttach();
			                               return self->GetMaterialCount();
		                               },
		                               "GetMaterial", [](UT::Renderer* self) -> UT::Material*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetMaterial();
		                               },
		                               "GetSharedMaterial", [](UT::Renderer* self) -> UT::Material*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetSharedMaterial();
		                               },
		                               "SetMaterial", [](UT::Renderer* self, UT::Material* material)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetMaterial(material);
		                               },
		                               "GetBounds", [](UT::Renderer* self) -> UT::Bounds
		                               {
			                               if (!self) return {};
			                               UR::ThreadAttach();
			                               return self->GetBounds();
		                               }
		);

		lua.new_usertype<UT::Sprite>("Sprite",
		                             sol::base_classes, sol::bases<UT::Object>(),
		                             "GetBounds", [](UT::Sprite* self) -> UT::Bounds
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->GetBounds();
		                             },
		                             "GetRect", [](UT::Sprite* self) -> UT::Rect
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->GetRect();
		                             },
		                             "GetBorder", [](UT::Sprite* self) -> UT::Vector4
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->GetBorder();
		                             },
		                             "GetTexture", [](UT::Sprite* self) -> UT::Texture2D*
		                             {
			                             if (!self) return nullptr;
			                             UR::ThreadAttach();
			                             return self->GetTexture();
		                             },
		                             "GetPixelsPerUnit", [](UT::Sprite* self) -> float
		                             {
			                             if (!self) return 0.f;
			                             UR::ThreadAttach();
			                             return self->GetPixelsPerUnit();
		                             },
		                             "GetPivot", [](UT::Sprite* self) -> UT::Vector2
		                             {
			                             if (!self) return {};
			                             UR::ThreadAttach();
			                             return self->GetPivot();
		                             },
		                             "GetPacked", [](UT::Sprite* self) -> bool
		                             {
			                             if (!self) return false;
			                             UR::ThreadAttach();
			                             return self->GetPacked();
		                             }
		);

		auto sprite_ns = unity["Sprite"].get_or_create<sol::table>();
		sprite_ns["Create"] = [](UT::Texture2D* texture, const UT::Rect& rect, const UT::Vector2& pivot,
		                         float pixelsPerUnit) -> UT::Sprite*
		{
			UR::ThreadAttach();
			return UT::Sprite::Create(texture, rect, pivot, pixelsPerUnit);
		};

		lua.new_usertype<UT::Shader>("Shader",
		                             sol::base_classes, sol::bases<UT::Object>(),
		                             "isSupported", [](UT::Shader* self) -> bool
		                             {
			                             if (!self) return false;
			                             UR::ThreadAttach();
			                             return self->isSupported();
		                             },
		                             "GetPropertyCount", [](UT::Shader* self) -> int
		                             {
			                             if (!self) return 0;
			                             UR::ThreadAttach();
			                             return self->GetPropertyCount();
		                             },
		                             "FindPropertyIndex", [](UT::Shader* self, const std::string& propertyName) -> int
		                             {
			                             if (!self) return -1;
			                             UR::ThreadAttach();
			                             return self->FindPropertyIndex(propertyName);
		                             },
		                             "GetPropertyName", [](UT::Shader* self, int propertyIndex) -> std::string
		                             {
			                             if (!self) return "";
			                             UR::ThreadAttach();
			                             if (auto* s = self->GetPropertyName(propertyIndex)) return s->ToString();
			                             return "";
		                             }
		);

		auto shader_ns = unity["Shader"].get_or_create<sol::table>();
		shader_ns["Find"] = [](const std::string& name) -> UT::Shader*
		{
			UR::ThreadAttach();
			return UT::Shader::Find(name);
		};
		shader_ns["EnableKeyword"] = [](const std::string& keyword)
		{
			UR::ThreadAttach();
			UT::Shader::EnableKeyword(keyword);
		};
		shader_ns["DisableKeyword"] = [](const std::string& keyword)
		{
			UR::ThreadAttach();
			UT::Shader::DisableKeyword(keyword);
		};
		shader_ns["PropertyToID"] = [](const std::string& name) -> int
		{
			UR::ThreadAttach();
			return UT::Shader::PropertyToID(name);
		};
		shader_ns["SetGlobalFloat"] = [](const std::string& name, float value)
		{
			UR::ThreadAttach();
			UT::Shader::SetGlobalFloat(name, value);
		};
		shader_ns["SetGlobalVector"] = [](const std::string& name, const UT::Vector4& value)
		{
			UR::ThreadAttach();
			UT::Shader::SetGlobalVector(name, value);
		};
		shader_ns["SetGlobalTexture"] = [](const std::string& name, UT::Texture* value)
		{
			UR::ThreadAttach();
			UT::Shader::SetGlobalTexture(name, value);
		};
		shader_ns["GetGlobalFloat"] = [](const std::string& name) -> float
		{
			UR::ThreadAttach();
			return UT::Shader::GetGlobalFloat(name);
		};

		lua.new_usertype<UT::Material>("Material",
		                               sol::base_classes, sol::bases<UT::Object>(),
		                               "GetShader", [](UT::Material* self) -> UT::Shader*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetShader();
		                               },
		                               "SetShader", [](UT::Material* self, UT::Shader* shader)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetShader(shader);
		                               },
		                               "SetTexture", [](UT::Material* self, const std::string& name, UT::Texture* value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetTexture(UT::String::New(name), value);
		                               },
		                               "SetTextureByID", [](UT::Material* self, int nameID, UT::Texture* value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetTexture(nameID, value);
		                               },
		                               "GetTexture", [](UT::Material* self, const std::string& name) -> UT::Texture*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetTexture(UT::String::New(name));
		                               },
		                               "GetTextureByID", [](UT::Material* self, int nameID) -> UT::Texture*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetTexture(nameID);
		                               },
		                               "SetColor", [](UT::Material* self, const std::string& name,
		                                              const UT::Color& value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetColor(UT::String::New(name), value);
		                               },
		                               "SetColorByID", [](UT::Material* self, int nameID, const UT::Color& value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetColor(nameID, value);
		                               },
		                               "SetFloat", [](UT::Material* self, const std::string& name, float value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetFloat(UT::String::New(name), value);
		                               },
		                               "SetFloatByID", [](UT::Material* self, int nameID, float value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetFloat(nameID, value);
		                               },
		                               "SetInt", [](UT::Material* self, const std::string& name, int value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetInt(UT::String::New(name), value);
		                               },
		                               "SetIntByID", [](UT::Material* self, int nameID, int value)
		                               {
			                               if (!self) return;
			                               UR::ThreadAttach();
			                               self->SetInt(nameID, value);
		                               }
		);

		lua.new_usertype<UT::Texture>("Texture",
		                              sol::base_classes, sol::bases<UT::Object>(),
		                              "GetWidth", [](UT::Texture* self) -> int
		                              {
			                              if (!self) return 0;
			                              UR::ThreadAttach();
			                              return self->GetWidth();
		                              },
		                              "GetHeight", [](UT::Texture* self) -> int
		                              {
			                              if (!self) return 0;
			                              UR::ThreadAttach();
			                              return self->GetHeight();
		                              },
		                              "GetMipmapCount", [](UT::Texture* self) -> int
		                              {
			                              if (!self) return 0;
			                              UR::ThreadAttach();
			                              return self->GetMipmapCount();
		                              },
		                              "GetIsReadable", [](UT::Texture* self) -> bool
		                              {
			                              if (!self) return false;
			                              UR::ThreadAttach();
			                              return self->GetIsReadable();
		                              },
		                              "GetWrapMode", [](UT::Texture* self) -> int
		                              {
			                              if (!self) return 0;
			                              UR::ThreadAttach();
			                              return self->GetWrapMode();
		                              },
		                              "SetWrapMode", [](UT::Texture* self, int wrapMode)
		                              {
			                              if (!self) return;
			                              UR::ThreadAttach();
			                              self->SetWrapMode(wrapMode);
		                              },
		                              "GetFilterMode", [](UT::Texture* self) -> int
		                              {
			                              if (!self) return 0;
			                              UR::ThreadAttach();
			                              return self->GetFilterMode();
		                              },
		                              "SetFilterMode", [](UT::Texture* self, int filterMode)
		                              {
			                              if (!self) return;
			                              UR::ThreadAttach();
			                              self->SetFilterMode(filterMode);
		                              }
		);

		lua.new_usertype<UT::Texture2D>("Texture2D",
		                                sol::base_classes, sol::bases<UT::Texture, UT::Object>(),
		                                "LoadRawTextureData", [](UT::Texture2D* self, void* data)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->LoadRawTextureData(static_cast<UT::Array<UT::Byte>*>(data));
		                                },
		                                "LoadImage", [](UT::Texture2D* self, void* data)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->LoadImage(static_cast<UT::Array<UT::Byte>*>(data));
		                                },
		                                "Apply", [](UT::Texture2D* self, sol::optional<bool> updateMipmaps,
		                                            sol::optional<bool> makeNoLongerReadable)
		                                {
			                                if (!self) return;
			                                UR::ThreadAttach();
			                                self->Apply(updateMipmaps.value_or(true),
			                                            makeNoLongerReadable.value_or(false));
		                                }
		);

		auto tex2d_ns = unity["Texture2D"].get_or_create<sol::table>();
		tex2d_ns["New"] = [](int width, int height, sol::optional<int> textureFormat,
		                     sol::optional<bool> mipChain) -> UT::Texture2D*
		{
			UR::ThreadAttach();
			if (textureFormat && mipChain)
				return UT::Texture2D::New(width, height, *textureFormat, *mipChain);
			return UT::Texture2D::New(width, height);
		};
		tex2d_ns["GetWhiteTexture"] = []() -> UT::Texture2D*
		{
			UR::ThreadAttach();
			return UT::Texture2D::GetWhiteTexture();
		};
		tex2d_ns["GetBlackTexture"] = []() -> UT::Texture2D*
		{
			UR::ThreadAttach();
			return UT::Texture2D::GetBlackTexture();
		};

		auto humanBodyBones = lua.create_table();
		humanBodyBones["Hips"] = UT::Animator::HumanBodyBones::Hips;
		humanBodyBones["LeftUpperLeg"] = UT::Animator::HumanBodyBones::LeftUpperLeg;
		humanBodyBones["RightUpperLeg"] = UT::Animator::HumanBodyBones::RightUpperLeg;
		humanBodyBones["LeftLowerLeg"] = UT::Animator::HumanBodyBones::LeftLowerLeg;
		humanBodyBones["RightLowerLeg"] = UT::Animator::HumanBodyBones::RightLowerLeg;
		humanBodyBones["LeftFoot"] = UT::Animator::HumanBodyBones::LeftFoot;
		humanBodyBones["RightFoot"] = UT::Animator::HumanBodyBones::RightFoot;
		humanBodyBones["Spine"] = UT::Animator::HumanBodyBones::Spine;
		humanBodyBones["Chest"] = UT::Animator::HumanBodyBones::Chest;
		humanBodyBones["UpperChest"] = UT::Animator::HumanBodyBones::UpperChest;
		humanBodyBones["Neck"] = UT::Animator::HumanBodyBones::Neck;
		humanBodyBones["Head"] = UT::Animator::HumanBodyBones::Head;
		humanBodyBones["LeftShoulder"] = UT::Animator::HumanBodyBones::LeftShoulder;
		humanBodyBones["RightShoulder"] = UT::Animator::HumanBodyBones::RightShoulder;
		humanBodyBones["LeftUpperArm"] = UT::Animator::HumanBodyBones::LeftUpperArm;
		humanBodyBones["RightUpperArm"] = UT::Animator::HumanBodyBones::RightUpperArm;
		humanBodyBones["LeftLowerArm"] = UT::Animator::HumanBodyBones::LeftLowerArm;
		humanBodyBones["RightLowerArm"] = UT::Animator::HumanBodyBones::RightLowerArm;
		humanBodyBones["LeftHand"] = UT::Animator::HumanBodyBones::LeftHand;
		humanBodyBones["RightHand"] = UT::Animator::HumanBodyBones::RightHand;
		humanBodyBones["LeftToes"] = UT::Animator::HumanBodyBones::LeftToes;
		humanBodyBones["RightToes"] = UT::Animator::HumanBodyBones::RightToes;
		humanBodyBones["LeftEye"] = UT::Animator::HumanBodyBones::LeftEye;
		humanBodyBones["RightEye"] = UT::Animator::HumanBodyBones::RightEye;
		humanBodyBones["Jaw"] = UT::Animator::HumanBodyBones::Jaw;
		humanBodyBones["LeftThumbProximal"] = UT::Animator::HumanBodyBones::LeftThumbProximal;
		humanBodyBones["LeftThumbIntermediate"] = UT::Animator::HumanBodyBones::LeftThumbIntermediate;
		humanBodyBones["LeftThumbDistal"] = UT::Animator::HumanBodyBones::LeftThumbDistal;
		humanBodyBones["LeftIndexProximal"] = UT::Animator::HumanBodyBones::LeftIndexProximal;
		humanBodyBones["LeftIndexIntermediate"] = UT::Animator::HumanBodyBones::LeftIndexIntermediate;
		humanBodyBones["LeftIndexDistal"] = UT::Animator::HumanBodyBones::LeftIndexDistal;
		humanBodyBones["LeftMiddleProximal"] = UT::Animator::HumanBodyBones::LeftMiddleProximal;
		humanBodyBones["LeftMiddleIntermediate"] = UT::Animator::HumanBodyBones::LeftMiddleIntermediate;
		humanBodyBones["LeftMiddleDistal"] = UT::Animator::HumanBodyBones::LeftMiddleDistal;
		humanBodyBones["LeftRingProximal"] = UT::Animator::HumanBodyBones::LeftRingProximal;
		humanBodyBones["LeftRingIntermediate"] = UT::Animator::HumanBodyBones::LeftRingIntermediate;
		humanBodyBones["LeftRingDistal"] = UT::Animator::HumanBodyBones::LeftRingDistal;
		humanBodyBones["LeftLittleProximal"] = UT::Animator::HumanBodyBones::LeftLittleProximal;
		humanBodyBones["LeftLittleIntermediate"] = UT::Animator::HumanBodyBones::LeftLittleIntermediate;
		humanBodyBones["LeftLittleDistal"] = UT::Animator::HumanBodyBones::LeftLittleDistal;
		humanBodyBones["RightThumbProximal"] = UT::Animator::HumanBodyBones::RightThumbProximal;
		humanBodyBones["RightThumbIntermediate"] = UT::Animator::HumanBodyBones::RightThumbIntermediate;
		humanBodyBones["RightThumbDistal"] = UT::Animator::HumanBodyBones::RightThumbDistal;
		humanBodyBones["RightIndexProximal"] = UT::Animator::HumanBodyBones::RightIndexProximal;
		humanBodyBones["RightIndexIntermediate"] = UT::Animator::HumanBodyBones::RightIndexIntermediate;
		humanBodyBones["RightIndexDistal"] = UT::Animator::HumanBodyBones::RightIndexDistal;
		humanBodyBones["RightMiddleProximal"] = UT::Animator::HumanBodyBones::RightMiddleProximal;
		humanBodyBones["RightMiddleIntermediate"] = UT::Animator::HumanBodyBones::RightMiddleIntermediate;
		humanBodyBones["RightMiddleDistal"] = UT::Animator::HumanBodyBones::RightMiddleDistal;
		humanBodyBones["RightRingProximal"] = UT::Animator::HumanBodyBones::RightRingProximal;
		humanBodyBones["RightRingIntermediate"] = UT::Animator::HumanBodyBones::RightRingIntermediate;
		humanBodyBones["RightRingDistal"] = UT::Animator::HumanBodyBones::RightRingDistal;
		humanBodyBones["RightLittleProximal"] = UT::Animator::HumanBodyBones::RightLittleProximal;
		humanBodyBones["RightLittleIntermediate"] = UT::Animator::HumanBodyBones::RightLittleIntermediate;
		humanBodyBones["RightLittleDistal"] = UT::Animator::HumanBodyBones::RightLittleDistal;
		humanBodyBones["LastBone"] = UT::Animator::HumanBodyBones::LastBone;
		unity["HumanBodyBones"] = humanBodyBones;

		lua.new_usertype<UT::Animator>("Animator",
		                               sol::base_classes,
		                               sol::bases<UT::Behaviour, UT::Component, UT::UnityObject, UT::Object>(),
		                               "GetBoneTransform",
		                               [](UT::Animator* self, UT::Animator::HumanBodyBones boneId) -> UT::Transform*
		                               {
			                               if (!self) return nullptr;
			                               UR::ThreadAttach();
			                               return self->GetBoneTransform(boneId);
		                               }
		);

		lua.new_usertype<UT::LayerMask>("LayerMask",
		                                "m_Mask", &UT::LayerMask::m_Mask
		);

		auto layer_ns = unity["LayerMask"].get_or_create<sol::table>();
		layer_ns["NameToLayer"] = [](const std::string& name) -> int
		{
			UR::ThreadAttach();
			return UT::LayerMask::NameToLayer(name);
		};
		layer_ns["LayerToName"] = [](int layer) -> std::string
		{
			UR::ThreadAttach();
			if (auto* s = UT::LayerMask::LayerToName(layer)) return s->ToString();
			return "";
		};
	}

	static void RegisterStaticClasses(sol::state& lua)
	{
		auto unity = lua["Unity"].get_or_create<sol::table>();

		auto obj_ns = unity["Object"].get_or_create<sol::table>();
		obj_ns["FindObjectsOfType"] = [](const std::string& className) -> sol::as_table_t<std::vector<void*>>
		{
			UR::ThreadAttach();
			auto* klass = UR::Get("UnityEngine.CoreModule.dll")->Get(className);
			if (!klass) return {};
			return klass->FindObjectsOfType<void*>();
		};
		obj_ns["FindObjectsByType"] = [](const std::string& className) -> sol::as_table_t<std::vector<void*>>
		{
			UR::ThreadAttach();
			auto* klass = UR::Get("UnityEngine.CoreModule.dll")->Get(className);
			if (!klass) return {};
			return klass->FindObjectsByType<void*>();
		};
		obj_ns["Instantiate"] = [](UT::UnityObject* original) -> UT::UnityObject*
		{
			if (!original) return nullptr;
			UR::ThreadAttach();
			return UT::UnityObject::Instantiate(original);
		};
		obj_ns["Destroy"] = [](UT::UnityObject* obj)
		{
			if (!obj) return;
			UR::ThreadAttach();
			UT::UnityObject::Destroy(obj);
		};
		obj_ns["FindObjectFromInstanceID"] = [](int32_t instanceID) -> UT::UnityObject*
		{
			UR::ThreadAttach();
			return UT::UnityObject::FindObjectFromInstanceID(instanceID);
		};
		obj_ns["DontDestroyOnLoad"] = [](UT::UnityObject* target)
		{
			if (!target) return;
			UR::ThreadAttach();
			UT::UnityObject::DontDestroyOnLoad(target);
		};

		auto phys_ns = unity["Physics"].get_or_create<sol::table>();
		phys_ns["Raycast"] = sol::overload(
			[](const UT::Vector3& origin, const UT::Vector3& direction, float maxDistance) -> bool
			{
				UR::ThreadAttach();
				return UT::Physics::Raycast(origin, direction, maxDistance);
			},
			[&lua](const UT::Vector3& origin, const UT::Vector3& direction, float maxDistance) -> sol::object
			{
				UR::ThreadAttach();
				UT::RaycastHit hit{};
				if (UT::Ray ray{origin, direction}; UT::Physics::Raycast(ray, &hit, maxDistance))
				{
					sol::table t = lua.create_table();
					t["point"] = hit.m_Point;
					t["normal"] = hit.m_Normal;
					t["distance"] = hit.m_Distance;
					t["collider"] = hit.m_Collider;
					return t;
				}
				return sol::nil;
			}
		);
		phys_ns["Linecast"] = [](const UT::Vector3& start, const UT::Vector3& end) -> bool
		{
			UR::ThreadAttach();
			return UT::Physics::Linecast(start, end);
		};
		phys_ns["IgnoreCollision"] = [](UT::Collider* a, UT::Collider* b)
		{
			if (!a || !b) return;
			UR::ThreadAttach();
			UT::Physics::IgnoreCollision(a, b);
		};

		auto time_ns = unity["Time"].get_or_create<sol::table>();
		time_ns["GetTime"] = []() -> float
		{
			UR::ThreadAttach();
			return UT::Time::GetTime();
		};
		time_ns["GetDeltaTime"] = []() -> float
		{
			UR::ThreadAttach();
			return UT::Time::GetDeltaTime();
		};
		time_ns["GetFixedDeltaTime"] = []() -> float
		{
			UR::ThreadAttach();
			return UT::Time::GetFixedDeltaTime();
		};
		time_ns["GetTimeScale"] = []() -> float
		{
			UR::ThreadAttach();
			return UT::Time::GetTimeScale();
		};
		time_ns["SetTimeScale"] = [](float v)
		{
			UR::ThreadAttach();
			UT::Time::SetTimeScale(v);
		};

		auto screen_ns = unity["Screen"].get_or_create<sol::table>();
		screen_ns["GetWidth"] = []() -> int
		{
			UR::ThreadAttach();
			return UT::Screen::get_width();
		};
		screen_ns["GetHeight"] = []() -> int
		{
			UR::ThreadAttach();
			return UT::Screen::get_height();
		};

		auto input_ns = unity["Input"].get_or_create<sol::table>();
		input_ns["GetKey"] = [](int key) -> bool
		{
			UR::ThreadAttach();
			return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
		};
		input_ns["GetKeyDown"] = [](int key) -> bool
		{
			UR::ThreadAttach();
			return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
		};
		input_ns["GetMouseButton"] = [](int button) -> bool
		{
			UR::ThreadAttach();
			return ImGui::IsMouseDown(button);
		};
		input_ns["GetMouseButtonDown"] = [](int button) -> bool
		{
			UR::ThreadAttach();
			return ImGui::IsMouseClicked(button);
		};
		input_ns["GetMousePos"] = []() -> UT::Vector2
		{
			ImVec2 pos = ImGui::GetMousePos();
			return UT::Vector2(pos.x, pos.y);
		};
		input_ns["GetMouseDelta"] = []() -> UT::Vector2
		{
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			return UT::Vector2(delta.x, delta.y);
		};
		input_ns["GetMouseWheel"] = []() -> float { return ImGui::GetIO().MouseWheel; };
	}

	static void RegisterReflection(sol::state& lua)
	{
		auto unity = lua["Unity"].get_or_create<sol::table>();

		lua.new_usertype<UR::Field>("Field",
		                            "name", &UR::Field::name,
		                            "offset", &UR::Field::offset,
		                            "static", &UR::Field::static_field,
		                            "typeName", [](const UR::Field* self) -> std::string
		                            {
			                            if (!self || !self->type) return "";
			                            return self->type->name;
		                            }
		);

		lua.new_usertype<UR::Method>("Method",
		                             "name", &UR::Method::name,
		                             "static", &UR::Method::static_function,
		                             "returnType", [](const UR::Method* self) -> std::string
		                             {
			                             if (!self || !self->return_type) return "";
			                             return self->return_type->name;
		                             },
		                             "argCount", [](const UR::Method* self) -> int
		                             {
			                             if (!self) return 0;
			                             return static_cast<int>(self->m_args.size());
		                             }
		);

		lua.new_usertype<UR::Class>("Class",
		                            "name", &UR::Class::m_name,
		                            "namespaze", &UR::Class::namespaze,
		                            "parent", &UR::Class::parent,
		                            "GetName", &UR::Class::m_name,
		                            "GetNamespace", &UR::Class::namespaze,
		                            "GetParent", &UR::Class::parent,
		                            "GetFields", [](const UR::Class* klass) -> sol::as_table_t<std::vector<UR::Field*>>
		                            {
			                            if (!klass) return sol::as_table(std::vector<UR::Field*>{});
			                            std::vector<UR::Field*> result;
			                            for (auto& f : klass->fields) result.push_back(f.get());
			                            return sol::as_table(result);
		                            },
		                            "GetMethods",
		                            [](const UR::Class* klass) -> sol::as_table_t<std::vector<UR::Method*>>
		                            {
			                            if (!klass) return sol::as_table(std::vector<UR::Method*>{});
			                            std::vector<UR::Method*> result;
			                            for (auto& m : klass->methods) result.push_back(m.get());
			                            return sol::as_table(result);
		                            },
		                            "FindObjectsOfType",
		                            [](UR::Class* klass) -> sol::as_table_t<std::vector<UT::Component*>>
		                            {
			                            if (!klass) return sol::as_table(std::vector<UT::Component*>{});
			                            UR::ThreadAttach();
			                            return sol::as_table(klass->FindObjectsOfType<UT::Component*>());
		                            },
		                            "FindObjectsByType",
		                            [](UR::Class* klass) -> sol::as_table_t<std::vector<UT::Component*>>
		                            {
			                            if (!klass) return sol::as_table(std::vector<UT::Component*>{});
			                            UR::ThreadAttach();
			                            return sol::as_table(klass->FindObjectsByType<UT::Component*>());
		                            },
		                            "New", [](UR::Class* klass) -> void*
		                            {
			                            if (!klass) return nullptr;
			                            UR::ThreadAttach();
			                            return klass->New<void*>();
		                            },
		                            "GetFieldValue",
		                            [&lua](UR::Class* klass, void* obj, const std::string& fieldName) -> sol::object
		                            {
			                            if (!klass) return sol::nil;
			                            UR::ThreadAttach();
			                            auto* field = klass->Get<UR::Field>(fieldName);
			                            if (!field) return sol::nil;

			                            std::string typeName = field->type->name;
			                            if (typeName == "System.Single" || typeName == "float")
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<float>(obj, fieldName));
			                            if (typeName == "System.Int32" || typeName == "int")
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<int>(obj, fieldName));
			                            if (typeName == "System.Boolean" || typeName == "bool")
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<bool>(obj, fieldName));
			                            if (typeName == "System.String")
			                            {
				                            if (auto* s = klass->GetValue<UT::String*>(obj, fieldName)) return
					                            sol::make_object(lua.lua_state(), s->ToString());
				                            return sol::make_object(lua.lua_state(), "");
			                            }
			                            if (typeName.find("UnityEngine.Vector3") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<UT::Vector3>(obj, fieldName));
			                            if (typeName.find("UnityEngine.Vector2") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<UT::Vector2>(obj, fieldName));
			                            if (typeName.find("UnityEngine.Quaternion") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<UT::Quaternion>(obj, fieldName));
			                            if (typeName.find("UnityEngine.Color") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    klass->GetValue<UT::Color>(obj, fieldName));

			                            return sol::make_object(lua.lua_state(),
			                                                    klass->GetValue<void*>(obj, fieldName));
		                            },
		                            "SetFieldValue",
		                            [](UR::Class* klass, void* obj, const std::string& fieldName,
		                               const sol::object& value)
		                            {
			                            if (!klass) return;
			                            UR::ThreadAttach();
			                            auto* field = klass->Get<UR::Field>(fieldName);
			                            if (!field) return;

			                            if (std::string typeName = field->type->name; typeName == "System.Single" ||
				                            typeName == "float")
				                            klass->SetValue(obj, fieldName, value.as<float>());
			                            else if (typeName == "System.Int32" || typeName == "int")
				                            klass->SetValue(obj, fieldName, value.as<int>());
			                            else if (typeName == "System.Boolean" || typeName == "bool")
				                            klass->SetValue(obj, fieldName, value.as<bool>());
			                            else if (typeName.find("UnityEngine.Vector3") != std::string::npos)
				                            klass->SetValue(obj, fieldName, value.as<UT::Vector3>());
			                            else if (typeName.find("UnityEngine.Vector2") != std::string::npos)
				                            klass->SetValue(obj, fieldName, value.as<UT::Vector2>());
			                            else if (typeName.find("UnityEngine.Quaternion") != std::string::npos)
				                            klass->SetValue(obj, fieldName, value.as<UT::Quaternion>());
			                            else if (typeName.find("UnityEngine.Color") != std::string::npos)
				                            klass->SetValue(obj, fieldName, value.as<UT::Color>());
		                            },
		                            "InvokeMethod",
		                            [&lua](UR::Class* klass, void* obj, const std::string& methodName,
		                                   sol::variadic_args) -> sol::object
		                            {
			                            if (!klass) return sol::nil;
			                            UR::ThreadAttach();
			                            auto* method = klass->Get<UR::Method>(methodName);
			                            if (!method) return sol::nil;

			                            std::string retType = method->return_type->name;
			                            auto result = method->RuntimeInvoke<void*>(obj);

			                            if (retType == "System.Void" || retType == "void")
				                            return sol::nil;
			                            if (retType == "System.Single" || retType == "float")
				                            return sol::make_object(lua.lua_state(), *static_cast<float*>(result));
			                            if (retType == "System.Int32" || retType == "int")
				                            return sol::make_object(lua.lua_state(), *static_cast<int*>(result));
			                            if (retType == "System.Boolean" || retType == "bool")
				                            return sol::make_object(lua.lua_state(), *static_cast<bool*>(result));
			                            if (retType == "System.String")
			                            {
				                            if (auto* s = static_cast<UT::String*>(result)) return sol::make_object(
					                            lua.lua_state(), s->ToString());
				                            return sol::make_object(lua.lua_state(), "");
			                            }
			                            if (retType.find("UnityEngine.Vector3") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    *static_cast<UT::Vector3*>(result));
			                            if (retType.find("UnityEngine.Vector2") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    *static_cast<UT::Vector2*>(result));
			                            if (retType.find("UnityEngine.Quaternion") != std::string::npos)
				                            return sol::make_object(lua.lua_state(),
				                                                    *static_cast<UT::Quaternion*>(result));

			                            return sol::make_object(lua.lua_state(), result);
		                            }
		);

		lua.new_usertype<UR::Assembly>("Assembly",
		                               "name", &UR::Assembly::name,
		                               "file", &UR::Assembly::file
		);

		auto resolve = unity["Resolve"].get_or_create<sol::table>();
		resolve["GetClass"] = [](const std::string& assembly, const std::string& className,
		                         const std::string& namespaze) -> UR::Class*
		{
			UR::ThreadAttach();
			auto* as = UR::Get(assembly);
			if (!as) return nullptr;
			return as->Get(className, namespaze);
		};
		resolve["GetAssemblies"] = []() -> sol::as_table_t<std::vector<UR::Assembly*>>
		{
			UR::ThreadAttach();
			std::vector<UR::Assembly*> result;
			for (auto& as : UR::assembly) result.push_back(as.get());
			return sol::as_table(result);
		};
	}

	static void RegisterImGui(sol::state& lua)
	{
		auto imgui = lua["imgui"].get_or_create<sol::table>();

		imgui["Begin"] = [](const std::string& name, sol::optional<bool> open, sol::optional<int> flags)
		{
			bool* pOpen = open ? &open.value() : nullptr;
			ImGui::Begin(name.c_str(), pOpen, flags.value_or(0));
		};
		imgui["End"] = &ImGui::End;
		imgui["Text"] = [](const std::string& text) { ImGui::Text("%s", text.c_str()); };
		imgui["TextColored"] = [](const UT::Color& col, const std::string& text)
		{
			ImGui::TextColored(ImVec4(col.r, col.g, col.b, col.a), "%s", text.c_str());
		};
		imgui["Button"] = [](const std::string& label, sol::optional<UT::Vector2> size) -> bool
		{
			ImVec2 sz = size ? ImVec2(size->x, size->y) : ImVec2(0, 0);
			return ImGui::Button(label.c_str(), sz);
		};
		imgui["SmallButton"] = [](const std::string& label) -> bool { return ImGui::SmallButton(label.c_str()); };
		imgui["Checkbox"] = [](const std::string& label, bool v) -> bool
		{
			ImGui::Checkbox(label.c_str(), &v);
			return v;
		};
		imgui["SliderFloat"] = [](const std::string& label, float v, float min, float max) -> float
		{
			ImGui::SliderFloat(label.c_str(), &v, min, max);
			return v;
		};
		imgui["SliderInt"] = [](const std::string& label, int v, int min, int max) -> int
		{
			ImGui::SliderInt(label.c_str(), &v, min, max);
			return v;
		};
		imgui["InputText"] = [](const std::string& label, std::string text, sol::optional<int> flags) -> std::string
		{
			char buffer[256] = {};
			strncpy_s(buffer, text.c_str(), sizeof(buffer) - 1);
			if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer), flags.value_or(0)))
				return std::string(buffer);
			return text;
		};
		imgui["InputFloat"] = [](const std::string& label, float v) -> float
		{
			ImGui::InputFloat(label.c_str(), &v);
			return v;
		};
		imgui["InputFloat3"] = [](const std::string& label, const UT::Vector3& v) -> UT::Vector3
		{
			float vec[3] = {v.x, v.y, v.z};
			ImGui::InputFloat3(label.c_str(), vec);
			return UT::Vector3(vec[0], vec[1], vec[2]);
		};
		imgui["Separator"] = &ImGui::Separator;
		imgui["Spacing"] = &ImGui::Spacing;
		imgui["SameLine"] = [](sol::optional<float> offset) { ImGui::SameLine(offset.value_or(0.0f)); };
		imgui["SetNextWindowSize"] = [](const UT::Vector2& size, sol::optional<int> cond)
		{
			ImGui::SetNextWindowSize(ImVec2(size.x, size.y), cond.value_or(ImGuiCond_FirstUseEver));
		};
		imgui["SetNextWindowPos"] = [](const UT::Vector2& pos, sol::optional<int> cond)
		{
			ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), cond.value_or(ImGuiCond_FirstUseEver));
		};
		imgui["IsKeyDown"] = [](int key) -> bool { return ImGui::IsKeyDown(static_cast<ImGuiKey>(key)); };
		imgui["IsMouseDown"] = [](int button) -> bool { return ImGui::IsMouseDown(button); };
		imgui["GetMousePos"] = []() -> UT::Vector2
		{
			ImVec2 pos = ImGui::GetMousePos();
			return UT::Vector2(pos.x, pos.y);
		};
		imgui["GetIO"] = []() -> sol::table
		{
			return sol::table();
		};

		auto drawlist_type = lua.new_usertype<ImDrawList>("DrawList",
		                                                  "AddLine", [](ImDrawList* self, const UT::Vector2& a,
		                                                                const UT::Vector2& b, uint32_t col,
		                                                                sol::optional<float> thickness)
		                                                  {
			                                                  self->AddLine(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), col,
				                                                  thickness.value_or(1.0f));
		                                                  },
		                                                  "AddRect", [](ImDrawList* self, const UT::Vector2& a,
		                                                                const UT::Vector2& b, uint32_t col,
		                                                                sol::optional<float> rounding,
		                                                                sol::optional<int> flags,
		                                                                sol::optional<float> thickness)
		                                                  {
			                                                  self->AddRect(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), col,
				                                                  rounding.value_or(0.0f), thickness.value_or(1.0f),
				                                                  flags.value_or(0));
		                                                  },
		                                                  "AddRectFilled",
		                                                  [](ImDrawList* self, const UT::Vector2& a,
		                                                     const UT::Vector2& b, uint32_t col,
		                                                     sol::optional<float> rounding, sol::optional<int> flags)
		                                                  {
			                                                  self->AddRectFilled(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), col,
				                                                  rounding.value_or(0.0f), flags.value_or(0));
		                                                  },
		                                                  "AddCircle",
		                                                  [](ImDrawList* self, const UT::Vector2& center, float radius,
		                                                     uint32_t col, sol::optional<int> segments,
		                                                     sol::optional<float> thickness)
		                                                  {
			                                                  self->AddCircle(
				                                                  ImVec2(center.x, center.y), radius, col,
				                                                  segments.value_or(0), thickness.value_or(1.0f));
		                                                  },
		                                                  "AddCircleFilled",
		                                                  [](ImDrawList* self, const UT::Vector2& center, float radius,
		                                                     uint32_t col, sol::optional<int> segments)
		                                                  {
			                                                  self->AddCircleFilled(
				                                                  ImVec2(center.x, center.y), radius, col,
				                                                  segments.value_or(0));
		                                                  },
		                                                  "AddText", [](ImDrawList* self, const UT::Vector2& pos,
		                                                                uint32_t col, const std::string& text)
		                                                  {
			                                                  self->AddText(ImVec2(pos.x, pos.y), col, text.c_str());
		                                                  },
		                                                  "AddTriangle",
		                                                  [](ImDrawList* self, const UT::Vector2& a,
		                                                     const UT::Vector2& b, const UT::Vector2& c, uint32_t col,
		                                                     sol::optional<float> thickness)
		                                                  {
			                                                  self->AddTriangle(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), ImVec2(c.x, c.y),
				                                                  col, thickness.value_or(1.0f));
		                                                  },
		                                                  "AddTriangleFilled",
		                                                  [](ImDrawList* self, const UT::Vector2& a,
		                                                     const UT::Vector2& b, const UT::Vector2& c, uint32_t col)
		                                                  {
			                                                  self->AddTriangleFilled(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), ImVec2(c.x, c.y),
				                                                  col);
		                                                  },
		                                                  "PushClipRect",
		                                                  [](ImDrawList* self, const UT::Vector2& a,
		                                                     const UT::Vector2& b, bool intersect)
		                                                  {
			                                                  self->PushClipRect(
				                                                  ImVec2(a.x, a.y), ImVec2(b.x, b.y), intersect);
		                                                  },
		                                                  "PopClipRect", &ImDrawList::PopClipRect
		);

		imgui["Combo"] = [](const std::string& label, int currentItem, const std::string& items) -> int
		{
			return ImGui::Combo(label.c_str(), &currentItem, items.c_str()) ? currentItem : -1;
		};
		imgui["ListBox"] = [](const std::string& label, int currentItem, const std::vector<std::string>& items) -> int
		{
			std::vector<const char*> cstrs;
			for (const auto& s : items) cstrs.push_back(s.c_str());
			return ImGui::ListBox(label.c_str(), &currentItem, cstrs.data(), static_cast<int>(cstrs.size()))
				       ? currentItem
				       : -1;
		};
		imgui["DragFloat"] = [](const std::string& label, float v, float v_speed, float v_min, float v_max) -> float
		{
			ImGui::DragFloat(label.c_str(), &v, v_speed, v_min, v_max);
			return v;
		};
		imgui["DragInt"] = [](const std::string& label, int v, float v_speed, int v_min, int v_max) -> int
		{
			ImGui::DragInt(label.c_str(), &v, v_speed, v_min, v_max);
			return v;
		};
		imgui["ColorEdit3"] = [](const std::string& label, const UT::Color& c) -> UT::Color
		{
			float col[3] = {c.r, c.g, c.b};
			if (ImGui::ColorEdit3(label.c_str(), col)) return UT::Color(col[0], col[1], col[2], c.a);
			return c;
		};
		imgui["ColorEdit4"] = [](const std::string& label, const UT::Color& c) -> UT::Color
		{
			float col[4] = {c.r, c.g, c.b, c.a};
			if (ImGui::ColorEdit4(label.c_str(), col)) return UT::Color(col[0], col[1], col[2], col[3]);
			return c;
		};
		imgui["Selectable"] = [](const std::string& label, bool selected) -> bool
		{
			return ImGui::Selectable(label.c_str(), selected);
		};
		imgui["RadioButton"] = [](const std::string& label, bool active) -> bool
		{
			return ImGui::RadioButton(label.c_str(), active);
		};
		imgui["GetWindowDrawList"] = [] { return ImGui::GetWindowDrawList(); };
		imgui["GetBackgroundDrawList"] = [] { return ImGui::GetBackgroundDrawList(); };
		imgui["GetForegroundDrawList"] = [] { return ImGui::GetForegroundDrawList(); };
		imgui["PushStyleColor"] = [](int idx, const UT::Color& col)
		{
			ImGui::PushStyleColor(idx, ImVec4(col.r, col.g, col.b, col.a));
		};
		imgui["PopStyleColor"] = [](int count) { ImGui::PopStyleColor(count); };
		imgui["PushStyleVar"] = [](int idx, float val) { ImGui::PushStyleVar(idx, val); };
		imgui["PopStyleVar"] = [](int count) { ImGui::PopStyleVar(count); };
		imgui["GetWindowSize"] = []() -> UT::Vector2
		{
			ImVec2 s = ImGui::GetWindowSize();
			return UT::Vector2(s.x, s.y);
		};
		imgui["GetWindowPos"] = []() -> UT::Vector2
		{
			ImVec2 p = ImGui::GetWindowPos();
			return UT::Vector2(p.x, p.y);
		};
		imgui["GetContentRegionAvail"] = []() -> UT::Vector2
		{
			ImVec2 s = ImGui::GetContentRegionAvail();
			return UT::Vector2(s.x, s.y);
		};
		imgui["GetFrameHeight"] = []() -> float { return ImGui::GetFrameHeight(); };
		imgui["GetFrameHeightWithSpacing"] = []() -> float { return ImGui::GetFrameHeightWithSpacing(); };
		imgui["IsItemHovered"] = []() -> bool { return ImGui::IsItemHovered(); };
		imgui["IsItemClicked"] = [](int mouseButton) -> bool { return ImGui::IsItemClicked(mouseButton); };
		imgui["SetTooltip"] = [](const std::string& text) { ImGui::SetTooltip("%s", text.c_str()); };
		imgui["BeginTooltip"] = &ImGui::BeginTooltip;
		imgui["EndTooltip"] = &ImGui::EndTooltip;
		imgui["BeginChild"] = [](const std::string& id, const UT::Vector2& size, bool border, int flags)
		{
			ImGui::BeginChild(id.c_str(), ImVec2(size.x, size.y), border, flags);
		};
		imgui["EndChild"] = &ImGui::EndChild;
		imgui["CollapsingHeader"] = [](const std::string& label, bool open) -> bool
		{
			return ImGui::CollapsingHeader(label.c_str(), &open);
		};
		imgui["TreeNode"] = [](const std::string& label) -> bool { return ImGui::TreeNode(label.c_str()); };
		imgui["TreePop"] = &ImGui::TreePop;
		imgui["Indent"] = &ImGui::Indent;
		imgui["Unindent"] = &ImGui::Unindent;
		imgui["BulletText"] = [](const std::string& text) { ImGui::BulletText("%s", text.c_str()); };
		imgui["Columns"] = [](int count, const std::string& id, bool border)
		{
			ImGui::Columns(count, id.c_str(), border);
		};
		imgui["NextColumn"] = &ImGui::NextColumn;
		imgui["SetColumnWidth"] = [](int index, float width) { ImGui::SetColumnWidth(index, width); };
	}

	static void RegisterLogging(sol::state& lua)
	{
		auto log = lua["log"].get_or_create<sol::table>();
		log["info"] = [](const std::string& msg)
		{
			DebugConsole::AddLog(msg, LogType::Log);
		};
		log["warn"] = [](const std::string& msg)
		{
			DebugConsole::AddLog(msg, LogType::Warning);
		};
		log["error"] = [](const std::string& msg)
		{
			DebugConsole::AddLog(msg, LogType::Error);
		};
		log["debug"] = [](const std::string& msg)
		{
			DebugConsole::AddLog(msg, LogType::Log);
		};
	}

	void RegisterAll(sol::state& lua)
	{
		lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table,
		                   sol::lib::math, sol::lib::io, sol::lib::os, sol::lib::debug);

		RegisterMathTypes(lua);
		RegisterUnityTypes(lua);
		RegisterStaticClasses(lua);
		RegisterReflection(lua);
		RegisterImGui(lua);
		RegisterLogging(lua);

		lua["print"] = [](sol::variadic_args args)
		{
			std::string result;
			for (auto arg : args)
			{
				if (!result.empty()) result += " ";
				result += arg.as<std::string>();
			}
			DebugConsole::AddLog(result, LogType::Log);
		};
	}
}
