#include "pch.h"
#include "inspector.h"

std::string Inspector::GetComponentFullTypeName(UT::Component* component) const
{
	if (!component) return "Unknown";

	if (Config::state.unityMode == UnityResolve::Mode::Mono)
	{
		if (void* monoClass = UR::Invoke<void*, void*>("mono_object_get_class", component))
		{
			const char* className = UR::Invoke<const char*, void*>("mono_class_get_name", monoClass);
			const char* nameSpace = UR::Invoke<const char*, void*>("mono_class_get_namespace", monoClass);

			if (className)
			{
				std::string fullName;
				if (nameSpace && strlen(nameSpace) > 0)
				{
					fullName = std::string(nameSpace) + "." + std::string(className);
				}
				else
				{
					fullName = std::string(className);
				}
				return fullName;
			}
		}
	}
	else
	{
		if (void* monoClass = UR::Invoke<void*, void*>("il2cpp_object_get_class", component))
		{
			const char* className = UR::Invoke<const char*, void*>("il2cpp_class_get_name", monoClass);
			const char* nameSpace = UR::Invoke<const char*, void*>("il2cpp_class_get_namespace", monoClass);

			if (className)
			{
				std::string fullName;
				if (nameSpace && strlen(nameSpace) > 0)
				{
					fullName = std::string(nameSpace) + "." + std::string(className);
				}
				else
				{
					fullName = std::string(className);
				}
				return fullName;
			}
		}
	}

	return "Component";
}

std::vector<ComponentFieldInfo> Inspector::GetComponentFields(UT::Component* component) const
{
	std::vector<ComponentFieldInfo> fields;
	if (!component) return fields;

	if (Config::state.unityMode == UnityResolve::Mode::Mono)
	{
		void* monoClass = UR::Invoke<void*, void*>("mono_object_get_class", component);
		if (!monoClass) return fields;

		void* currentClass = monoClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* field;

			while ((field = UR::Invoke<void*, void*, void*>("mono_class_get_fields", currentClass, &iter)))
			{
				ComponentFieldInfo info;
				info.fieldHandle = field;
				info.classHandle = currentClass;

				const char* fieldName = UR::Invoke<const char*, void*>("mono_field_get_name", field);
				info.name = fieldName ? fieldName : "(unknown)";

				info.offset = UR::Invoke<int, void*>("mono_field_get_offset", field);

				int flags = UR::Invoke<int, void*>("mono_field_get_flags", field);
				info.isStatic = (flags & 0x10) != 0;

				if (void* fieldType = UR::Invoke<void*, void*>("mono_field_get_type", field))
				{
					const char* typeName = UR::Invoke<const char*, void*>("mono_type_get_name", fieldType);
					info.typeName = typeName ? typeName : "unknown";
				}
				else
				{
					info.typeName = "unknown";
				}

				info.editableType = DetermineEditableType(info.typeName);

				fields.push_back(info);
			}

			currentClass = UR::Invoke<void*, void*>("mono_class_get_parent", currentClass);
		}
	}
	else
	{
		void* il2cppClass = UR::Invoke<void*, void*>("il2cpp_object_get_class", component);
		if (!il2cppClass) return fields;

		void* currentClass = il2cppClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* field;

			while ((field = UR::Invoke<void*, void*, void*>("il2cpp_class_get_fields", currentClass, &iter)))
			{
				ComponentFieldInfo info;
				info.fieldHandle = field;
				info.classHandle = currentClass;

				const char* fieldName = UR::Invoke<const char*, void*>("il2cpp_field_get_name", field);
				info.name = fieldName ? fieldName : "(unknown)";

				info.offset = UR::Invoke<int, void*>("il2cpp_field_get_offset", field);

				int flags = UR::Invoke<int, void*>("il2cpp_field_get_flags", field);
				info.isStatic = (flags & 0x10) != 0;

				if (void* fieldType = UR::Invoke<void*, void*>("il2cpp_field_get_type", field))
				{
					const char* typeName = UR::Invoke<const char*, void*>("il2cpp_type_get_name", fieldType);
					info.typeName = typeName ? typeName : "unknown";
				}
				else
				{
					info.typeName = "unknown";
				}

				info.editableType = DetermineEditableType(info.typeName);

				fields.push_back(info);
			}

			currentClass = UR::Invoke<void*, void*>("il2cpp_class_get_parent", currentClass);
		}
	}

	return fields;
}

std::vector<ComponentPropertyInfo> Inspector::GetComponentProperties(UT::Component* component) const
{
	std::vector<ComponentPropertyInfo> properties;
	if (!component) return properties;

	if (Config::state.unityMode == UnityResolve::Mode::Mono)
	{
		void* monoClass = UR::Invoke<void*, void*>("mono_object_get_class", component);
		if (!monoClass) return properties;

		void* currentClass = monoClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* prop;

			while ((prop = UR::Invoke<void*, void*, void*>("mono_class_get_properties", currentClass, &iter)))
			{
				ComponentPropertyInfo info;

				const char* propName = UR::Invoke<const char*, void*>("mono_property_get_name", prop);
				info.name = propName ? propName : "(unknown)";

				info.getterHandle = UR::Invoke<void*, void*>("mono_property_get_get_method", prop);
				info.setterHandle = UR::Invoke<void*, void*>("mono_property_get_set_method", prop);
				info.canRead = info.getterHandle != nullptr;
				info.canWrite = info.setterHandle != nullptr;

				if (info.getterHandle)
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

				info.editableType = DetermineEditableType(info.typeName);
				properties.push_back(info);
			}

			currentClass = UR::Invoke<void*, void*>("mono_class_get_parent", currentClass);
		}
	}
	else
	{
		void* il2cppClass = UR::Invoke<void*, void*>("il2cpp_object_get_class", component);
		if (!il2cppClass) return properties;

		void* currentClass = il2cppClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* prop;

			while ((prop = UR::Invoke<void*, void*, void*>("il2cpp_class_get_properties", currentClass, &iter)))
			{
				ComponentPropertyInfo info;

				const char* propName = UR::Invoke<const char*, void*>("il2cpp_property_get_name", prop);
				info.name = propName ? propName : "(unknown)";

				info.getterHandle = UR::Invoke<void*, void*>("il2cpp_property_get_get_method", prop);
				info.setterHandle = UR::Invoke<void*, void*>("il2cpp_property_get_set_method", prop);
				info.canRead = info.getterHandle != nullptr;
				info.canWrite = info.setterHandle != nullptr;

				if (info.getterHandle)
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

			currentClass = UR::Invoke<void*, void*>("il2cpp_class_get_parent", currentClass);
		}
	}

	return properties;
}

std::vector<ComponentMethodInfo> Inspector::GetComponentMethods(UT::Component* component) const
{
	std::vector<ComponentMethodInfo> methods;
	if (!component) return methods;

	if (Config::state.unityMode == UnityResolve::Mode::Mono)
	{
		void* monoClass = UR::Invoke<void*, void*>("mono_object_get_class", component);
		if (!monoClass) return methods;

		void* currentClass = monoClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* method;

			while ((method = UR::Invoke<void*, void*, void*>("mono_class_get_methods", currentClass, &iter)))
			{
				const auto signature = UR::Invoke<void*, void*>("mono_method_signature", method);
				if (!signature) continue;

				ComponentMethodInfo info;
				info.methodHandle = method;

				const char* methodName = UR::Invoke<const char*, void*>("mono_method_get_name", method);
				info.name = methodName ? methodName : "(unknown)";

				if (void* returnType = UR::Invoke<void*, void*>("mono_signature_get_return_type", signature))
				{
					const char* typeName = UR::Invoke<const char*, void*>("mono_type_get_name", returnType);
					info.returnTypeName = typeName ? typeName : "void";
				}
				else
				{
					info.returnTypeName = "void";
				}

				int fFlags = 0;
				info.flags = UR::Invoke<int, void*, int*>("mono_method_get_flags", method, &fFlags);
				info.isStatic = (info.flags & 0x10) != 0;
				info.isVirtual = (info.flags & 0x40) != 0;

				int paramCount = UR::Invoke<int, void*>("mono_signature_get_param_count", signature);
				if (paramCount > 0)
				{
					std::vector<char*> paramNames(paramCount);
					UR::Invoke<void, void*, char**>("mono_method_get_param_names", method, paramNames.data());

					void* mIter = nullptr;
					void* mType;
					int paramIndex = 0;
					while (((mType = UR::Invoke<void*, void*, void*>("mono_signature_get_params", signature, &mIter))) && paramIndex < paramCount)
					{
						const char* paramTypeName = UR::Invoke<const char*, void*>("mono_type_get_name", mType);
						std::string pName = (std::cmp_less(paramIndex, static_cast<int>(paramNames.size())) && paramNames[paramIndex])
							? paramNames[paramIndex] : "arg" + std::to_string(paramIndex);
						std::string pType = paramTypeName ? paramTypeName : "unknown";
						info.parameters.emplace_back(pName, pType);
						info.parameterEditableTypes.push_back(DetermineEditableType(pType));
						paramIndex++;
					}
				}

				methods.push_back(std::move(info));
			}

			currentClass = UR::Invoke<void*, void*>("mono_class_get_parent", currentClass);
		}
	}
	else
	{
		void* il2cppClass = UR::Invoke<void*, void*>("il2cpp_object_get_class", component);
		if (!il2cppClass) return methods;

		void* currentClass = il2cppClass;
		while (currentClass)
		{
			void* iter = nullptr;
			void* method;

			while ((method = UR::Invoke<void*, void*, void*>("il2cpp_class_get_methods", currentClass, &iter)))
			{
				ComponentMethodInfo info;
				info.methodHandle = method;

				const char* methodName = UR::Invoke<const char*, void*>("il2cpp_method_get_name", method);
				info.name = methodName ? methodName : "(unknown)";

				if (void* returnType = UR::Invoke<void*, void*>("il2cpp_method_get_return_type", method))
				{
					const char* typeName = UR::Invoke<const char*, void*>("il2cpp_type_get_name", returnType);
					info.returnTypeName = typeName ? typeName : "void";
				}
				else
				{
					info.returnTypeName = "void";
				}

				int fFlags = 0;
				info.flags = UR::Invoke<int, void*, int*>("il2cpp_method_get_flags", method, &fFlags);
				info.isStatic = (info.flags & 0x10) != 0;
				info.isVirtual = (info.flags & 0x40) != 0;

				int paramCount = UR::Invoke<int, void*>("il2cpp_method_get_param_count", method);
				for (int i = 0; i < paramCount; i++)
				{
					const char* pName = UR::Invoke<const char*, void*, int>("il2cpp_method_get_param_name", method, i);
					void* pType = UR::Invoke<void*, void*, int>("il2cpp_method_get_param", method, i);
					const char* pTypeName = pType ? UR::Invoke<const char*, void*>("il2cpp_type_get_name", pType) : nullptr;

					std::string typeName = pTypeName ? pTypeName : "unknown";
					info.parameters.emplace_back(
						pName ? pName : "arg" + std::to_string(i),
						typeName
					);
					info.parameterEditableTypes.push_back(DetermineEditableType(typeName));
				}

				methods.push_back(std::move(info));
			}

			currentClass = UR::Invoke<void*, void*>("il2cpp_class_get_parent", currentClass);
		}
	}

	return methods;
}

std::string Inspector::GetComponentTypeName(UT::Component* component) const
{
	if (!component) return "Unknown";

	if (Config::state.unityMode == UnityResolve::Mode::Mono)
	{
		if (void* monoClass = UR::Invoke<void*, void*>("mono_object_get_class", component))
		{
			if (const char* className = UR::Invoke<const char*, void*>("mono_class_get_name", monoClass))
			{
				return { className };
			}
		}
	}
	else
	{
		if (void* il2cppClass = UR::Invoke<void*, void*>("il2cpp_object_get_class", component))
		{
			if (const char* className = UR::Invoke<const char*, void*>("il2cpp_class_get_name", il2cppClass))
			{
				return {className};
			}
		}
	}

	return "Component";
}

void* Inspector::InvokeMethod(UT::Component* component, const ComponentMethodInfo& method, const std::vector<std::string>& paramValues) const
{
	if (!method.methodHandle) return nullptr;

	auto paramBuffers = Helper::BuildInvokeParams(paramValues, method.parameterEditableTypes);

	bool success = false;
	void* obj = method.isStatic ? nullptr : component;
	void* result = Helper::SafeInvokeMethod(obj, method.methodHandle,
		paramBuffers.params.empty() ? nullptr : paramBuffers.params.data(), success);

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

		UT::String* name = nullptr;
		if (Helper::SafeGetName(go, name) && name)
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

	UT::String* nameStr = nullptr;
	if (Helper::SafeGetName(go, nameStr) && nameStr)
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
		UT::Transform* child = nullptr;
		if (Helper::SafeGetChild(transform, i, child) && child)
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
	std::vector<UT::Transform*> transforms;
	transforms = transformClass->FindObjectsByType<UT::Transform*>();
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
	tab.cachedComponents.clear();
	tab.cachedComponentNames.clear();
	tab.cachedComponentFields.clear();
	tab.cachedComponentProperties.clear();
	tab.cachedComponentMethods.clear();
	tab.objectPath.clear();

	if (!tab.gameObject || !Helper::SafeIsAlive(tab.gameObject)) return;

	UT::Transform* transform = nullptr;
	if (Helper::SafeGetTransform(tab.gameObject, transform) && transform)
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

		for (auto& comp : components)
		{

			if (comp && Helper::SafeIsAlive(comp))
			{
				tab.cachedComponents.push_back(comp);
				tab.cachedComponentNames.push_back(GetComponentTypeName(comp));
				tab.cachedComponentFields.push_back(GetComponentFields(comp));
				tab.cachedComponentProperties.push_back(GetComponentProperties(comp));
				tab.cachedComponentMethods.push_back(GetComponentMethods(comp));
			}

		}
	}
}

bool Inspector::PassesComponentFilter(const std::string& componentName, const char* searchBuffer) const
{
	if (!searchBuffer || searchBuffer[0] == '\0') return true;

	std::string lowerName = componentName;
	std::string lowerSearch = searchBuffer;
	std::ranges::transform(lowerName, lowerName.begin(), ::tolower);
	std::ranges::transform(lowerSearch, lowerSearch.begin(), ::tolower);

	return lowerName.find(lowerSearch) != std::string::npos;
}

bool Inspector::PassesFieldFilter(const ComponentFieldInfo& field, const char* searchBuffer, const bool editableOnly, const bool staticOnly, const bool instanceOnly) const
{
	if (searchBuffer && searchBuffer[0] != '\0')
	{
		std::string lowerName = field.name;
		std::string lowerSearch = searchBuffer;
		std::ranges::transform(lowerName, lowerName.begin(), ::tolower);
		std::ranges::transform(lowerSearch, lowerSearch.begin(), ::tolower);

		if (lowerName.find(lowerSearch) == std::string::npos)
			return false;
	}

	if (editableOnly && field.editableType == EditableType::None)
		return false;

	if (staticOnly && !field.isStatic)
		return false;

	if (instanceOnly && field.isStatic)
		return false;

	return true;
}

bool Inspector::PassesPropertyFilter(const ComponentPropertyInfo& prop, const char* searchBuffer, const bool editableOnly) const
{
	if (searchBuffer && searchBuffer[0] != '\0')
	{
		std::string lowerName = prop.name;
		std::string lowerSearch = searchBuffer;
		std::ranges::transform(lowerName, lowerName.begin(), ::tolower);
		std::ranges::transform(lowerSearch, lowerSearch.begin(), ::tolower);

		if (lowerName.find(lowerSearch) == std::string::npos)
			return false;
	}

	if (editableOnly && !prop.canWrite)
		return false;

	return true;
}

bool Inspector::PassesMethodFilter(const ComponentMethodInfo& method, const char* searchBuffer, const bool staticOnly, const bool instanceOnly) const
{
	if (searchBuffer && searchBuffer[0] != '\0')
	{
		std::string lowerName = method.name;
		std::string lowerSearch = searchBuffer;
		std::ranges::transform(lowerName, lowerName.begin(), ::tolower);
		std::ranges::transform(lowerSearch, lowerSearch.begin(), ::tolower);

		if (lowerName.find(lowerSearch) == std::string::npos)
			return false;
	}

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
	{
		return;
	}

	InspectedObjectTab newTab;
	newTab.gameObject = obj;

	UT::String* name = nullptr;
	if (Helper::SafeGetName(obj, name) && name)
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
	showDetailsWindow = true;
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
	showDetailsWindow = true;
}

InspectedObjectTab* Inspector::GetActiveTab()
{
	if (activeTabIndex < 0 || std::cmp_greater_equal(activeTabIndex, openTabs.size()))
		return nullptr;

	return &openTabs[activeTabIndex];
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
