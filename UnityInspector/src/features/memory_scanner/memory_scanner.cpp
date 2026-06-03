#include "pch.h"
#include "memory_scanner.h"
#include "features/inspector/inspector.h"
#include "helper/helper.h"

REGISTER_FEATURE(MemoryScanner)

namespace
{
	template <typename T>
	bool SafeRead(void* ptr, int offset, T& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	bool SafeReadBool(void* ptr, int offset, bool& outValue)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			outValue = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(ptr) + offset);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}
}

void MemoryScanner::Update(float deltaTime)
{
	(void)deltaTime;

	if (pendingOperation != ScanOperation::None && !scanInProgress)
	{
		scanInProgress = true;
		try
		{
			UR::ThreadAttach();

			switch (pendingOperation)
			{
			case ScanOperation::FirstScan:
				PerformFirstScan();
				break;
			case ScanOperation::NextScan:
				PerformNextScan();
				break;
			case ScanOperation::Reset:
				ResetScan();
				break;
			default:
				break;
			}
		}
		catch (const std::exception& e)
		{
			statusText = std::string("Scan error: ") + e.what();
			scanInProgress = false;
		}
		catch (...)
		{
			statusText = "Scan error: unknown exception";
			scanInProgress = false;
		}
		pendingOperation = ScanOperation::None;
	}
}

void MemoryScanner::Render()
{
	if (!Config::state.showMenu) return;
	if (!Config::settings.memoryScanner.showWindow) return;

	if (ImGui::Begin("Memory Scanner", &Config::settings.memoryScanner.showWindow))
	{
		UR::ThreadAttach();

		const bool locked = hasDoneFirstScan || scanInProgress;

		ImGui::Text("Value Type:");
		ImGui::SameLine();
		const char* typeNames[] = {"Int (8/16/32-bit)", "Long (64-bit)", "Float", "Double", "Bool"};
		int typeIndex = static_cast<int>(selectedType);
		if (locked) ImGui::BeginDisabled();
		if (ImGui::Combo("##Type", &typeIndex, typeNames, IM_ARRAYSIZE(typeNames)))
		{
			selectedType = static_cast<ScanValueType>(typeIndex);
			SyncValueToBuffer();
		}
		if (locked) ImGui::EndDisabled();

		ImGui::Text("Value:");
		ImGui::SameLine();
		RenderValueInput();

		ImGui::Text("Comparison:");
		ImGui::SameLine();
		const char* compNames[] = {"Exact", "Increased", "Decreased", "Changed", "Unchanged"};
		int compIndex = static_cast<int>(comparison);
		if (ImGui::Combo("##Comp", &compIndex, compNames, IM_ARRAYSIZE(compNames)))
		{
			comparison = static_cast<ScanComparison>(compIndex);
		}

		ImGui::Checkbox("Include System/Unity namespaces", &includeSystemNamespaces);
		ImGui::Checkbox("Debug logging (console)", &debugLogging);

		ImGui::Separator();
		auto statusColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		if (scanInProgress) statusColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
		else if (!statusText.empty() && statusText.find("error") != std::string::npos)
			statusColor = ImVec4(
				1.0f, 0.0f, 0.0f, 1.0f);

		if (scanInProgress)
		{
			ImGui::TextColored(statusColor, "Scanning... %s", statusText.c_str());
		}
		else
		{
			if (!hasDoneFirstScan)
			{
				if (ImGui::Button("First Scan", ImVec2(120, 0)))
				{
					pendingOperation = ScanOperation::FirstScan;
				}
			}
			else
			{
				if (ImGui::Button("Next Scan", ImVec2(120, 0)))
				{
					pendingOperation = ScanOperation::NextScan;
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset", ImVec2(120, 0)))
				{
					pendingOperation = ScanOperation::Reset;
				}
			}
		}

		ImGui::SameLine();
		ImGui::TextDisabled("Results: %zu / %zu", currentResults.size(), MAX_RESULTS);

		if (!statusText.empty() && !scanInProgress)
		{
			ImGui::SameLine();
			ImGui::TextColored(statusColor, "%s", statusText.c_str());
		}

		ImGui::Separator();
		ImGui::InputTextWithHint("##Filter", "Filter results...", resultFilterBuffer, sizeof(resultFilterBuffer),
		                         ImGuiInputTextFlags_EscapeClearsAll);

		if (ImGui::BeginTable("Results", 6,
		                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
		                      ImGuiTableFlags_Sortable))
		{
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch, 0.1f);
			ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_WidthStretch, 0.2f);
			ImGui::TableSetupColumn("Namespace", ImGuiTableColumnFlags_WidthStretch, 0.2f);
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableSetupColumn("Object", ImGuiTableColumnFlags_WidthStretch, 0.2f);
			ImGui::TableHeadersRow();

			std::string filterLower = resultFilterBuffer;
			std::ranges::transform(filterLower, filterLower.begin(), tolower);

			for (size_t i = 0; i < currentResults.size(); ++i)
			{
				const auto& result = currentResults[i];

				if (!filterLower.empty())
				{
					auto contains = [&](const std::string& s)
					{
						std::string lower = s;
						std::ranges::transform(lower, lower.begin(), tolower);
						return lower.find(filterLower) != std::string::npos;
					};
					if (!contains(result.className) && !contains(result.namespaze) &&
						!contains(result.fieldName) && !contains(result.objectName) &&
						!contains(FormatFieldValue(result)))
					{
						continue;
					}
				}

				ImGui::TableNextRow();
				ImGui::PushID(static_cast<int>(i));

				ImGui::TableNextColumn();
				std::string valStr = FormatFieldValue(result);
				if (bool isSelected = (selectedResultIndex == static_cast<int>(i)); ImGui::Selectable(
					valStr.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedResultIndex = static_cast<int>(i);
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					OpenResultInInspector(result);
				}

				std::string popupName = "ResultContext_" + std::to_string(i);
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup(popupName.c_str());
				}
				if (ImGui::BeginPopup(popupName.c_str()))
				{
					if (ImGui::MenuItem("Inspect in Hierarchy"))
					{
						OpenResultInInspector(result);
					}
					if (ImGui::MenuItem("Copy Address"))
					{
						char addrStr[32];
						snprintf(addrStr, sizeof(addrStr), "%p", result.object);
						ImGui::SetClipboardText(addrStr);
					}
					ImGui::EndPopup();
				}

				ImGui::TableNextColumn();
				ImGui::Text("%s", GetValueTypeName(result.valueType));

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.className.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.namespaze.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.fieldName.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.objectName.c_str());

				ImGui::PopID();
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void MemoryScanner::RenderValueInput()
{
	switch (selectedType)
	{
	case ScanValueType::Int:
		{
			int64_t val = typedValue.i64;
			if (ImGui::InputScalar("##ValueInt", ImGuiDataType_S64, &val))
			{
				typedValue.i64 = val;
				snprintf(valueBuffer, sizeof(valueBuffer), "%lld", static_cast<long long>(val));
			}
			break;
		}
	case ScanValueType::Long:
		{
			int64_t val = typedValue.i64;
			if (ImGui::InputScalar("##ValueLong", ImGuiDataType_S64, &val))
			{
				typedValue.i64 = val;
				snprintf(valueBuffer, sizeof(valueBuffer), "%lld", static_cast<long long>(val));
			}
			break;
		}
	case ScanValueType::Float:
		{
			float val = typedValue.f32;
			if (ImGui::InputFloat("##ValueFloat", &val, 0.0f, 0.0f, "%.3f"))
			{
				typedValue.f32 = val;
				snprintf(valueBuffer, sizeof(valueBuffer), "%.3f", val);
			}
			break;
		}
	case ScanValueType::Double:
		{
			double val = typedValue.f64;
			if (ImGui::InputDouble("##ValueDouble", &val, 0.0, 0.0, "%.6f"))
			{
				typedValue.f64 = val;
				snprintf(valueBuffer, sizeof(valueBuffer), "%.6f", val);
			}
			break;
		}
	case ScanValueType::Bool:
		{
			bool val = typedValue.b;
			if (ImGui::Checkbox("##ValueBool", &val))
			{
				typedValue.b = val;
				snprintf(valueBuffer, sizeof(valueBuffer), "%s", val ? "true" : "false");
			}
			break;
		}
	}
}

void MemoryScanner::SyncValueToBuffer()
{
	switch (selectedType)
	{
	case ScanValueType::Int:
	case ScanValueType::Long:
		snprintf(valueBuffer, sizeof(valueBuffer), "%lld", static_cast<long long>(typedValue.i64));
		break;
	case ScanValueType::Float:
		snprintf(valueBuffer, sizeof(valueBuffer), "%.3f", typedValue.f32);
		break;
	case ScanValueType::Double:
		snprintf(valueBuffer, sizeof(valueBuffer), "%.6f", typedValue.f64);
		break;
	case ScanValueType::Bool:
		snprintf(valueBuffer, sizeof(valueBuffer), "%s", typedValue.b ? "true" : "false");
		break;
	}
}

void MemoryScanner::PerformFirstScan()
{
	currentResults.clear();

	if (comparison == ScanComparison::Exact)
	{
		bool valid = false;
		switch (selectedType)
		{
		case ScanValueType::Int:
		case ScanValueType::Long:
			valid = true;
			break;
		case ScanValueType::Float:
			{
				float f;
				valid = GetTargetValueAsFloat(f);
				break;
			}
		case ScanValueType::Double:
			{
				double d;
				valid = GetTargetValueAsDouble(d);
				break;
			}
		case ScanValueType::Bool:
			{
				bool b;
				valid = GetTargetValueAsBool(b);
				break;
			}
		}
		if (!valid)
		{
			statusText = "Invalid target value";
			scanInProgress = false;
			return;
		}
	}

	if (debugLogging)
		printf("[MemoryScanner] First scan: %s = %s\n", GetValueTypeName(selectedType), valueBuffer);

	statusText = "Scanning static fields...";
	ScanStaticFields(currentResults);
	if (debugLogging)
		printf("[MemoryScanner] Static matches: %zu\n", currentResults.size());

	if (currentResults.size() < MAX_RESULTS)
	{
		statusText = "Scanning objects...";
		ScanUnityObjectFields(currentResults);
	}

	statusText = "Scan complete. " + std::to_string(currentResults.size()) + " matches.";
	if (debugLogging)
		printf("[MemoryScanner] Total matches: %zu\n", currentResults.size());
	hasDoneFirstScan = true;
	scanInProgress = false;
	selectedResultIndex = -1;
}

void MemoryScanner::PerformNextScan()
{
	std::vector<ScanField> newResults;
	newResults.reserve(currentResults.size());

	for (auto& field : currentResults)
	{
		char currentValue[sizeof(double)] = {};
		if (!ReadFieldValue(field, currentValue))
			continue;

		bool match = false;

		switch (comparison)
		{
		case ScanComparison::Exact:
			match = CompareValueWithTarget(currentValue, field.valueType);
			break;
		case ScanComparison::Increased:
			match = CompareValueWithPrevious(currentValue, field) > 0;
			break;
		case ScanComparison::Decreased:
			match = CompareValueWithPrevious(currentValue, field) < 0;
			break;
		case ScanComparison::Changed:
			match = CompareValueWithPrevious(currentValue, field) != 0;
			break;
		case ScanComparison::Unchanged:
			match = CompareValueWithPrevious(currentValue, field) == 0;
			break;
		}

		if (match)
		{
			ScanField updated = field;
			memcpy(&updated.lastValue, currentValue, sizeof(double));
			newResults.push_back(updated);
		}
	}

	currentResults = std::move(newResults);
	statusText = "Next scan complete. " + std::to_string(currentResults.size()) + " matches.";
	scanInProgress = false;
	selectedResultIndex = -1;
}

void MemoryScanner::ResetScan()
{
	currentResults.clear();
	hasDoneFirstScan = false;
	statusText.clear();
	selectedResultIndex = -1;
	scanInProgress = false;
}

void MemoryScanner::ScanStaticFields(std::vector<ScanField>& out) const
{
	if (out.size() >= MAX_RESULTS)
		return;

	for (const auto& assembly : UR::assembly)
	{
		if (!assembly)
			continue;

		for (const auto& klass : assembly->classes)
		{
			if (!klass || !klass->address)
				continue;

			if (klass->m_name.find('`') != std::string::npos ||
				klass->m_name.find('<') != std::string::npos ||
				klass->m_name.find('$') != std::string::npos)
				continue;

			if (!includeSystemNamespaces &&
				(klass->namespaze.starts_with("System.") || klass->namespaze.starts_with("UnityEngine.") ||
					klass->namespaze.starts_with("Unity.")))
				continue;

			for (const auto& field : klass->fields)
			{
				if (out.size() >= MAX_RESULTS)
					return;
				if (!field || !field->static_field || !field->type)
					continue;

				std::string typeName = field->type->name;
				if (!TypeNameMatchesSearchType(typeName))
					continue;

				ScanValueType actualType = DetermineActualValueType(typeName);
				char value[sizeof(double)] = {};
				if (!ReadStaticFieldValue(field->address, value))
					continue;
				if (!CompareValueWithTarget(value, actualType))
					continue;

				ScanField scanField;
				scanField.fieldHandle = field->address;
				scanField.classHandle = klass->address;
				scanField.object = nullptr;
				scanField.isStatic = true;
				scanField.valueType = actualType;
				scanField.fieldName = field->name;
				scanField.className = klass->m_name;
				scanField.namespaze = klass->namespaze;
				scanField.objectName = "(static)";
				memcpy(&scanField.lastValue, value, sizeof(double));

				out.push_back(scanField);
			}
		}
	}
}

void MemoryScanner::ScanUnityObjectFields(std::vector<ScanField>& out)
{
	if (out.size() >= MAX_RESULTS)
		return;

	auto coreAssembly = UR::Get("UnityEngine.CoreModule.dll");
	if (!coreAssembly)
		return;

	auto objectClass = coreAssembly->Get("Object", "UnityEngine");
	if (!objectClass)
		return;

	std::vector<void*> allObjects;
	try
	{
		allObjects = objectClass->FindObjectsByType<void*>(1, 0);
	}
	catch (...)
	{
		allObjects.clear();
	}

	if (allObjects.empty())
	{
		try
		{
			allObjects = objectClass->FindObjectsByType<void*>();
		}
		catch (...)
		{
			allObjects.clear();
		}
	}

	if (allObjects.empty())
	{
		try
		{
			allObjects = objectClass->FindObjectsOfType<void*>();
		}
		catch (...)
		{
			allObjects.clear();
		}
	}

	if (allObjects.empty())
	{
		if (debugLogging)
			printf("[MemoryScanner] No Unity objects found\n");
		return;
	}

	if (allObjects.size() > MAX_OBJECTS_TO_SCAN)
		allObjects.resize(MAX_OBJECTS_TO_SCAN);

	if (debugLogging)
		printf("[MemoryScanner] Scanning %zu Unity objects...\n", allObjects.size());

	std::unordered_set<VisitedKey, VisitedKeyHash> visited;
	int processed = 0;

	for (void* obj : allObjects)
	{
		if (!obj)
			continue;
		if (out.size() >= MAX_RESULTS)
			break;

		void* klass = Helper::SafeGetObjectClass(obj);
		if (!klass)
			continue;

		std::string objName = "(unknown)";
		if (UT::String* nameStr = nullptr;
			Helper::SafeGetName(static_cast<UnityObject*>(obj), nameStr) && nameStr)
		{
			objName = nameStr->ToString();
		}

		ScanObjectInstance(obj, klass, out, visited, 0, objName);

		processed++;
		if (processed % 500 == 0)
		{
			statusText = "Scanning objects... " + std::to_string(processed) + "/" + std::to_string(allObjects.size());
		}
	}

	if (debugLogging)
		printf("[MemoryScanner] Scanned %d objects, found %zu matches\n", processed, out.size());
}

void MemoryScanner::ScanObjectInstance(void* obj, void* klass, std::vector<ScanField>& out,
                                       std::unordered_set<VisitedKey, VisitedKeyHash>& visited, int depth,
                                       const std::string& objName)
{
	if (!obj || !klass)
		return;
	if (depth > MAX_SCAN_DEPTH)
		return;

	VisitedKey key{obj, klass};
	if (visited.contains(key))
		return;
	visited.insert(key);

	if (out.size() >= MAX_RESULTS)
		return;

	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	void* currentClass = klass;
	while (currentClass)
	{
		if (!includeSystemNamespaces)
		{
			const char* ns = UR::Invoke<const char*, void*>(
				mono ? "mono_class_get_namespace" : "il2cpp_class_get_namespace", currentClass);
			if (ns && (std::string_view(ns).starts_with("System.") ||
				std::string_view(ns).starts_with("UnityEngine.") ||
				std::string_view(ns).starts_with("Unity.")))
			{
				currentClass = UR::Invoke<void*, void*>(
					mono ? "mono_class_get_parent" : "il2cpp_class_get_parent", currentClass);
				continue;
			}
		}

		void* iter = nullptr;
		void* field;

		while ((field = UR::Invoke<void*, void*, void*>(
			mono ? "mono_class_get_fields" : "il2cpp_class_get_fields", currentClass, &iter)))
		{
			if (out.size() >= MAX_RESULTS)
				break;

			const char* fieldName = UR::Invoke<const char*, void*>(
				mono ? "mono_field_get_name" : "il2cpp_field_get_name", field);
			int offset = UR::Invoke<int, void*>(
				mono ? "mono_field_get_offset" : "il2cpp_field_get_offset", field);
			int flags = UR::Invoke<int, void*>(
				mono ? "mono_field_get_flags" : "il2cpp_field_get_flags", field);

			if ((flags & 0x10) != 0)
				continue;

			void* fieldType = UR::Invoke<void*, void*>(
				mono ? "mono_field_get_type" : "il2cpp_field_get_type", field);
			if (!fieldType)
				continue;

			const char* typeName = UR::Invoke<const char*, void*>(
				mono ? "mono_type_get_name" : "il2cpp_type_get_name", fieldType);

			if (std::string typeNameStr = typeName ? typeName : "unknown"; TypeNameMatchesSearchType(typeNameStr))
			{
				ScanValueType actualType = DetermineActualValueType(typeNameStr);
				char value[sizeof(double)] = {};
				if (ReadInstanceFieldValue(obj, offset, actualType, value))
				{
					if (CompareValueWithTarget(value, actualType))
					{
						ScanField scanField;
						scanField.fieldHandle = field;
						scanField.classHandle = currentClass;
						scanField.object = obj;
						scanField.isStatic = false;
						scanField.offset = offset;
						scanField.valueType = actualType;
						scanField.fieldName = fieldName ? fieldName : "unknown";
						scanField.className = UR::Invoke<const char*, void*>(
							mono ? "mono_class_get_name" : "il2cpp_class_get_name", currentClass);
						if (scanField.className.empty())
							scanField.className = "Unknown";
						const char* ns = UR::Invoke<const char*, void*>(
							mono ? "mono_class_get_namespace" : "il2cpp_class_get_namespace", currentClass);
						scanField.namespaze = ns ? ns : "";
						scanField.objectName = objName;
						memcpy(&scanField.lastValue, value, sizeof(double));
						out.push_back(scanField);
					}
				}
			}
			else if (depth < MAX_SCAN_DEPTH)
			{
				bool isPrimitive = TypeNameMatchesSearchType(typeNameStr);
				bool isString = typeNameStr == "string" || typeNameStr == "System.String";
				bool isArray = typeNameStr.ends_with("[]") || typeNameStr.find("<") != std::string::npos;

				bool isDelegate = typeNameStr.starts_with("System.") &&
				(typeNameStr.find("Action") != std::string::npos ||
					typeNameStr.find("Func") != std::string::npos ||
					typeNameStr.find("Predicate") != std::string::npos ||
					typeNameStr.find("EventHandler") != std::string::npos ||
					typeNameStr.find("Delegate") != std::string::npos ||
					typeNameStr.find("MulticastDelegate") != std::string::npos);

				if (!isPrimitive && !isString && !isArray && !isDelegate)
				{
					void* fieldClass = UR::Invoke<void*, void*>(
						mono ? "mono_class_from_mono_type" : "il2cpp_class_from_type", fieldType);
					if (fieldClass)
					{
						bool isValueType = UR::Invoke<bool, void*>(
							mono ? "mono_class_is_valuetype" : "il2cpp_class_is_valuetype", fieldClass);

						if (isValueType)
						{
							std::string childName = objName + "." + (fieldName ? fieldName : "unknown") + "(S)";
							ScanObjectInstance(static_cast<char*>(obj) + offset, fieldClass, out, visited, depth + 1,
							                   childName);
						}
						else
						{
							if (void* refObj = nullptr; Helper::SafeReadPointer(obj, offset, refObj) && refObj)
							{
								std::string childName = objName + "." + (fieldName ? fieldName : "unknown");
								ScanObjectInstance(refObj, fieldClass, out, visited, depth + 1, childName);
							}
						}
					}
				}
			}
		}

		currentClass = UR::Invoke<void*, void*>(
			mono ? "mono_class_get_parent" : "il2cpp_class_get_parent", currentClass);
	}
}

bool MemoryScanner::TypeNameMatchesSearchType(const std::string& typeName) const
{
	switch (selectedType)
	{
	case ScanValueType::Int:
		return typeName == "System.Int32" || typeName == "int" ||
			typeName == "System.UInt32" || typeName == "uint" ||
			typeName == "System.Int16" || typeName == "short" ||
			typeName == "System.UInt16" || typeName == "ushort" ||
			typeName == "System.Byte" || typeName == "byte" ||
			typeName == "System.SByte" || typeName == "sbyte";
	case ScanValueType::Long:
		return typeName == "System.Int64" || typeName == "long" ||
			typeName == "System.UInt64" || typeName == "ulong";
	case ScanValueType::Float:
		return typeName == "System.Single" || typeName == "float";
	case ScanValueType::Double:
		return typeName == "System.Double" || typeName == "double";
	case ScanValueType::Bool:
		return typeName == "System.Boolean" || typeName == "bool";
	}
	return false;
}

ScanValueType MemoryScanner::DetermineActualValueType(const std::string& typeName) const
{
	if (typeName == "System.Int32" || typeName == "int" ||
		typeName == "System.UInt32" || typeName == "uint" ||
		typeName == "System.Int16" || typeName == "short" ||
		typeName == "System.UInt16" || typeName == "ushort" ||
		typeName == "System.Byte" || typeName == "byte" ||
		typeName == "System.SByte" || typeName == "sbyte")
		return ScanValueType::Int;
	if (typeName == "System.Int64" || typeName == "long" ||
		typeName == "System.UInt64" || typeName == "ulong")
		return ScanValueType::Long;
	if (typeName == "System.Single" || typeName == "float")
		return ScanValueType::Float;
	if (typeName == "System.Double" || typeName == "double")
		return ScanValueType::Double;
	if (typeName == "System.Boolean" || typeName == "bool")
		return ScanValueType::Bool;
	return ScanValueType::Int;
}

bool MemoryScanner::ReadFieldValue(const ScanField& field, void* outValue) const
{
	if (field.isStatic)
	{
		return ReadStaticFieldValue(field.fieldHandle, outValue);
	}
	return ReadInstanceFieldValue(field.object, field.offset, field.valueType, outValue);
}

bool MemoryScanner::ReadStaticFieldValue(void* fieldHandle, void* outValue) const
{
	if (!fieldHandle || !outValue)
		return false;

	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	try
	{
		if (mono)
		{
			void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
			                                               UR::Invoke<void*, void*>(
				                                               "mono_field_get_parent", fieldHandle));
			UR::Invoke<void, void*, void*, void*>("mono_field_static_get_value", vTable, fieldHandle, outValue);
		}
		else
		{
			UR::Invoke<void, void*, void*>("il2cpp_field_static_get_value", fieldHandle, outValue);
		}
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::ReadInstanceFieldValue(void* obj, int offset, ScanValueType type, void* outValue) const
{
	if (!obj || offset < 0 || !outValue)
		return false;

	switch (type)
	{
	case ScanValueType::Int:
		{
			int32_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ScanValueType::Long:
		{
			int64_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = v;
			return true;
		}
	case ScanValueType::Float:
		{
			float v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<float*>(outValue) = v;
			return true;
		}
	case ScanValueType::Double:
		{
			double v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<double*>(outValue) = v;
			return true;
		}
	case ScanValueType::Bool:
		{
			bool v;
			if (!SafeReadBool(obj, offset, v))
				return false;
			*static_cast<bool*>(outValue) = v;
			return true;
		}
	}
	return false;
}

bool MemoryScanner::GetTargetValueAsInt64(int64_t& out) const
{
	try
	{
		out = typedValue.i64;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::GetTargetValueAsUInt64(uint64_t& out) const
{
	try
	{
		out = static_cast<uint64_t>(typedValue.i64);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::GetTargetValueAsFloat(float& out) const
{
	try
	{
		out = typedValue.f32;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::GetTargetValueAsDouble(double& out) const
{
	try
	{
		out = typedValue.f64;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::GetTargetValueAsBool(bool& out) const
{
	try
	{
		out = typedValue.b;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MemoryScanner::CompareValueWithTarget(const void* value, ScanValueType actualType) const
{
	if (comparison != ScanComparison::Exact)
		return true;

	switch (selectedType)
	{
	case ScanValueType::Int:
		{
			int64_t target;
			if (!GetTargetValueAsInt64(target))
				return false;
			if (actualType == ScanValueType::Int)
				return *static_cast<const int64_t*>(value) == target;
			if (actualType == ScanValueType::Long)
				return *static_cast<const int64_t*>(value) == target;
			return false;
		}
	case ScanValueType::Long:
		{
			int64_t target;
			if (!GetTargetValueAsInt64(target))
				return false;
			if (actualType == ScanValueType::Int)
				return *static_cast<const int64_t*>(value) == target;
			if (actualType == ScanValueType::Long)
				return *static_cast<const int64_t*>(value) == target;
			return false;
		}
	case ScanValueType::Float:
		{
			float target;
			if (!GetTargetValueAsFloat(target))
				return false;
			if (actualType == ScanValueType::Float)
				return *static_cast<const float*>(value) == target;
			return false;
		}
	case ScanValueType::Double:
		{
			double target;
			if (!GetTargetValueAsDouble(target))
				return false;
			if (actualType == ScanValueType::Double)
				return *static_cast<const double*>(value) == target;
			return false;
		}
	case ScanValueType::Bool:
		{
			bool target;
			if (!GetTargetValueAsBool(target))
				return false;
			if (actualType == ScanValueType::Bool)
				return *static_cast<const bool*>(value) == target;
			return false;
		}
	}
	return false;
}

int MemoryScanner::CompareValueWithPrevious(const void* currentValue, const ScanField& field) const
{
	switch (field.valueType)
	{
	case ScanValueType::Int:
	case ScanValueType::Long:
		{
			int64_t cur = *static_cast<const int64_t*>(currentValue);
			int64_t prev = field.lastValue.i64;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ScanValueType::Float:
		{
			float cur = *static_cast<const float*>(currentValue);
			float prev = field.lastValue.f32;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ScanValueType::Double:
		{
			double cur = *static_cast<const double*>(currentValue);
			double prev = field.lastValue.f64;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ScanValueType::Bool:
		{
			int cur = *static_cast<const bool*>(currentValue) ? 1 : 0;
			int prev = field.lastValue.b ? 1 : 0;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	}
	return 0;
}

void MemoryScanner::OpenResultInInspector(const ScanField& result) const
{
	if (!result.object && !result.isStatic)
		return;

	auto* inspector = Inspector::GetInstance();
	if (!inspector)
		return;

	if (result.isStatic)
	{
		inspector->InspectInstance(nullptr, result.classHandle, result.className + "::" + result.fieldName);
	}
	else
	{
		inspector->InspectInstance(result.object, result.classHandle, result.objectName);
	}
}

const char* MemoryScanner::GetValueTypeName(ScanValueType type)
{
	switch (type)
	{
	case ScanValueType::Int: return "int";
	case ScanValueType::Long: return "long";
	case ScanValueType::Float: return "float";
	case ScanValueType::Double: return "double";
	case ScanValueType::Bool: return "bool";
	}
	return "unknown";
}

const char* MemoryScanner::GetComparisonName(ScanComparison comp)
{
	switch (comp)
	{
	case ScanComparison::Exact: return "Exact";
	case ScanComparison::Increased: return "Increased";
	case ScanComparison::Decreased: return "Decreased";
	case ScanComparison::Changed: return "Changed";
	case ScanComparison::Unchanged: return "Unchanged";
	}
	return "unknown";
}

std::string MemoryScanner::FormatFieldValue(const ScanField& field)
{
	char buf[64];
	switch (field.valueType)
	{
	case ScanValueType::Int:
	case ScanValueType::Long:
		return std::to_string(field.lastValue.i64);
	case ScanValueType::Float:
		snprintf(buf, sizeof(buf), "%.3f", field.lastValue.f32);
		return buf;
	case ScanValueType::Double:
		snprintf(buf, sizeof(buf), "%.6f", field.lastValue.f64);
		return buf;
	case ScanValueType::Bool:
		return field.lastValue.b ? "true" : "false";
	}
	return "?";
}
