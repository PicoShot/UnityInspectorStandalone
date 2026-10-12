#include "pch.h"
#include "helper/helper.h"
#include "inspector.h"

#define API(fn) (mono ? "mono_" fn : "il2cpp_" fn)

std::string Inspector::GetComponentTypeName(UT::Component* component) const
{
	if (!component) return "Unknown";

	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (void* klass = Helper::SafeGetObjectClass(component))
	{
		if (const char* className = UR::Invoke<const char*, void*>(API("class_get_name"), klass)) return {className};
	}

	return "Component";
}

std::vector<ComponentFieldInfo> Inspector::GetObjectFields(void* obj, void* klass) const
{
	std::vector<ComponentFieldInfo> fields;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (!klass && obj) klass = Helper::SafeGetObjectClass(obj);

	if (!klass) return fields;

	void* currentClass = klass;
	while (currentClass)
	{
		void* iter = nullptr;
		void* field;

		while ((field = UR::Invoke<void*, void*, void*>(API("class_get_fields"), currentClass, &iter)))
		{
			ComponentFieldInfo info;
			info.fieldHandle = field;
			info.classHandle = currentClass;

			const char* fieldName = UR::Invoke<const char*, void*>(API("field_get_name"), field);
			info.name = fieldName && fieldName[0] ? fieldName : "(unnamed)";

			info.offset = UR::Invoke<int, void*>(API("field_get_offset"), field);

			const int flags = UR::Invoke<int, void*>(API("field_get_flags"), field);
			info.isStatic = (flags & 0x10) != 0;

			if (!info.isStatic && UR::Invoke<bool, void*>(API("class_is_valuetype"), currentClass))
			{
				info.offset -= 0x10;
			}

			if (void* fieldType = UR::Invoke<void*, void*>(API("field_get_type"), field))
			{
				info.typeName = Helper::GetTypeName(fieldType);

				if (mono)
				{
					info.typeClassHandle = UR::Invoke<void*, void*>("mono_class_from_mono_type", fieldType);
					if (!info.typeClassHandle)
						info.typeClassHandle = UR::Invoke<void*, void*>("mono_type_get_class", fieldType);
				}
				else
				{
					info.typeClassHandle = UR::Invoke<void*, void*>("il2cpp_class_from_type", fieldType);
				}

				if (info.typeClassHandle)
				{
					info.isValueType = UR::Invoke<bool, void*>(API("class_is_valuetype"), info.typeClassHandle);
				}
			}
			else
			{
				info.typeName = "unknown";
			}

			info.editableType = DetermineEditableType(info.typeName, &info.enumTypeName);
			fields.push_back(info);
		}

		currentClass = UR::Invoke<void*, void*>(API("class_get_parent"), currentClass);
	}

	return fields;
}

std::vector<ComponentFieldInfo> Inspector::GetComponentFields(UT::Component* component) const
{
	return GetObjectFields(component, nullptr);
}

std::vector<ComponentPropertyInfo> Inspector::GetObjectProperties(void* obj, void* klass) const
{
	std::vector<ComponentPropertyInfo> properties;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (!klass && obj) klass = Helper::SafeGetObjectClass(obj);

	if (!klass) return properties;

	void* currentClass = klass;
	while (currentClass)
	{
		void* iter = nullptr;
		void* prop;

		while ((prop = UR::Invoke<void*, void*, void*>(API("class_get_properties"), currentClass, &iter)))
		{
			ComponentPropertyInfo info;

			const char* propName = UR::Invoke<const char*, void*>(API("property_get_name"), prop);
			info.name = propName && propName[0] ? propName : "(unnamed)";

			info.getterHandle = UR::Invoke<void*, void*>(API("property_get_get_method"), prop);
			info.setterHandle = UR::Invoke<void*, void*>(API("property_get_set_method"), prop);
			info.canRead = info.getterHandle != nullptr;
			info.canWrite = info.setterHandle != nullptr;

			if (mono && info.getterHandle)
			{
				if (void* sig = UR::Invoke<void*, void*>("mono_method_signature", info.getterHandle))
				{
					if (void* retType = UR::Invoke<void*, void*>("mono_signature_get_return_type", sig))
						info.typeName = Helper::GetTypeName(retType);
				}
			}
			else if (info.getterHandle)
			{
				if (void* retType = UR::Invoke<void*, void*>("il2cpp_method_get_return_type", info.getterHandle))
					info.typeName = Helper::GetTypeName(retType);
			}

			info.editableType = DetermineEditableType(info.typeName);
			properties.push_back(info);
		}

		currentClass = UR::Invoke<void*, void*>(API("class_get_parent"), currentClass);
	}

	return properties;
}

std::vector<ComponentPropertyInfo> Inspector::GetComponentProperties(UT::Component* component) const
{
	return GetObjectProperties(component, nullptr);
}

std::vector<ComponentMethodInfo> Inspector::GetObjectMethods(void* obj, void* klass) const
{
	std::vector<ComponentMethodInfo> methods;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (!klass && obj) klass = Helper::SafeGetObjectClass(obj);

	if (!klass) return methods;

	void* currentClass = klass;
	while (currentClass)
	{
		void* iter = nullptr;
		void* method;

		while ((method = UR::Invoke<void*, void*, void*>(API("class_get_methods"), currentClass, &iter)))
		{
			ComponentMethodInfo info;
			info.methodHandle = method;

			const char* methodName = UR::Invoke<const char*, void*>(API("method_get_name"), method);
			info.name = methodName && methodName[0] ? methodName : "(unnamed)";

			int fFlags = 0;
			info.flags = UR::Invoke<int, void*, int*>(API("method_get_flags"), method, &fFlags);
			info.isStatic = (info.flags & 0x10) != 0;
			info.isVirtual = (info.flags & 0x40) != 0;

			if (mono)
			{
				const auto signature = UR::Invoke<void*, void*>("mono_method_signature", method);
				if (!signature) continue;

				info.returnTypeName =
				    Helper::GetTypeName(UR::Invoke<void*, void*>("mono_signature_get_return_type", signature), "void");

				if (int paramCount = UR::Invoke<int, void*>("mono_signature_get_param_count", signature);
				    paramCount > 0)
				{
					std::vector<char*> paramNames(paramCount);
					UR::Invoke<void, void*, char**>("mono_method_get_param_names", method, paramNames.data());

					void* mIter = nullptr;
					void* mType;
					int paramIndex = 0;
					while (
					    ((mType = UR::Invoke<void*, void*, void*>("mono_signature_get_params", signature, &mIter))) &&
					    paramIndex < paramCount)
					{
						std::string pName =
						    (std::cmp_less(paramIndex, static_cast<int>(paramNames.size())) && paramNames[paramIndex])
						        ? paramNames[paramIndex]
						        : "arg" + std::to_string(paramIndex);
						std::string pType = Helper::GetTypeName(mType);
						info.parameters.emplace_back(pName, pType);
						info.parameterEditableTypes.push_back(DetermineEditableType(pType));
						paramIndex++;
					}
				}
			}
			else
			{
				info.returnTypeName =
				    Helper::GetTypeName(UR::Invoke<void*, void*>("il2cpp_method_get_return_type", method), "void");

				int paramCount = UR::Invoke<int, void*>("il2cpp_method_get_param_count", method);
				for (int i = 0; i < paramCount; i++)
				{
					const char* pName = UR::Invoke<const char*, void*, int>("il2cpp_method_get_param_name", method, i);
					void* pType = UR::Invoke<void*, void*, int>("il2cpp_method_get_param", method, i);
					std::string typeName = Helper::GetTypeName(pType);
					info.parameters.emplace_back(pName ? pName : "arg" + std::to_string(i), typeName);
					info.parameterEditableTypes.push_back(DetermineEditableType(typeName));
				}
			}

			methods.push_back(std::move(info));
		}

		currentClass = UR::Invoke<void*, void*>(API("class_get_parent"), currentClass);
	}

	return methods;
}

std::vector<ComponentMethodInfo> Inspector::GetComponentMethods(UT::Component* component) const
{
	return GetObjectMethods(component, nullptr);
}

#undef API

void* Inspector::InvokeMethod(void* instance, const ComponentMethodInfo& method,
                              const std::vector<std::string>& paramValues) const
{
	if (!method.methodHandle) return nullptr;

	auto [params, buffers] = Helper::BuildInvokeParams(paramValues, method.parameterEditableTypes);

	bool success = false;
	void* obj = method.isStatic ? nullptr : instance;
	void* result =
	    Helper::SafeInvokeMethod(obj, method.methodHandle, params.empty() ? nullptr : params.data(), success);

	return success ? result : nullptr;
}

std::string Inspector::BuildObjectPath(UT::Transform* transform) const
{
	if (!transform) return "";

	std::vector<std::string> pathParts;
	UT::Transform* current = transform;
	std::unordered_set<UT::Transform*> visited;

	while (current && pathParts.size() < 256 && visited.insert(current).second && Helper::SafeIsAlive(current))
	{
		UT::GameObject* go = nullptr;
		if (!Helper::SafeGetGameObject(current, go) || !go) break;

		if (!Helper::SafeIsAlive(go)) break;

		if (UT::String* name = nullptr; Helper::SafeGetName(go, name) && name)
		{
			pathParts.push_back(name->ToString());
		}

		UT::Transform* parent = nullptr;
		if (!Helper::SafeGetParent(current, parent)) break;
		current = parent;
	}

	std::string path;
	for (const auto& pathPart : std::ranges::reverse_view(pathParts))
	{
		if (!path.empty()) path += " / ";
		path += pathPart;
	}

	return path;
}

void Inspector::BuildHierarchyNode(HierarchyNode& node, UT::Transform* transform,
                                   std::unordered_set<UT::Transform*>& visited, const size_t depth, size_t& nodeBudget)
{
	if (depth >= 256 || nodeBudget == 0 || !transform || !visited.insert(transform).second) return;
	--nodeBudget;
	if (!Helper::SafeIsAlive(transform)) return;

	node.transform = transform;
	node.transformPin = Helper::GcPin(transform);

	UT::GameObject* go = nullptr;
	if (!Helper::SafeGetGameObject(transform, go) || !go)
	{
		node.gameObject = nullptr;
		return;
	}

	if (!Helper::SafeIsAlive(go))
	{
		node.gameObject = nullptr;
		return;
	}

	node.gameObject = go;
	node.gameObjectPin = Helper::GcPin(go);

	if (UT::String* nameStr = nullptr; Helper::SafeGetName(go, nameStr) && nameStr)
	{
		node.name = nameStr->ToString();
	}
	else
	{
		node.name = "(Unnamed)";
	}

	int childCount = 0;
	if (!Helper::SafeGetChildCount(transform, childCount)) return;

	if (childCount < 0) return;
	for (int i = 0; i < childCount && nodeBudget > 0; i++)
	{
		if (UT::Transform* child = nullptr; Helper::SafeGetChild(transform, i, child) && child)
		{
			HierarchyNode childNode;
			BuildHierarchyNode(childNode, child, visited, depth + 1, nodeBudget);
			if (childNode.gameObject)
			{
				node.children.push_back(std::move(childNode));
			}
		}
	}
}

void Inspector::RefreshHierarchy()
{
	rootNodes.clear();

	const auto assembly = UR::Get("UnityEngine.CoreModule.dll");
	if (!assembly) return;

	const auto transformClass = assembly->Get("Transform", "UnityEngine");
	if (!transformClass) return;
	std::vector<UT::Transform*> transforms = transformClass->FindObjectsByType<UT::Transform*>(1, 0);
	if (transforms.empty())
	{
		transforms = transformClass->FindObjectsOfType<UT::Transform*>();
		if (transforms.empty()) return;
	}

	std::unordered_set<UT::Transform*> visited;
	size_t nodeBudget = std::max<size_t>(transforms.size() * 2, 500000);
	for (const auto& t : transforms)
	{
		if (!Helper::SafeIsAlive(t)) continue;

		UT::Transform* parent = nullptr;
		if (!Helper::SafeGetParent(t, parent)) continue;

		if (!parent)
		{
			HierarchyNode node;
			BuildHierarchyNode(node, t, visited, 0, nodeBudget);
			if (node.gameObject)
			{
				rootNodes.push_back(std::move(node));
			}
		}
	}
}

void Inspector::RefreshTabData(InspectedObjectTab& tab) const
{
	if (!tab.gameObject || !Helper::SafeIsAlive(tab.gameObject)) return;

	tab.navigationStack.clear();
	tab.objectPath.clear();

	if (UT::Transform* transform = nullptr; Helper::SafeGetTransform(tab.gameObject, transform) && transform)
	{
		if (Helper::SafeIsAlive(transform))
		{
			tab.objectPath = BuildObjectPath(transform);
		}
	}

	const auto assembly = UR::Get("UnityEngine.CoreModule.dll");
	if (!assembly) return;

	if (const auto componentClass = assembly->Get("Component", "UnityEngine"))
	{
		if (!tab.gameObject) return;
		if (!tab.gameObject->IsAlive()) return;

		std::vector<UT::Component*> components;
		if (!Helper::SafeGetComponents(tab.gameObject, componentClass, components)) return;

		InspectionTarget rootTarget;
		rootTarget.gameObject = tab.gameObject;
		rootTarget.name = tab.tabName;

		for (auto& comp : components)
		{
			if (comp && Helper::SafeIsAlive(comp))
			{
				rootTarget.cachedComponents.push_back(comp);
				rootTarget.cachedComponentNames.push_back(GetComponentTypeName(comp));
				rootTarget.cachedComponentFields.push_back(GetComponentFields(comp));
				rootTarget.cachedComponentProperties.push_back(GetComponentProperties(comp));
				rootTarget.cachedComponentMethods.push_back(GetComponentMethods(comp));
			}
		}

		PinTarget(rootTarget);
		tab.navigationStack.push_back(std::move(rootTarget));
	}
}

bool Inspector::PassesComponentFilter(const std::string& componentName, std::string_view lowerSearch) const
{
	if (lowerSearch.empty()) return true;
	return Helper::CaseInsensitiveFind(componentName, lowerSearch);
}

bool Inspector::PassesFieldFilter(const ComponentFieldInfo& field, std::string_view lowerSearch,
                                  const bool editableOnly, const bool staticOnly, const bool instanceOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(field.name, lowerSearch)) return false;

	if (editableOnly && field.editableType == EditableType::None) return false;

	if (staticOnly && !field.isStatic) return false;

	if (instanceOnly && field.isStatic) return false;

	return true;
}

bool Inspector::PassesPropertyFilter(const ComponentPropertyInfo& prop, std::string_view lowerSearch,
                                     const bool editableOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(prop.name, lowerSearch)) return false;

	if (editableOnly && !prop.canWrite) return false;

	return true;
}

bool Inspector::PassesMethodFilter(const ComponentMethodInfo& method, std::string_view lowerSearch,
                                   const bool staticOnly, const bool instanceOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(method.name, lowerSearch)) return false;

	if (staticOnly && !method.isStatic) return false;

	if (instanceOnly && method.isStatic) return false;

	return true;
}

void Inspector::OpenObjectInNewTab(UT::GameObject* obj)
{
	if (!obj || !Helper::SafeIsAlive(obj)) return;

	if (const int existingIndex = FindTabForObject(obj); existingIndex >= 0)
	{
		SwitchToTab(existingIndex);
		return;
	}

	if (openTabs.size() >= maxTabs) return;

	InspectedObjectTab newTab;
	newTab.gameObject = obj;
	newTab.gameObjectPin = Helper::GcPin(obj);

	if (UT::String* name = nullptr; Helper::SafeGetName(obj, name) && name)
	{
		newTab.tabName = name->ToString();
	}
	else
	{
		newTab.tabName = "(Unknown)";
	}

	RefreshTabData(newTab);

	openTabs.push_back(std::move(newTab));
	activeTabIndex = static_cast<int>(openTabs.size()) - 1;
	pendingTabSwitch = true;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
}

void Inspector::CloseTab(const int tabIndex)
{
	if (tabIndex < 0 || std::cmp_greater_equal(tabIndex, static_cast<int>(openTabs.size()))) return;

	UnpinObject(openTabs[tabIndex].gameObject);
	openTabs.erase(openTabs.begin() + tabIndex);

	if (tabIndex < activeTabIndex) --activeTabIndex;
	if (std::cmp_greater_equal(activeTabIndex, openTabs.size()))
	{
		activeTabIndex = static_cast<int>(openTabs.size()) - 1;
	}

	if (openTabs.empty())
	{
		showDetailsWindow = false;
		activeTabIndex = -1;
	}
}

void Inspector::SwitchToTab(const int tabIndex)
{
	if (tabIndex < 0 || std::cmp_greater_equal(tabIndex, openTabs.size())) return;

	activeTabIndex = tabIndex;
	pendingTabSwitch = true;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
}

InspectedObjectTab* Inspector::GetActiveTab() const
{
	if (activeTabIndex < 0 || std::cmp_greater_equal(activeTabIndex, openTabs.size())) return nullptr;

	return const_cast<InspectedObjectTab*>(&openTabs[activeTabIndex]);
}

int Inspector::FindTabForObject(const UT::GameObject* obj) const
{
	for (size_t i = 0; i < openTabs.size(); ++i)
	{
		if (openTabs[i].gameObject == obj) return static_cast<int>(i);
	}
	return -1;
}

void Inspector::PinObject(UT::GameObject* obj)
{
	if (!obj || !Helper::SafeIsAlive(obj)) return;

	if (!IsObjectPinned(obj))
	{
		pinnedObjects.push_back(obj);
	}
}

void Inspector::UnpinObject(UT::GameObject* obj)
{
	if (!obj) return;

	if (const auto it = std::ranges::find(pinnedObjects, obj); it != pinnedObjects.end())
	{
		pinnedObjects.erase(it);
	}
}

bool Inspector::IsObjectPinned(UT::GameObject* obj)
{
	return std::ranges::find(pinnedObjects, obj) != pinnedObjects.end();
}
