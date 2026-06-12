#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

#define API(fn) (mono ? "mono_" fn : "il2cpp_" fn)


std::string Inspector::GetComponentTypeName(UT::Component* component) const
{
	if (!component) return "Unknown";

	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (void* klass = Helper::SafeGetObjectClass(component))
	{
		if (const char* className = UR::Invoke<const char*, void*>(API("class_get_name"), klass))
			return {className};
	}

	return "Component";
}

std::vector<ComponentFieldInfo> Inspector::GetObjectFields(void* obj, void* klass) const
{
	std::vector<ComponentFieldInfo> fields;
	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	if (!klass && obj)
		klass = Helper::SafeGetObjectClass(obj);

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
			info.name = fieldName ? fieldName : "(unknown)";

			info.offset = UR::Invoke<int, void*>(API("field_get_offset"), field);

			const int flags = UR::Invoke<int, void*>(API("field_get_flags"), field);
			info.isStatic = (flags & 0x10) != 0;

			if (!info.isStatic && UR::Invoke<bool, void*>(API("class_is_valuetype"), currentClass))
			{
				info.offset -= 0x10; // sizeof(Object) header
			}

			if (void* fieldType = UR::Invoke<void*, void*>(API("field_get_type"), field))
			{
				const char* typeName = UR::Invoke<const char*, void*>(API("type_get_name"), fieldType);
				info.typeName = typeName ? typeName : "unknown";

				if (mono)
				{
					info.typeClassHandle = UR::Invoke<void*, void*>("mono_class_from_mono_type", fieldType);
					if (!info.typeClassHandle) // fallback
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

	if (!klass && obj)
		klass = Helper::SafeGetObjectClass(obj);

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
			info.name = propName ? propName : "(unknown)";

			info.getterHandle = UR::Invoke<void*, void*>(API("property_get_get_method"), prop);
			info.setterHandle = UR::Invoke<void*, void*>(API("property_get_set_method"), prop);
			info.canRead = info.getterHandle != nullptr;
			info.canWrite = info.setterHandle != nullptr;

			if (mono && info.getterHandle)
			{
				if (void* sig = UR::Invoke<void*, void*>("mono_method_signature", info.getterHandle))
				{
					if (void* retType = UR::Invoke<void*, void*>("mono_signature_get_return_type", sig))
					{
						const char* typeName = UR::Invoke<const char*, void*>("mono_type_get_name", retType);
						info.typeName = typeName ? typeName : "unknown";
					}
				}
			}
			else if (info.getterHandle)
			{
				if (void* retType = UR::Invoke<void*, void*>("il2cpp_method_get_return_type", info.getterHandle))
				{
					const char* typeName = UR::Invoke<const char*, void*>("il2cpp_type_get_name", retType);
					info.typeName = typeName ? typeName : "unknown";
				}
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

	if (!klass && obj)
		klass = Helper::SafeGetObjectClass(obj);

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
			info.name = methodName ? methodName : "(unknown)";

			int fFlags = 0;
			info.flags = UR::Invoke<int, void*, int*>(API("method_get_flags"), method, &fFlags);
			info.isStatic = (info.flags & 0x10) != 0;
			info.isVirtual = (info.flags & 0x40) != 0;

			if (mono)
			{
				const auto signature = UR::Invoke<void*, void*>("mono_method_signature", method);
				if (!signature) continue;

				if (void* returnType = UR::Invoke<void*, void*>("mono_signature_get_return_type", signature))
				{
					const char* typeName = UR::Invoke<const char*, void*>("mono_type_get_name", returnType);
					info.returnTypeName = typeName ? typeName : "void";
				}
				else
				{
					info.returnTypeName = "void";
				}

				if (int paramCount = UR::Invoke<int, void*>("mono_signature_get_param_count", signature); paramCount >
					0)
				{
					std::vector<char*> paramNames(paramCount);
					UR::Invoke<void, void*, char**>("mono_method_get_param_names", method, paramNames.data());

					void* mIter = nullptr;
					void* mType;
					int paramIndex = 0;
					while (((mType = UR::Invoke<void*, void*, void*>("mono_signature_get_params", signature, &mIter)))
						&& paramIndex < paramCount)
					{
						const char* paramTypeName = UR::Invoke<const char*, void*>("mono_type_get_name", mType);
						std::string pName = (std::cmp_less(paramIndex, static_cast<int>(paramNames.size())) &&
							                    paramNames[paramIndex])
							                    ? paramNames[paramIndex]
							                    : "arg" + std::to_string(paramIndex);
						std::string pType = paramTypeName ? paramTypeName : "unknown";
						info.parameters.emplace_back(pName, pType);
						info.parameterEditableTypes.push_back(DetermineEditableType(pType));
						paramIndex++;
					}
				}
			}
			else
			{
				if (void* returnType = UR::Invoke<void*, void*>("il2cpp_method_get_return_type", method))
				{
					const char* typeName = UR::Invoke<const char*, void*>("il2cpp_type_get_name", returnType);
					info.returnTypeName = typeName ? typeName : "void";
				}
				else
				{
					info.returnTypeName = "void";
				}

				int paramCount = UR::Invoke<int, void*>("il2cpp_method_get_param_count", method);
				for (int i = 0; i < paramCount; i++)
				{
					const char* pName = UR::Invoke<const char*, void*, int>("il2cpp_method_get_param_name", method, i);
					void* pType = UR::Invoke<void*, void*, int>("il2cpp_method_get_param", method, i);
					const char* pTypeName = pType
						                        ? UR::Invoke<const char*, void*>("il2cpp_type_get_name", pType)
						                        : nullptr;

					std::string typeName = pTypeName ? pTypeName : "unknown";
					info.parameters.emplace_back(
						pName ? pName : "arg" + std::to_string(i),
						typeName
					);
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
	void* result = Helper::SafeInvokeMethod(obj, method.methodHandle,
	                                        params.empty() ? nullptr : params.data(), success);

	return success ? result : nullptr;
}

std::string Inspector::BuildObjectPath(UT::Transform* transform) const
{
	if (!transform) return "";

	std::vector<std::string> pathParts;
	UT::Transform* current = transform;

	while (current && Helper::SafeIsAlive(current))
	{
		UT::GameObject* go = nullptr;
		if (!Helper::SafeGetGameObject(current, go) || !go)
			break;

		if (!Helper::SafeIsAlive(go))
			break;

		if (UT::String* name = nullptr; Helper::SafeGetName(go, name) && name)
		{
			pathParts.push_back(name->ToString());
		}

		UT::Transform* parent = nullptr;
		if (!Helper::SafeGetParent(current, parent))
			break;
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

void Inspector::BuildHierarchyNode(HierarchyNode& node, UT::Transform* transform)
{
	if (!Helper::SafeIsAlive(transform)) return;

	node.transform = transform;

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

	if (UT::String* nameStr = nullptr; Helper::SafeGetName(go, nameStr) && nameStr)
	{
		node.name = nameStr->ToString();
	}
	else
	{
		node.name = "(Unnamed)";
	}

	int childCount = 0;
	if (!Helper::SafeGetChildCount(transform, childCount))
		return;

	for (int i = 0; i < childCount; i++)
	{
		if (UT::Transform* child = nullptr; Helper::SafeGetChild(transform, i, child) && child)
		{
			HierarchyNode childNode;
			BuildHierarchyNode(childNode, child);
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
	std::vector<UT::Transform*> transforms = transformClass->FindObjectsByType<UT::Transform*>();
	if (transforms.empty())
	{
		transforms = transformClass->FindObjectsOfType<UT::Transform*>();
		if (transforms.empty()) return;
	}

	for (const auto& t : transforms)
	{
		if (!Helper::SafeIsAlive(t)) continue;

		UT::Transform* parent = nullptr;
		if (!Helper::SafeGetParent(t, parent))
			continue;

		if (!parent)
		{
			HierarchyNode node;
			BuildHierarchyNode(node, t);
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

		tab.navigationStack.push_back(std::move(rootTarget));
	}
}

bool Inspector::PassesComponentFilter(const std::string& componentName, std::string_view lowerSearch) const
{
	if (lowerSearch.empty()) return true;
	return Helper::CaseInsensitiveFind(componentName, lowerSearch);
}

bool Inspector::PassesFieldFilter(const ComponentFieldInfo& field, std::string_view lowerSearch, const bool editableOnly,
                                  const bool staticOnly, const bool instanceOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(field.name, lowerSearch))
		return false;

	if (editableOnly && field.editableType == EditableType::None)
		return false;

	if (staticOnly && !field.isStatic)
		return false;

	if (instanceOnly && field.isStatic)
		return false;

	return true;
}

bool Inspector::PassesPropertyFilter(const ComponentPropertyInfo& prop, std::string_view lowerSearch,
                                     const bool editableOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(prop.name, lowerSearch))
		return false;

	if (editableOnly && !prop.canWrite)
		return false;

	return true;
}

bool Inspector::PassesMethodFilter(const ComponentMethodInfo& method, std::string_view lowerSearch, const bool staticOnly,
                                   const bool instanceOnly) const
{
	if (!lowerSearch.empty() && !Helper::CaseInsensitiveFind(method.name, lowerSearch))
		return false;

	if (staticOnly && !method.isStatic)
		return false;

	if (instanceOnly && method.isStatic)
		return false;

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

	if (openTabs.size() >= maxTabs)
		return;

	InspectedObjectTab newTab;
	newTab.gameObject = obj;

	if (UT::String* name = nullptr; Helper::SafeGetName(obj, name) && name)
	{
		newTab.tabName = name->ToString();
	}
	else
	{
		newTab.tabName = "(Unknown)";
	}

	RefreshTabData(newTab);

	openTabs.push_back(newTab);
	activeTabIndex = static_cast<int>(openTabs.size()) - 1;
	pendingTabSwitch = true;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
	AddToRecentSelections(obj);
}

void Inspector::CloseTab(const int tabIndex)
{
	if (tabIndex < 0 || std::cmp_greater_equal(tabIndex, static_cast<int>(openTabs.size())))
		return;

	openTabs.erase(openTabs.begin() + tabIndex);

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
	if (tabIndex < 0 || std::cmp_greater_equal(tabIndex, openTabs.size()))
		return;

	activeTabIndex = tabIndex;
	pendingTabSwitch = true;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
}

InspectedObjectTab* Inspector::GetActiveTab() const
{
	if (activeTabIndex < 0 || std::cmp_greater_equal(activeTabIndex, openTabs.size()))
		return nullptr;

	return const_cast<InspectedObjectTab*>(&openTabs[activeTabIndex]);
}

int Inspector::FindTabForObject(const UT::GameObject* obj) const
{
	for (size_t i = 0; i < openTabs.size(); ++i)
	{
		if (openTabs[i].gameObject == obj)
			return static_cast<int>(i);
	}
	return -1;
}

void Inspector::AddToRecentSelections(UT::GameObject* obj)
{
	if (!obj) return;

	if (const auto it = std::ranges::find(recentSelections, obj); it != recentSelections.end())
	{
		recentSelections.erase(it);
	}

	recentSelections.push_front(obj);

	while (recentSelections.size() > 10)
	{
		recentSelections.pop_back();
	}
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
