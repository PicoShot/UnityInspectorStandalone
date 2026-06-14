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

	template <typename T>
	bool SafeWrite(void* ptr, int offset, const T& value)
	{
		if (!ptr || offset < 0) return false;
		__try
		{
			*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset) = value;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}
}

MemoryScanner::~MemoryScanner()
{
	stopRequested = true;
	if (scanThread.joinable())
		scanThread.join();
}

void MemoryScanner::Update(float deltaTime)
{
	(void)deltaTime;

	if (pendingOperation != ScanOperation::None && !scanInProgress)
	{
		if (pendingOperation == ScanOperation::Reset)
		{
			ResetScan();
			pendingOperation = ScanOperation::None;
		}
		else
		{
			scanInProgress = true;
			stopRequested = false;

			if (scanThread.joinable())
				scanThread.join();

			const ScanOperation op = pendingOperation;
			pendingOperation = ScanOperation::None;

			if (op == ScanOperation::FirstScan)
			{
				statusText = "Gathering Unity objects...";
				objectsGathered = GatherUnityObjects();
			}

			scanThread = std::thread([this, op]
			{
				try
				{
					UR::ThreadAttach();

					if (op == ScanOperation::FirstScan)
					{
						PerformFirstScan();
					}
					else if (op == ScanOperation::NextScan)
					{
						PerformNextScan();
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
			});
		}
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

		const char* typeNames[] = {"Int (8/16/32-bit)", "Long (64-bit)", "Float", "Double", "Bool"};
		const char* compNames[] = {"Exact Match", "Increased Value", "Decreased Value", "Changed Value", "Unchanged Value"};

		auto statusColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		if (scanInProgress) statusColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
		else if (!statusText.empty() && statusText.find("error") != std::string::npos)
			statusColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

		if (ImGui::BeginTable("##TopSettingsTable", 2, ImGuiTableFlags_None))
		{
			ImGui::TableSetupColumn("LeftCol", ImGuiTableColumnFlags_WidthStretch, 0.55f);
			ImGui::TableSetupColumn("RightCol", ImGuiTableColumnFlags_WidthStretch, 0.45f);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::TextDisabled("SCAN CONFIGURATION");
			ImGui::Separator();
			ImGui::Spacing();

			float comboWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

			ImGui::SetNextItemWidth(comboWidth);
			int typeIndex = static_cast<int>(selectedType);
			if (locked) ImGui::BeginDisabled();
			if (ImGui::Combo("##Type", &typeIndex, typeNames, IM_ARRAYSIZE(typeNames)))
			{
				selectedType = static_cast<ScanValueType>(typeIndex);
				SyncValueToBuffer();
			}
			if (locked) ImGui::EndDisabled();

			ImGui::SameLine();
			ImGui::SetNextItemWidth(comboWidth);
			int compIndex = static_cast<int>(comparison);
			if (ImGui::Combo("##Comp", &compIndex, compNames, IM_ARRAYSIZE(compNames)))
			{
				comparison = static_cast<ScanComparison>(compIndex);
			}

			ImGui::Spacing();
			ImGui::Text("Scan Value:");
			ImGui::SetNextItemWidth(-1.0f);
			RenderValueInput();

			ImGui::TableNextColumn();
			ImGui::TextDisabled("FILTERS & ACTIONS");
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Checkbox("Include System/Unity Classes", &includeSystemNamespaces);
			ImGui::Checkbox("Console Debug Logging", &debugLogging);

			ImGui::Spacing();

			if (scanInProgress)
			{
				ImGui::TextColored(statusColor, "Scanning... %s", statusText.c_str());
			}
			else
			{
				if (!hasDoneFirstScan)
				{
					if (ImGui::Button("First Scan", ImVec2(120, 30)))
					{
						pendingOperation = ScanOperation::FirstScan;
					}
				}
				else
				{
					if (ImGui::Button("Next Scan", ImVec2(120, 30)))
					{
						pendingOperation = ScanOperation::NextScan;
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset", ImVec2(120, 30)))
					{
						pendingOperation = ScanOperation::Reset;
					}
				}
			}

			ImGui::EndTable();
		}

		ImGui::Separator();
		ImGui::Spacing();

		std::unique_lock resultsLock(resultsMutex);
		ImGui::TextDisabled("Matches Found: %zu / %zu", currentResults.size(), MAX_RESULTS);
		resultsLock.unlock();

		if (!statusText.empty() && !scanInProgress)
		{
			ImGui::SameLine();
			ImGui::TextColored(statusColor, "|  %s", statusText.c_str());
		}

		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputTextWithHint("##Filter", "Filter results by class, field, namespace, or value...", resultFilterBuffer, sizeof(resultFilterBuffer),
								 ImGuiInputTextFlags_EscapeClearsAll);
		ImGui::Spacing();

		if (ImGui::BeginTable("Results", 7,
		                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
		                      ImGuiTableFlags_Sortable, ImVec2(0, -120)))
		{
			ImGui::TableSetupColumn("Live Value", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableSetupColumn("Previous", ImGuiTableColumnFlags_WidthStretch, 0.12f);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch, 0.1f);
			ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_WidthStretch, 0.18f);
			ImGui::TableSetupColumn("Namespace", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableSetupColumn("Object", ImGuiTableColumnFlags_WidthStretch, 0.15f);
			ImGui::TableHeadersRow();

			std::string filterLower = resultFilterBuffer;
			std::ranges::transform(filterLower, filterLower.begin(), tolower);

			for (size_t i = 0; i < currentResults.size(); ++i)
			{
				const auto& result = currentResults[i];

				char liveValueBytes[sizeof(double)] = {};
				bool readSuccess = ReadFieldValue(result, liveValueBytes);
				std::string liveValStr = "??";
				bool isChanged = false;
				if (readSuccess)
				{
					ScanField::ValUnion liveUnion;
					memcpy(&liveUnion, liveValueBytes, sizeof(double));
					liveValStr = FormatValue(liveUnion, result.actualType);
					isChanged = (CompareValueWithPrevious(liveValueBytes, result) != 0);
				}

				if (!filterLower.empty())
				{
					if (!Helper::CaseInsensitiveFind(result.className, filterLower) &&
						!Helper::CaseInsensitiveFind(result.namespaze, filterLower) &&
						!Helper::CaseInsensitiveFind(result.fieldName, filterLower) &&
						!Helper::CaseInsensitiveFind(result.objectName, filterLower) &&
						!Helper::CaseInsensitiveFind(liveValStr, filterLower))
					{
						continue;
					}
				}

				ImGui::TableNextRow();
				ImGui::PushID(static_cast<int>(i));

				ImGui::TableNextColumn();
				bool isSelected = (selectedResultIndex == static_cast<int>(i));

				if (isChanged)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
				}
				if (ImGui::Selectable(liveValStr.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedResultIndex = static_cast<int>(i);
					if (readSuccess)
					{
						memcpy(&editValue, liveValueBytes, sizeof(double));
					}
					else
					{
						memset(&editValue, 0, sizeof(double));
					}
				}
				if (isChanged)
				{
					ImGui::PopStyleColor();
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
				if (result.hasPrevValue)
				{
					ImGui::Text("%s", FormatValue(result.prevValue, result.actualType).c_str());
				}
				else
				{
					ImGui::TextDisabled("-");
				}

				ImGui::TableNextColumn();
				ImGui::Text("%s", GetActualFieldTypeName(result.actualType));

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.className.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.namespaze.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", result.fieldName.c_str());

				ImGui::TableNextColumn();
				if (result.isStatic)
					ImGui::TextDisabled("(static)");
				else
					ImGui::Text("%s (%p)", result.objectName.c_str(), result.object);

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		if (selectedResultIndex >= 0 && selectedResultIndex < static_cast<int>(currentResults.size()))
		{
			ImGui::Separator();
			auto& result = currentResults[selectedResultIndex];
			ImGui::Text("Selected: %s::%s (%s)", result.className.c_str(), result.fieldName.c_str(), GetActualFieldTypeName(result.actualType));
			if (result.isStatic)
				ImGui::TextDisabled("Location: Static Field");
			else
				ImGui::TextDisabled("Location: Object %s (%p), Offset: 0x%X", result.objectName.c_str(), result.object, result.offset);

			ImGui::SetNextItemWidth(300.0f);
			
			bool valueChanged = false;
			if (result.actualType == ActualFieldType::Bool)
			{
				valueChanged = ImGui::Checkbox("New Value", &editValue.b);
			}
			else if (result.actualType == ActualFieldType::Float)
			{
				valueChanged = ImGui::InputFloat("New Value", &editValue.f32, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			}
			else if (result.actualType == ActualFieldType::Double)
			{
				valueChanged = ImGui::InputDouble("New Value", &editValue.f64, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
			}
			else if (result.actualType == ActualFieldType::ULong)
			{
				valueChanged = ImGui::InputScalar("New Value", ImGuiDataType_U64, &editValue.u64, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
			}
			else
			{
				valueChanged = ImGui::InputScalar("New Value", ImGuiDataType_S64, &editValue.i64, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
			}

			ImGui::SameLine();
			if (ImGui::Button("Write Value", ImVec2(100, 0)) || valueChanged)
			{
				if (WriteFieldValue(result, &editValue))
					memcpy(&currentResults[selectedResultIndex].lastValue, &editValue, sizeof(double));
			}
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
				snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%lld", static_cast<long long>(val));
			}
			break;
		}
	case ScanValueType::Long:
		{
			int64_t val = typedValue.i64;
			if (ImGui::InputScalar("##ValueLong", ImGuiDataType_S64, &val))
			{
				typedValue.i64 = val;
				snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%lld", static_cast<long long>(val));
			}
			break;
		}
	case ScanValueType::Float:
		{
			float val = typedValue.f32;
			if (ImGui::InputFloat("##ValueFloat", &val, 0.0f, 0.0f, "%.3f"))
			{
				typedValue.f32 = val;
				snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%.3f", val);
			}
			break;
		}
	case ScanValueType::Double:
		{
			double val = typedValue.f64;
			if (ImGui::InputDouble("##ValueDouble", &val, 0.0, 0.0, "%.6f"))
			{
				typedValue.f64 = val;
				snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%.6f", val);
			}
			break;
		}
	case ScanValueType::Bool:
		{
			bool val = typedValue.b;
			if (ImGui::Checkbox("##ValueBool", &val))
			{
				typedValue.b = val;
				snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%s", val ? "true" : "false");
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
		snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%lld", static_cast<long long>(typedValue.i64));
		break;
	case ScanValueType::Float:
		snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%.3f", typedValue.f32);
		break;
	case ScanValueType::Double:
		snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%.6f", typedValue.f64);
		break;
	case ScanValueType::Bool:
		snprintf(m_valueBuffer, sizeof(m_valueBuffer), "%s", typedValue.b ? "true" : "false");
		break;
	}
}

void MemoryScanner::PerformFirstScan()
{
	std::vector<ScanField> tempResults;

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
		printf("[MemoryScanner] First scan: %s = %s\n", GetValueTypeName(selectedType), m_valueBuffer);

	statusText = "Scanning static fields...";
	ScanStaticFields(tempResults);
	if (debugLogging)
		printf("[MemoryScanner] Static matches: %zu\n", tempResults.size());

	if (!stopRequested && tempResults.size() < MAX_RESULTS)
	{
		statusText = "Scanning objects...";
		ScanUnityObjectFields(tempResults);
	}

	if (!stopRequested)
	{
		std::scoped_lock lock(resultsMutex);
		currentResults = std::move(tempResults);
		statusText = "Scan complete. " + std::to_string(currentResults.size()) + " matches.";
		if (debugLogging)
			printf("[MemoryScanner] Total matches: %zu\n", currentResults.size());
		hasDoneFirstScan = true;
	}
	scanInProgress = false;
	selectedResultIndex = -1;
}

void MemoryScanner::PerformNextScan()
{
	std::vector<ScanField> localResults;
	{
		std::scoped_lock lock(resultsMutex);
		localResults = currentResults;
	}

	std::vector<ScanField> newResults;
	newResults.reserve(localResults.size());

	for (auto& field : localResults)
	{
		if (stopRequested)
			break;

		char currentValue[sizeof(double)] = {};
		if (!ReadFieldValue(field, currentValue))
			continue;

		bool match = false;

		switch (comparison)
		{
		case ScanComparison::Exact:
			match = CompareValueWithTarget(currentValue, field.actualType);
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
			updated.prevValue = field.lastValue;
			updated.hasPrevValue = true;
			memcpy(&updated.lastValue, currentValue, sizeof(double));
			newResults.push_back(updated);
		}
	}

	if (!stopRequested)
	{
		std::scoped_lock lock(resultsMutex);
		currentResults = std::move(newResults);
		statusText = "Next scan complete. " + std::to_string(currentResults.size()) + " matches.";
	}
	scanInProgress = false;
	selectedResultIndex = -1;
}

void MemoryScanner::ResetScan()
{
	std::scoped_lock lock(resultsMutex);
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
		if (stopRequested)
			return;
		if (!assembly)
			continue;

		for (const auto& klass : assembly->classes)
		{
			if (stopRequested)
				return;
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
				if (stopRequested)
					return;
				if (out.size() >= MAX_RESULTS)
					return;
				if (!field || !field->static_field || !field->type)
					continue;

				std::string typeName = field->type->name;
				if (!TypeNameMatchesSearchType(typeName))
					continue;

				ActualFieldType actualType = DetermineActualFieldType(typeName);
				char rawBytes[8] = {};
				if (!ReadStaticFieldValue(field->address, rawBytes))
					continue;

				char value[8] = {};
				switch (actualType)
				{
				case ActualFieldType::Byte:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<uint8_t*>(rawBytes));
					break;
				case ActualFieldType::SByte:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<int8_t*>(rawBytes));
					break;
				case ActualFieldType::Short:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<int16_t*>(rawBytes));
					break;
				case ActualFieldType::UShort:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<uint16_t*>(rawBytes));
					break;
				case ActualFieldType::Int:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<int32_t*>(rawBytes));
					break;
				case ActualFieldType::UInt:
					*reinterpret_cast<int64_t*>(value) = static_cast<int64_t>(*reinterpret_cast<uint32_t*>(rawBytes));
					break;
				case ActualFieldType::Long:
					*reinterpret_cast<int64_t*>(value) = *reinterpret_cast<int64_t*>(rawBytes);
					break;
				case ActualFieldType::ULong:
					*reinterpret_cast<uint64_t*>(value) = *reinterpret_cast<uint64_t*>(rawBytes);
					break;
				case ActualFieldType::Float:
					*reinterpret_cast<float*>(value) = *reinterpret_cast<float*>(rawBytes);
					break;
				case ActualFieldType::Double:
					*reinterpret_cast<double*>(value) = *reinterpret_cast<double*>(rawBytes);
					break;
				case ActualFieldType::Bool:
					*reinterpret_cast<bool*>(value) = *reinterpret_cast<bool*>(rawBytes);
					break;
				}

				if (!CompareValueWithTarget(value, actualType))
					continue;

				ScanField scanField;
				scanField.fieldHandle = field->address;
				scanField.classHandle = klass->address;
				scanField.object = nullptr;
				scanField.isStatic = true;
				scanField.valueType = selectedType;
				scanField.actualType = actualType;
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

	if (objectsGathered.empty())
	{
		if (debugLogging)
			printf("[MemoryScanner] No Unity objects found to scan\n");
		return;
	}

	if (debugLogging)
		printf("[MemoryScanner] Scanning %zu Unity objects...\n", objectsGathered.size());

	std::unordered_set<VisitedKey, VisitedKeyHash> visited;
	int processed = 0;

	for (void* obj : objectsGathered)
	{
		if (stopRequested)
			break;
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
			statusText = "Scanning objects... " + std::to_string(processed) + "/" + std::to_string(objectsGathered.size());
		}
	}

	objectsGathered.clear();

	if (debugLogging)
		printf("[MemoryScanner] Scanned %d objects, found %zu matches\n", processed, out.size());
}

void MemoryScanner::ScanObjectInstance(void* obj, void* klass, std::vector<ScanField>& out,
                                       std::unordered_set<VisitedKey, VisitedKeyHash>& visited, int depth,
                                       const std::string& objName)
{
	if (stopRequested)
		return;
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
		if (stopRequested)
			return;

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
			if (stopRequested)
				return;
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
				ActualFieldType actualType = DetermineActualFieldType(typeNameStr);
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
						scanField.valueType = selectedType;
						scanField.actualType = actualType;
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

ActualFieldType MemoryScanner::DetermineActualFieldType(const std::string& typeName) const
{
	if (typeName == "System.Byte" || typeName == "byte") return ActualFieldType::Byte;
	if (typeName == "System.SByte" || typeName == "sbyte") return ActualFieldType::SByte;
	if (typeName == "System.Int16" || typeName == "short") return ActualFieldType::Short;
	if (typeName == "System.UInt16" || typeName == "ushort") return ActualFieldType::UShort;
	if (typeName == "System.Int32" || typeName == "int") return ActualFieldType::Int;
	if (typeName == "System.UInt32" || typeName == "uint") return ActualFieldType::UInt;
	if (typeName == "System.Int64" || typeName == "long") return ActualFieldType::Long;
	if (typeName == "System.UInt64" || typeName == "ulong") return ActualFieldType::ULong;
	if (typeName == "System.Single" || typeName == "float") return ActualFieldType::Float;
	if (typeName == "System.Double" || typeName == "double") return ActualFieldType::Double;
	if (typeName == "System.Boolean" || typeName == "bool") return ActualFieldType::Bool;
	return ActualFieldType::Int;
}

bool MemoryScanner::ReadFieldValue(const ScanField& field, void* outValue) const
{
	if (field.isStatic)
	{
		char rawBytes[8] = {};
		if (!ReadStaticFieldValue(field.fieldHandle, rawBytes))
			return false;

		switch (field.actualType)
		{
		case ActualFieldType::Byte:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<uint8_t*>(rawBytes));
			return true;
		case ActualFieldType::SByte:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<int8_t*>(rawBytes));
			return true;
		case ActualFieldType::Short:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<int16_t*>(rawBytes));
			return true;
		case ActualFieldType::UShort:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<uint16_t*>(rawBytes));
			return true;
		case ActualFieldType::Int:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<int32_t*>(rawBytes));
			return true;
		case ActualFieldType::UInt:
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(*reinterpret_cast<uint32_t*>(rawBytes));
			return true;
		case ActualFieldType::Long:
			*static_cast<int64_t*>(outValue) = *reinterpret_cast<int64_t*>(rawBytes);
			return true;
		case ActualFieldType::ULong:
			*static_cast<uint64_t*>(outValue) = *reinterpret_cast<uint64_t*>(rawBytes);
			return true;
		case ActualFieldType::Float:
			*static_cast<float*>(outValue) = *reinterpret_cast<float*>(rawBytes);
			return true;
		case ActualFieldType::Double:
			*static_cast<double*>(outValue) = *reinterpret_cast<double*>(rawBytes);
			return true;
		case ActualFieldType::Bool:
			*static_cast<bool*>(outValue) = *reinterpret_cast<bool*>(rawBytes);
			return true;
		}
		return false;
	}
	return ReadInstanceFieldValue(field.object, field.offset, field.actualType, outValue);
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

bool MemoryScanner::ReadInstanceFieldValue(void* obj, int offset, ActualFieldType type, void* outValue) const
{
	if (!obj || offset < 0 || !outValue)
		return false;

	switch (type)
	{
	case ActualFieldType::Byte:
		{
			uint8_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::SByte:
		{
			int8_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::Short:
		{
			int16_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::UShort:
		{
			uint16_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::Int:
		{
			int32_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::UInt:
		{
			uint32_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = static_cast<int64_t>(v);
			return true;
		}
	case ActualFieldType::Long:
		{
			int64_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<int64_t*>(outValue) = v;
			return true;
		}
	case ActualFieldType::ULong:
		{
			uint64_t v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<uint64_t*>(outValue) = v;
			return true;
		}
	case ActualFieldType::Float:
		{
			float v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<float*>(outValue) = v;
			return true;
		}
	case ActualFieldType::Double:
		{
			double v;
			if (!SafeRead(obj, offset, v))
				return false;
			*static_cast<double*>(outValue) = v;
			return true;
		}
	case ActualFieldType::Bool:
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

bool MemoryScanner::CompareValueWithTarget(const void* value, ActualFieldType actualType) const
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
			if (actualType == ActualFieldType::Byte ||
				actualType == ActualFieldType::SByte ||
				actualType == ActualFieldType::Short ||
				actualType == ActualFieldType::UShort ||
				actualType == ActualFieldType::Int ||
				actualType == ActualFieldType::UInt ||
				actualType == ActualFieldType::Long)
			{
				return *static_cast<const int64_t*>(value) == target;
			}
			if (actualType == ActualFieldType::ULong)
			{
				return static_cast<int64_t>(*static_cast<const uint64_t*>(value)) == target;
			}
			return false;
		}
	case ScanValueType::Long:
		{
			int64_t target;
			if (!GetTargetValueAsInt64(target))
				return false;
			if (actualType == ActualFieldType::Byte ||
				actualType == ActualFieldType::SByte ||
				actualType == ActualFieldType::Short ||
				actualType == ActualFieldType::UShort ||
				actualType == ActualFieldType::Int ||
				actualType == ActualFieldType::UInt ||
				actualType == ActualFieldType::Long)
			{
				return *static_cast<const int64_t*>(value) == target;
			}
			if (actualType == ActualFieldType::ULong)
			{
				return static_cast<int64_t>(*static_cast<const uint64_t*>(value)) == target;
			}
			return false;
		}
	case ScanValueType::Float:
		{
			float target;
			if (!GetTargetValueAsFloat(target))
				return false;
			if (actualType == ActualFieldType::Float)
				return *static_cast<const float*>(value) == target;
			return false;
		}
	case ScanValueType::Double:
		{
			double target;
			if (!GetTargetValueAsDouble(target))
				return false;
			if (actualType == ActualFieldType::Double)
				return *static_cast<const double*>(value) == target;
			return false;
		}
	case ScanValueType::Bool:
		{
			bool target;
			if (!GetTargetValueAsBool(target))
				return false;
			if (actualType == ActualFieldType::Bool)
				return *static_cast<const bool*>(value) == target;
			return false;
		}
	}
	return false;
}

int MemoryScanner::CompareValueWithPrevious(const void* currentValue, const ScanField& field) const
{
	switch (field.actualType)
	{
	case ActualFieldType::Byte:
	case ActualFieldType::SByte:
	case ActualFieldType::Short:
	case ActualFieldType::UShort:
	case ActualFieldType::Int:
	case ActualFieldType::UInt:
	case ActualFieldType::Long:
		{
			int64_t cur = *static_cast<const int64_t*>(currentValue);
			int64_t prev = field.lastValue.i64;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ActualFieldType::ULong:
		{
			uint64_t cur = *static_cast<const uint64_t*>(currentValue);
			uint64_t prev = field.lastValue.u64;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ActualFieldType::Float:
		{
			float cur = *static_cast<const float*>(currentValue);
			float prev = field.lastValue.f32;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ActualFieldType::Double:
		{
			double cur = *static_cast<const double*>(currentValue);
			double prev = field.lastValue.f64;
			if (cur > prev) return 1;
			if (cur < prev) return -1;
			return 0;
		}
	case ActualFieldType::Bool:
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

std::vector<void*> MemoryScanner::GatherUnityObjects() const
{
	std::vector<void*> allObjects;
	auto coreAssembly = UR::Get("UnityEngine.CoreModule.dll");
	if (!coreAssembly)
		return allObjects;

	auto objectClass = coreAssembly->Get("Object", "UnityEngine");
	if (!objectClass)
		return allObjects;

	try
	{
		allObjects = objectClass->FindObjectsByType<void*>(1, 0);
	}
	catch (...) {}

	if (allObjects.empty())
	{
		try
		{
			allObjects = objectClass->FindObjectsByType<void*>();
		}
		catch (...) {}
	}

	if (allObjects.empty())
	{
		try
		{
			allObjects = objectClass->FindObjectsOfType<void*>();
		}
		catch (...) {}
	}

	if (allObjects.size() > MAX_OBJECTS_TO_SCAN)
		allObjects.resize(MAX_OBJECTS_TO_SCAN);

	return allObjects;
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

const char* MemoryScanner::GetActualFieldTypeName(ActualFieldType type)
{
	switch (type)
	{
	case ActualFieldType::Byte: return "byte";
	case ActualFieldType::SByte: return "sbyte";
	case ActualFieldType::Short: return "short";
	case ActualFieldType::UShort: return "ushort";
	case ActualFieldType::Int: return "int";
	case ActualFieldType::UInt: return "uint";
	case ActualFieldType::Long: return "long";
	case ActualFieldType::ULong: return "ulong";
	case ActualFieldType::Float: return "float";
	case ActualFieldType::Double: return "double";
	case ActualFieldType::Bool: return "bool";
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
	return FormatValue(field.lastValue, field.actualType);
}

std::string MemoryScanner::FormatValue(const ScanField::ValUnion& val, ActualFieldType type)
{
	char buf[64];
	switch (type)
	{
	case ActualFieldType::Byte:
	case ActualFieldType::SByte:
	case ActualFieldType::Short:
	case ActualFieldType::UShort:
	case ActualFieldType::Int:
	case ActualFieldType::UInt:
	case ActualFieldType::Long:
		return std::to_string(val.i64);
	case ActualFieldType::ULong:
		return std::to_string(val.u64);
	case ActualFieldType::Float:
		snprintf(buf, sizeof(buf), "%.3f", val.f32);
		return buf;
	case ActualFieldType::Double:
		snprintf(buf, sizeof(buf), "%.6f", val.f64);
		return buf;
	case ActualFieldType::Bool:
		return val.b ? "true" : "false";
	}
	return "?";
}

bool MemoryScanner::WriteFieldValue(const ScanField& field, const void* valueBuffer) const
{
	if (field.isStatic)
	{
		if (!field.fieldHandle) return false;
		const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;
		try
		{
			char rawBytes[8] = {};
			switch (field.actualType)
			{
			case ActualFieldType::Byte:
				*reinterpret_cast<uint8_t*>(rawBytes) = static_cast<uint8_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::SByte:
				*reinterpret_cast<int8_t*>(rawBytes) = static_cast<int8_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::Short:
				*reinterpret_cast<int16_t*>(rawBytes) = static_cast<int16_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::UShort:
				*reinterpret_cast<uint16_t*>(rawBytes) = static_cast<uint16_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::Int:
				*reinterpret_cast<int32_t*>(rawBytes) = static_cast<int32_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::UInt:
				*reinterpret_cast<uint32_t*>(rawBytes) = static_cast<uint32_t>(*static_cast<const int64_t*>(valueBuffer));
				break;
			case ActualFieldType::Long:
				*reinterpret_cast<int64_t*>(rawBytes) = *static_cast<const int64_t*>(valueBuffer);
				break;
			case ActualFieldType::ULong:
				*reinterpret_cast<uint64_t*>(rawBytes) = *static_cast<const uint64_t*>(valueBuffer);
				break;
			case ActualFieldType::Float:
				*reinterpret_cast<float*>(rawBytes) = *static_cast<const float*>(valueBuffer);
				break;
			case ActualFieldType::Double:
				*reinterpret_cast<double*>(rawBytes) = *static_cast<const double*>(valueBuffer);
				break;
			case ActualFieldType::Bool:
				*reinterpret_cast<bool*>(rawBytes) = *static_cast<const bool*>(valueBuffer);
				break;
			}

			if (mono)
			{
				void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
				                                               UR::Invoke<void*, void*>(
					                                               "mono_field_get_parent", field.fieldHandle));
				UR::Invoke<void, void*, void*, const void*>("mono_field_static_set_value", vTable, field.fieldHandle, rawBytes);
			}
			else
			{
				UR::Invoke<void, void*, const void*>("il2cpp_field_static_set_value", field.fieldHandle, rawBytes);
			}
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	if (!field.object || field.offset < 0) return false;
	switch (field.actualType)
	{
	case ActualFieldType::Byte:
		return SafeWrite(field.object, field.offset, static_cast<uint8_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::SByte:
		return SafeWrite(field.object, field.offset, static_cast<int8_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::Short:
		return SafeWrite(field.object, field.offset, static_cast<int16_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::UShort:
		return SafeWrite(field.object, field.offset, static_cast<uint16_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::Int:
		return SafeWrite(field.object, field.offset, static_cast<int32_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::UInt:
		return SafeWrite(field.object, field.offset, static_cast<uint32_t>(*static_cast<const int64_t*>(valueBuffer)));
	case ActualFieldType::Long:
		return SafeWrite(field.object, field.offset, *static_cast<const int64_t*>(valueBuffer));
	case ActualFieldType::ULong:
		return SafeWrite(field.object, field.offset, *static_cast<const uint64_t*>(valueBuffer));
	case ActualFieldType::Float:
		return SafeWrite(field.object, field.offset, *static_cast<const float*>(valueBuffer));
	case ActualFieldType::Double:
		return SafeWrite(field.object, field.offset, *static_cast<const double*>(valueBuffer));
	case ActualFieldType::Bool:
		return SafeWrite(field.object, field.offset, *static_cast<const bool*>(valueBuffer));
	}
	return false;
}
