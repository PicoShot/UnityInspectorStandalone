#include "pch.h"
#include "field_editor.h"
#include "helper/helper.h"

#define API(fn) (Config::state.unityMode == UnityResolve::Mode::Mono ? "mono_" fn : "il2cpp_" fn)

bool IsEnumClass(std::string_view typeName)
{
	for (const auto& assembly : UR::assembly)
	{
		if (!assembly) continue;
		for (const auto& klass : assembly->classes)
		{
			if (!klass) continue;
			if (klass->m_name == typeName)
			{
				if (void* klassPtr = klass->address; UR::Invoke<bool, void*>(API("class_is_enum"), klassPtr))
					return true;
			}
		}
	}
	return false;
}

std::vector<std::pair<std::string, int>> GetEnumValues(std::string_view enumTypeName)
{
	std::vector<std::pair<std::string, int>> result;

	void* enumClass = nullptr;
	for (const auto& assembly : UR::assembly)
	{
		if (!assembly) continue;
		for (const auto& klass : assembly->classes)
		{
			if (!klass) continue;
			if (klass->m_name == enumTypeName)
			{
				enumClass = klass->address;
				break;
			}
		}
		if (enumClass) break;
	}

	if (!enumClass) return result;

	void* iter = nullptr;
	void* field;
	while ((field = UR::Invoke<void*, void*, void*>(API("class_get_fields"), enumClass, &iter)))
	{
		if (const int flags = UR::Invoke<int, void*>(API("field_get_flags"), field); (flags & 0x10) != 0)
		{
			if (const char* fieldName = UR::Invoke<const char*, void*>(API("field_get_name"), field))
			{
				int value = 0;
				if (Config::state.unityMode == UnityResolve::Mode::Mono)
				{
					void* vTable = UR::Invoke<void*, void*, void*>("mono_class_vtable", UR::pDomain,
					                                               UR::Invoke<void*, void*>("mono_field_get_parent", field));
					UR::Invoke<void, void*, void*, int*>("mono_field_static_get_value", vTable, field, &value);
				}
				else
				{
					UR::Invoke<void, void*, int*>("il2cpp_field_static_get_value", field, &value);
				}
				result.push_back({fieldName, value});
			}
		}
	}

	return result;
}

static void CheckAndUpdateEnumType(std::string& typeName, std::string_view fieldTypeName,
                                   std::string* enumTypeNameOut)
{
	if (enumTypeNameOut) enumTypeNameOut->clear();

	const size_t lastDot = typeName.rfind('.');

	if (const std::string shortName = (lastDot != std::string::npos) ? typeName.substr(lastDot + 1) : typeName;
		IsEnumClass(shortName))
	{
		typeName = "Enum";
		if (enumTypeNameOut) *enumTypeNameOut = shortName;
	}
	else if (IsEnumClass(fieldTypeName))
	{
		typeName = "Enum";
		if (enumTypeNameOut) *enumTypeNameOut = fieldTypeName;
	}
}

bool IsUInt64WrappingType(std::string_view typeName)
{
	return Helper::CaseInsensitiveFind(typeName, "gametimestamp");
}

EditableType DetermineEditableType(std::string_view typeName, std::string* enumTypeNameOut)
{
	std::string effectiveTypeName(typeName);
	CheckAndUpdateEnumType(effectiveTypeName, typeName, enumTypeNameOut);

	if (effectiveTypeName == "Enum") return EditableType::Enum;

	if (typeName == "System.Int16" || typeName == "System.Int32" || typeName == "System.Int64" ||
		typeName == "System.UInt16" || typeName == "System.UInt32" || typeName == "System.UInt64" ||
		typeName == "System.Byte" || typeName == "System.SByte" || typeName == "System.Char" ||
		typeName == "System.Short" || typeName == "System.UShort" ||
		typeName == "System.Long" || typeName == "System.ULong" ||
		typeName == "System.IntPtr" || typeName == "System.UIntPtr" ||
		IsUInt64WrappingType(typeName))
		return EditableType::Int;
	if (typeName == "System.Single")
		return EditableType::Float;
	if (typeName == "System.Double")
		return EditableType::Double;
	if (typeName == "System.Decimal")
		return EditableType::Decimal;
	if (typeName == "System.Boolean")
		return EditableType::Bool;
	if (typeName == "System.String")
		return EditableType::String;
	if (typeName == "UnityEngine.Vector2")
		return EditableType::Vector2;
	if (typeName == "UnityEngine.Vector3")
		return EditableType::Vector3;
	if (typeName == "UnityEngine.Vector4")
		return EditableType::Vector4;
	if (typeName == "UnityEngine.Quaternion")
		return EditableType::Quaternion;
	if (typeName == "UnityEngine.Color")
		return EditableType::Color;

	return EditableType::CustomObject;
}

FieldEditor::FieldEditor() = default;
FieldEditor::~FieldEditor() = default;

void FieldEditor::OpenFieldEditor(UR::Field* field, void* instance, std::string_view title)
{
	if (!field) return;

	state.targetField = field;
	state.targetInstance = instance;
	state.windowTitle = title;
	state.showWindow = true;

	state.nestedClass = nullptr;
	state.nestedInstance = nullptr;
	state.isValueType = false;
	state.ownedField.reset();

	if (field->type)
	{
		if (const std::string_view typeName = field->type->name; IsPointerType(typeName))
		{
			state.nestedClass = GetPointerClass(typeName);
			if (state.nestedClass)
			{
				state.isValueType = (state.nestedClass->parent == "ValueType");

				if (state.isValueType)
				{
					if (instance)
					{
						state.nestedInstance = reinterpret_cast<void*>(
							reinterpret_cast<uintptr_t>(instance) + field->offset);
					}
				}
				else
				{
					void* ptrValue = nullptr;
					if (field->static_field)
					{
						field->GetStaticValue(&ptrValue);
					}
					else if (instance)
					{
						ptrValue = *reinterpret_cast<void**>(
							reinterpret_cast<uintptr_t>(instance) + field->offset);
					}
					state.nestedInstance = ptrValue;
				}
			}
		}
	}

	ReadFieldValue();
}

void FieldEditor::OpenFieldEditor(const ComponentFieldInfo& fieldInfo, void* instance, std::string_view title)
{
	if (!IsPointerType(fieldInfo.typeName) && !IsEditableType(fieldInfo.typeName)) return;

	state.showWindow = true;
	state.windowTitle = title;
	state.targetInstance = instance;
	state.nestedClass = nullptr;
	state.nestedInstance = nullptr;
	state.isValueType = false;
	state.ownedField.reset();

	auto ownedField = std::make_unique<UR::Field>();
	ownedField->name = fieldInfo.name;
	ownedField->offset = fieldInfo.offset;
	ownedField->static_field = fieldInfo.isStatic;
	auto fieldType = std::make_unique<UR::Type>();
	fieldType->name = fieldInfo.typeName;
	ownedField->type = std::move(fieldType);
	state.ownedField = std::move(ownedField);
	state.targetField = state.ownedField.get();

	if (IsEditableType(fieldInfo.typeName))
	{
		ReadFieldValue();
		return;
	}

	state.nestedClass = GetPointerClass(fieldInfo.typeName);
	if (!state.nestedClass) return;

	state.isValueType = (state.nestedClass->parent == "ValueType");

	if (state.isValueType)
	{
		if (instance)
		{
			state.nestedInstance = reinterpret_cast<void*>(
				reinterpret_cast<uintptr_t>(instance) + fieldInfo.offset);
		}
	}
	else
	{
		if (fieldInfo.isStatic)
		{
			Helper::SafeGetStaticFieldPointer(fieldInfo.fieldHandle, state.nestedInstance);
		}
		else if (instance)
		{
			state.nestedInstance = *reinterpret_cast<void**>(
				reinterpret_cast<uintptr_t>(instance) + fieldInfo.offset);
		}
	}
}

void FieldEditor::Close()
{
	state.showWindow = false;
	state.targetField = nullptr;
	state.targetInstance = nullptr;
	state.nestedClass = nullptr;
	state.nestedInstance = nullptr;
	state.isValueType = false;
	state.ownedField.reset();
	nestedEditors.clear();
}

void FieldEditor::Render()
{
	if (!state.showWindow || !state.targetField) return;

	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(state.windowTitle.c_str(), &state.showWindow))
	{
		const UR::Field* field = state.targetField;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
		ImGui::Text("%s", field->name.c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
		ImGui::Text("(%s)", field->type ? field->type->name.c_str() : "unknown");
		ImGui::PopStyleColor();

		ImGui::Separator();

		if (state.nestedClass && state.nestedInstance)
		{
			RenderNestedInspector();
		}
		else if (state.nestedClass && !state.nestedInstance)
		{
			ImGui::TextDisabled("This field is a pointer to %s", state.nestedClass->m_name.c_str());
			ImGui::TextDisabled("Current value: null");

			ImGui::Separator();
			ImGui::TextDisabled("Cannot inspect null pointer.");
		}
		else if (IsEditableType(field->type ? field->type->name : ""))
		{
			if (const std::string typeName = field->type->name; typeName == "System.String")
			{
				RenderStringEditor();
			}
			else if (typeName == "System.Boolean" || typeName == "System.Bool")
			{
				RenderBoolEditor();
			}
			else if (IsFloatType(typeName))
			{
				RenderFloatEditor();
			}
			else
			{
				RenderIntEditor(typeName);
			}
		}
		else
		{
			ImGui::TextDisabled("This field type is not directly editable.");
			ImGui::TextDisabled("Type: %s", field->type ? field->type->name.c_str() : "unknown");
		}

		ImGui::Separator();

		if (IsEditableType(field->type ? field->type->name : "") &&
			!(state.nestedClass && !state.nestedInstance))
		{
			if (ImGui::Button("Apply", ImVec2(120, 0)))
			{
				WriteFieldValue();
				if (state.onValueChanged)
					state.onValueChanged();
			}

			ImGui::SameLine();
		}

		if (ImGui::Button("Close", ImVec2(120, 0)))
		{
			Close();
		}
	}
	ImGui::End();

	std::vector<std::unique_ptr<FieldEditor>> newEditors;
	for (auto& editor : nestedEditors)
	{
		if (editor)
		{
			editor->Render();
			editor->TakePendingEditors(newEditors);
		}
	}
	nestedEditors.insert(nestedEditors.end(), std::make_move_iterator(newEditors.begin()),
	                     std::make_move_iterator(newEditors.end()));

	std::erase_if(nestedEditors, [](const std::unique_ptr<FieldEditor>& e)
	{
		return !e || !e->IsOpen();
	});
}

bool FieldEditor::IsEditableType(std::string_view typeName)
{
	return IsIntegerType(typeName) || IsFloatType(typeName) || IsBoolType(typeName) || IsStringType(typeName) ||
		typeName == "System.Decimal" ||
		IsUInt64WrappingType(typeName);
}

bool FieldEditor::IsIntegerType(std::string_view typeName)
{
	return typeName == "System.Int16" || typeName == "System.UInt16" ||
		typeName == "System.Int32" || typeName == "System.Int" ||
		typeName == "System.UInt32" || typeName == "System.UInt" ||
		typeName == "System.Int64" || typeName == "System.UInt64" ||
		typeName == "System.Long" || typeName == "System.ULong" ||
		typeName == "System.Byte" || typeName == "System.SByte" ||
		typeName == "System.Short" || typeName == "System.UShort" ||
		typeName == "System.Char";
}

bool FieldEditor::IsFloatType(std::string_view typeName)
{
	return typeName == "System.Single" || typeName == "System.Float" ||
		typeName == "System.Double" || typeName == "System.Decimal";
}

bool FieldEditor::IsBoolType(std::string_view typeName)
{
	return typeName == "System.Boolean" || typeName == "System.Bool";
}

bool FieldEditor::IsStringType(std::string_view typeName)
{
	return typeName == "System.String";
}

bool FieldEditor::IsPointerType(std::string_view typeName)
{
	return !IsEditableType(typeName) &&
		!typeName.empty() &&
		!typeName.starts_with("System.") &&
		!typeName.starts_with("UnityEngine.");
}

UR::Class* FieldEditor::GetPointerClass(std::string_view typeName)
{
	for (const auto& assembly : UR::assembly)
	{
		if (!assembly) continue;
		for (const auto& klass : assembly->classes)
		{
			if (!klass) continue;
			if (klass->m_name == typeName)
				return klass.get();
		}
	}

	if (const size_t lastDot = typeName.rfind('.'); lastDot != std::string::npos)
	{
		const std::string_view shortName = typeName.substr(lastDot + 1);
		for (const auto& assembly : UR::assembly)
		{
			if (!assembly) continue;
			for (const auto& klass : assembly->classes)
			{
				if (!klass) continue;
				if (klass->m_name == shortName)
					return klass.get();
			}
		}
	}

	return nullptr;
}

void FieldEditor::ReadValueFromAddress(void* addr, std::string_view typeName, FieldEditorState& state)
{
	if (IsStringType(typeName))
	{
		if (const UT::String* strPtr = *static_cast<UT::String**>(addr))
		{
			const std::string str = strPtr->ToString();
			strncpy_s(state.stringBuffer, str.c_str(), sizeof(state.stringBuffer) - 1);
		}
		else
		{
			state.stringBuffer[0] = '\0';
		}
	}
	else if (IsBoolType(typeName))
	{
		state.boolValue = *static_cast<bool*>(addr);
	}
	else if (typeName == "System.Double")
	{
		state.floatValue = static_cast<float>(*static_cast<double*>(addr));
	}
	else if (IsFloatType(typeName))
	{
		state.floatValue = *static_cast<float*>(addr);
	}
	else if (typeName == "System.Int64" || typeName == "System.UInt64")
	{
		state.intValue = *static_cast<int64_t*>(addr);
	}
	else if (typeName == "System.Int16")
	{
		state.intValue = static_cast<long long>(*static_cast<int16_t*>(addr));
	}
	else if (typeName == "System.UInt16")
	{
		state.intValue = static_cast<long long>(*static_cast<uint16_t*>(addr));
	}
	else if (typeName == "System.Char")
	{
		state.intValue = static_cast<long long>(*static_cast<char16_t*>(addr));
	}
	else if (typeName == "System.Byte")
	{
		state.intValue = static_cast<long long>(*static_cast<uint8_t*>(addr));
	}
	else if (typeName == "System.SByte")
	{
		state.intValue = static_cast<long long>(*static_cast<int8_t*>(addr));
	}
	else if (typeName == "System.IntPtr")
	{
		state.intValue = *static_cast<intptr_t*>(addr);
	}
	else if (typeName == "System.UIntPtr")
	{
		state.intValue = static_cast<long long>(*static_cast<uintptr_t*>(addr));
	}
	else if (IsUInt64WrappingType(typeName))
	{
		state.intValue = static_cast<long long>(*static_cast<uint64_t*>(addr));
	}
	else if (typeName == "System.Decimal")
	{
		auto* parts = static_cast<int32_t*>(addr);
		const int scale = (parts[0] >> 16) & 0x1F;
		const bool negative = (parts[0] & 0x80000000) != 0;
		const int64_t lo = static_cast<uint32_t>(parts[2]);
		const int64_t mid = static_cast<uint32_t>(parts[3]);
		const int64_t hi = static_cast<uint32_t>(parts[1]);
		const double unscaled = static_cast<double>(lo) + static_cast<double>(mid) * 4294967296.0 +
			static_cast<double>(hi) * 18446744073709551616.0;
		state.floatValue = static_cast<float>(unscaled / std::pow(10.0, scale) * (negative ? -1.0 : 1.0));
	}
	else if (typeName == "UnityEngine.Vector2")
	{
		state.floatValue = static_cast<Vec2*>(addr)->x;
	}
	else if (typeName == "UnityEngine.Vector3")
	{
		state.floatValue = static_cast<Vec3*>(addr)->x;
	}
	else if (typeName == "UnityEngine.Vector4")
	{
		state.floatValue = static_cast<Vec4*>(addr)->x;
	}
	else if (typeName == "UnityEngine.Quaternion")
	{
		state.floatValue = static_cast<Quat*>(addr)->x;
	}
	else if (typeName == "UnityEngine.Color")
	{
		state.floatValue = static_cast<Color*>(addr)->r;
	}
	else
	{
		state.intValue = *static_cast<int32_t*>(addr);
	}
}

void FieldEditor::WriteValueToAddress(void* addr, std::string_view typeName, const FieldEditorState& state)
{
	if (IsStringType(typeName))
	{
		*static_cast<UT::String**>(addr) = UT::String::New(state.stringBuffer);
	}
	else if (IsBoolType(typeName))
	{
		*static_cast<bool*>(addr) = state.boolValue;
	}
	else if (typeName == "System.Double")
	{
		*static_cast<double*>(addr) = state.floatValue;
	}
	else if (IsFloatType(typeName))
	{
		*static_cast<float*>(addr) = state.floatValue;
	}
	else if (typeName == "System.Int64")
	{
		*static_cast<int64_t*>(addr) = state.intValue;
	}
	else if (typeName == "System.UInt64")
	{
		*static_cast<uint64_t*>(addr) = static_cast<uint64_t>(state.intValue);
	}
	else if (typeName == "System.Int16")
	{
		*static_cast<int16_t*>(addr) = static_cast<int16_t>(state.intValue);
	}
	else if (typeName == "System.UInt16")
	{
		*static_cast<uint16_t*>(addr) = static_cast<uint16_t>(state.intValue);
	}
	else if (typeName == "System.Char")
	{
		*static_cast<char16_t*>(addr) = static_cast<char16_t>(state.intValue);
	}
	else if (typeName == "System.Byte")
	{
		*static_cast<uint8_t*>(addr) = static_cast<uint8_t>(state.intValue);
	}
	else if (typeName == "System.SByte")
	{
		*static_cast<int8_t*>(addr) = static_cast<int8_t>(state.intValue);
	}
	else if (typeName == "System.IntPtr")
	{
		*static_cast<intptr_t*>(addr) = static_cast<intptr_t>(state.intValue);
	}
	else if (typeName == "System.UIntPtr")
	{
		*static_cast<uintptr_t*>(addr) = static_cast<uintptr_t>(state.intValue);
	}
	else if (IsUInt64WrappingType(typeName))
	{
		*static_cast<uint64_t*>(addr) = static_cast<uint64_t>(state.intValue);
	}
	else if (typeName == "System.Decimal")
	{
		auto* parts = static_cast<int32_t*>(addr);
		const double value = state.floatValue;
		const int scale = (parts[0] >> 16) & 0x1F;
		const bool negative = value < 0;
		const double absValue = negative ? -value : value;
		const double scaled = absValue * std::pow(10.0, scale);
		const uint64_t unscaled = static_cast<uint64_t>(scaled);
		parts[0] = (parts[0] & 0x00FF0000) | (negative ? 0x80000000 : 0);
		parts[1] = static_cast<int32_t>(unscaled >> 32);
		parts[2] = static_cast<int32_t>(unscaled & 0xFFFFFFFF);
		parts[3] = static_cast<int32_t>((unscaled >> 32) >> 32);
	}
	else if (typeName == "UnityEngine.Vector2")
	{
		auto& v = *static_cast<Vec2*>(addr);
		v.x = v.y = state.floatValue;
	}
	else if (typeName == "UnityEngine.Vector3")
	{
		auto& v = *static_cast<Vec3*>(addr);
		v.x = v.y = v.z = state.floatValue;
	}
	else if (typeName == "UnityEngine.Vector4")
	{
		auto& v = *static_cast<Vec4*>(addr);
		v.x = v.y = v.z = v.w = state.floatValue;
	}
	else if (typeName == "UnityEngine.Quaternion")
	{
		auto& v = *static_cast<Quat*>(addr);
		v.x = v.y = v.z = v.w = state.floatValue;
	}
	else if (typeName == "UnityEngine.Color")
	{
		auto& v = *static_cast<Color*>(addr);
		v.r = v.g = v.b = v.a = state.floatValue;
	}
	else
	{
		*static_cast<int32_t*>(addr) = state.intValue;
	}
}

std::string FieldEditor::FormatFieldValue(void* addr, std::string_view typeName)
{
	if (IsStringType(typeName))
	{
		if (const UT::String* strPtr = *static_cast<UT::String**>(addr))
		{
			std::string str = strPtr->ToString();
			if (str.length() > 30) str = str.substr(0, 27) + "...";
			return "\"" + str + "\"";
		}
		return "null";
	}
	if (IsBoolType(typeName))
		return *static_cast<bool*>(addr) ? "true" : "false";
	if (typeName == "System.Double")
		return std::format("{:.4f}", *static_cast<double*>(addr));
	if (IsFloatType(typeName))
		return std::format("{:.4f}", *static_cast<float*>(addr));
	if (typeName == "System.Int64")
		return std::to_string(*static_cast<int64_t*>(addr));
	if (typeName == "System.UInt64")
		return std::to_string(*static_cast<uint64_t*>(addr));
	if (typeName == "System.Int16")
		return std::to_string(*static_cast<int16_t*>(addr));
	if (typeName == "System.UInt16")
		return std::to_string(*static_cast<uint16_t*>(addr));
	if (typeName == "System.Char")
	{
		char16_t val = *static_cast<char16_t*>(addr);
		if (val >= 32 && val < 127)
			return std::format("'{}'", static_cast<char>(val));
		return std::format("'\\u{:04X}'", static_cast<int>(val));
	}
	if (typeName == "System.Byte")
		return std::to_string(*static_cast<uint8_t*>(addr));
	if (typeName == "System.SByte")
		return std::to_string(*static_cast<int8_t*>(addr));
	if (typeName == "System.IntPtr")
		return std::to_string(*static_cast<intptr_t*>(addr));
	if (typeName == "System.UIntPtr")
		return std::to_string(*static_cast<uintptr_t*>(addr));
	if (IsUInt64WrappingType(typeName))
		return std::to_string(*static_cast<uint64_t*>(addr));
	if (typeName == "System.Decimal")
	{
		const auto* parts = static_cast<int32_t*>(addr);
		const int scale = (parts[0] >> 16) & 0x1F;
		const bool negative = (parts[0] & 0x80000000) != 0;
		const int64_t lo = static_cast<uint32_t>(parts[2]);
		const int64_t mid = static_cast<uint32_t>(parts[3]);
		const int64_t hi = static_cast<uint32_t>(parts[1]);
		const double unscaled = static_cast<double>(lo) + static_cast<double>(mid) * 4294967296.0 +
			static_cast<double>(hi) * 18446744073709551616.0;
		const double value = unscaled / std::pow(10.0, scale) * (negative ? -1.0 : 1.0);
		return std::format("{:.6f}", value);
	}
	if (typeName == "UnityEngine.Vector2")
	{
		const auto& v = *static_cast<Vec2*>(addr);
		return std::format("({:.3f}, {:.3f})", v.x, v.y);
	}
	if (typeName == "UnityEngine.Vector3")
	{
		const auto& v = *static_cast<Vec3*>(addr);
		return std::format("({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
	}
	if (typeName == "UnityEngine.Vector4")
	{
		const auto& v = *static_cast<Vec4*>(addr);
		return std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
	}
	if (typeName == "UnityEngine.Quaternion")
	{
		const auto& v = *static_cast<Quat*>(addr);
		return std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
	}
	if (typeName == "UnityEngine.Color")
	{
		const auto& v = *static_cast<Color*>(addr);
		return std::format("RGBA({:.2f}, {:.2f}, {:.2f}, {:.2f})", v.r, v.g, v.b, v.a);
	}
	if (IsIntegerType(typeName))
		return std::to_string(*static_cast<int32_t*>(addr));

	if (void* ptr = *static_cast<void**>(addr))
		return std::format("0x{:X}", reinterpret_cast<uintptr_t>(ptr));
	return "null";
}

void FieldEditor::ReadFieldValue()
{
	const UR::Field* field = state.targetField;
	if (!field || !field->type) return;

	const std::string_view typeName = field->type->name;

	try
	{
		if (field->static_field)
		{
			if (IsStringType(typeName))
			{
				UT::String* strPtr = nullptr;
				field->GetStaticValue(&strPtr);
				if (strPtr)
					strncpy_s(state.stringBuffer, strPtr->ToString().c_str(), sizeof(state.stringBuffer) - 1);
				else
					state.stringBuffer[0] = '\0';
			}
			else if (IsBoolType(typeName))
			{
				bool val = false;
				field->GetStaticValue(&val);
				state.boolValue = val;
			}
			else if (typeName == "System.Double")
			{
				double val = 0.0;
				field->GetStaticValue(&val);
				state.floatValue = static_cast<float>(val);
			}
			else if (IsFloatType(typeName))
			{
				float val = 0.0f;
				field->GetStaticValue(&val);
				state.floatValue = val;
			}
			else if (typeName == "System.Byte")
			{
				uint8_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "System.SByte")
			{
				int8_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "System.IntPtr")
			{
				intptr_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = val;
			}
			else if (typeName == "System.UIntPtr")
			{
				uintptr_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (IsUInt64WrappingType(typeName))
			{
				uint64_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "System.Decimal")
			{
				int32_t parts[4] = {};
				field->GetStaticValue(&parts);
				const int scale = (parts[0] >> 16) & 0x1F;
				const bool negative = (parts[0] & 0x80000000) != 0;
				const int64_t lo = static_cast<uint32_t>(parts[2]);
				const int64_t mid = static_cast<uint32_t>(parts[3]);
				const int64_t hi = static_cast<uint32_t>(parts[1]);
				const double unscaled = static_cast<double>(lo) + static_cast<double>(mid) * 4294967296.0 +
					static_cast<double>(hi) * 18446744073709551616.0;
				state.floatValue = static_cast<float>(unscaled / std::pow(10.0, scale) * (negative ? -1.0 : 1.0));
			}
			else if (typeName == "System.Int16" || typeName == "System.Short")
			{
				int16_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "System.UInt16" || typeName == "System.UShort")
			{
				uint16_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "System.Char")
			{
				char16_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = static_cast<long long>(val);
			}
			else if (typeName == "UnityEngine.Vector2")
			{
				Vec2 val = {};
				field->GetStaticValue(&val);
				state.floatValue = val.x;
			}
			else if (typeName == "UnityEngine.Vector3")
			{
				Vec3 val = {};
				field->GetStaticValue(&val);
				state.floatValue = val.x;
			}
			else if (typeName == "UnityEngine.Vector4")
			{
				Vec4 val = {};
				field->GetStaticValue(&val);
				state.floatValue = val.x;
			}
			else if (typeName == "UnityEngine.Quaternion")
			{
				Quat val = {};
				field->GetStaticValue(&val);
				state.floatValue = val.x;
			}
			else if (typeName == "UnityEngine.Color")
			{
				Color val = {};
				field->GetStaticValue(&val);
				state.floatValue = val.r;
			}
			else if (DetermineEditableType(typeName) == EditableType::Enum)
			{
				int32_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = val;
			}
			else
			{
				int64_t val = 0;
				field->GetStaticValue(&val);
				state.intValue = val;
			}
		}
		else if (state.targetInstance)
		{
			const auto fieldAddr = reinterpret_cast<void*>(
				reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
			ReadValueFromAddress(fieldAddr, typeName, state);
		}
	}
	catch (...)
	{
	}
}

void FieldEditor::WriteFieldValue()
{
	const UR::Field* field = state.targetField;
	if (!field || !field->type) return;

	const std::string_view typeName = field->type->name;

	try
	{
		if (field->static_field)
		{
			if (IsStringType(typeName))
			{
				UT::String* newStr = UT::String::New(state.stringBuffer);
				field->SetStaticValue(&newStr);
			}
			else if (IsBoolType(typeName))
			{
				field->SetStaticValue(&state.boolValue);
			}
			else if (typeName == "System.Double")
			{
				double val = state.floatValue;
				field->SetStaticValue(&val);
			}
			else if (IsFloatType(typeName))
			{
				field->SetStaticValue(&state.floatValue);
			}
			else if (typeName == "System.Int64")
			{
				int64_t val = state.intValue;
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.UInt64")
			{
				uint64_t val = static_cast<uint64_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.Int16")
			{
				int16_t val = static_cast<int16_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.UInt16")
			{
				uint16_t val = static_cast<uint16_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.Char")
			{
				char16_t val = static_cast<char16_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.Byte")
			{
				uint8_t val = static_cast<uint8_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.SByte")
			{
				int8_t val = static_cast<int8_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.IntPtr")
			{
				intptr_t val = state.intValue;
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.UIntPtr")
			{
				uintptr_t val = static_cast<uintptr_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (IsUInt64WrappingType(typeName))
			{
				uint64_t val = static_cast<uint64_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else if (typeName == "System.Decimal")
			{
				int32_t parts[4] = {};
				field->GetStaticValue(&parts);
				const int scale = (parts[0] >> 16) & 0x1F;
				const double value = state.floatValue;
				const bool negative = value < 0;
				const double absValue = negative ? -value : value;
				const double scaled = absValue * std::pow(10.0, scale);
				const uint64_t unscaled = static_cast<uint64_t>(scaled);
				parts[0] = (parts[0] & 0x00FF0000) | (negative ? 0x80000000 : 0);
				parts[1] = static_cast<int32_t>(unscaled >> 32);
				parts[2] = static_cast<int32_t>(unscaled & 0xFFFFFFFF);
				parts[3] = static_cast<int32_t>((unscaled >> 32) >> 32);
				field->SetStaticValue(&parts);
			}
			else if (typeName == "UnityEngine.Vector2")
			{
				Vec2 val = { state.floatValue, state.floatValue };
				field->SetStaticValue(&val);
			}
			else if (typeName == "UnityEngine.Vector3")
			{
				Vec3 val = { state.floatValue, state.floatValue, state.floatValue };
				field->SetStaticValue(&val);
			}
			else if (typeName == "UnityEngine.Vector4")
			{
				Vec4 val = { state.floatValue, state.floatValue, state.floatValue, state.floatValue };
				field->SetStaticValue(&val);
			}
			else if (typeName == "UnityEngine.Quaternion")
			{
				Quat val = { state.floatValue, state.floatValue, state.floatValue, state.floatValue };
				field->SetStaticValue(&val);
			}
			else if (typeName == "UnityEngine.Color")
			{
				Color val = { state.floatValue, state.floatValue, state.floatValue, state.floatValue };
				field->SetStaticValue(&val);
			}
			else if (DetermineEditableType(typeName) == EditableType::Enum)
			{
				int32_t val = static_cast<int32_t>(state.intValue);
				field->SetStaticValue(&val);
			}
			else
			{
				int32_t val = state.intValue;
				field->SetStaticValue(&val);
			}
		}
		else if (state.targetInstance)
		{
			const auto fieldAddr = reinterpret_cast<void*>(
				reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
			WriteValueToAddress(fieldAddr, typeName, state);
		}
	}
	catch (...)
	{
	}
}

void FieldEditor::RenderIntEditor(std::string_view typeName)
{
	ImGui::Text("Value:");
	if (typeName == "System.UInt64")
		ImGui::InputScalar("##int_value", ImGuiDataType_U64, &state.intValue);
	else
		ImGui::InputScalar("##int_value", ImGuiDataType_S64, &state.intValue);
	if (typeName == "System.Char")
	{
		ImGui::SameLine();
		if (char16_t val = static_cast<char16_t>(state.intValue); val >= 32 && val < 127)
			ImGui::Text("'%c'", static_cast<char>(val));
		else
			ImGui::Text("'\\u%04X'", static_cast<int>(val));
	}
}

void FieldEditor::RenderFloatEditor()
{
	ImGui::Text("Value:");
	ImGui::InputFloat("##float_value", &state.floatValue, 0.1f, 1.0f, "%.6f");
}

void FieldEditor::RenderBoolEditor()
{
	ImGui::Text("Value:");
	ImGui::Checkbox("##bool_value", &state.boolValue);
}

void FieldEditor::RenderStringEditor()
{
	ImGui::Text("Value:");
	ImGui::InputTextMultiline("##string_value", state.stringBuffer, sizeof(state.stringBuffer),
	                          ImVec2(-1, ImGui::GetTextLineHeight() * 4));
}

void FieldEditor::TakePendingEditors(std::vector<std::unique_ptr<FieldEditor>>& out)
{
	out.insert(out.end(), std::make_move_iterator(pendingEditors.begin()),
	           std::make_move_iterator(pendingEditors.end()));
	pendingEditors.clear();
}

void FieldEditor::RenderNestedInspector()
{
	if (!state.nestedClass || !state.nestedInstance) return;

	ImGui::TextDisabled("This field points to an instance of:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
	ImGui::Text("%s", state.nestedClass->m_name.c_str());
	ImGui::PopStyleColor();

	ImGui::TextDisabled("Address: %p", state.nestedInstance);

	ImGui::Separator();

	ImGui::Text("Nested Type Fields:");

	if (ImGui::BeginTable("NestedFieldsTable", 4,
	                      ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 50.0f);

		for (const auto& field : state.nestedClass->fields)
		{
			if (!field) continue;

			const bool isEditable = IsEditableType(field->type ? field->type->name : "");
			const bool isPointer = IsPointerType(field->type ? field->type->name : "");

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImVec4 color = field->static_field ? ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(field->name.c_str());
			ImGui::PopStyleColor();

			ImGui::TableSetColumnIndex(1);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
			std::string typeName = field->type ? field->type->name : "unknown";
			if (typeName.length() > 30) typeName = typeName.substr(0, 27) + "...";
			ImGui::TextUnformatted(typeName.c_str());
			ImGui::PopStyleColor();

			ImGui::TableSetColumnIndex(2);
			RenderNestedFieldValue(field.get(), state.nestedInstance);

			ImGui::TableSetColumnIndex(3);
			if ((isEditable || isPointer) && !field->static_field)
			{
				ImGui::PushID(field.get());
				if (ImGui::SmallButton("Edit"))
				{
					std::string title = "Edit " + state.nestedClass->m_name + "." + field->name;
					auto editor = std::make_unique<FieldEditor>();
					editor->OpenFieldEditor(field.get(), state.nestedInstance, title);
					pendingEditors.push_back(std::move(editor));
				}
				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}
}

void FieldEditor::RenderNestedFieldValue(const UR::Field* field, void* instance) const
{
	if (!field || !field->type || !instance)
	{
		ImGui::TextDisabled("-");
		return;
	}

	const std::string typeName = field->type->name;
	const auto fieldAddr = reinterpret_cast<void*>(
		reinterpret_cast<uintptr_t>(instance) + field->offset);

	try
	{
		const std::string formatted = FormatFieldValue(fieldAddr, typeName);
		if (const bool isNull = (formatted == "null"); isNull || (!IsEditableType(typeName) && !
			IsPointerType(typeName)))
			ImGui::TextDisabled("%s", formatted.c_str());
		else
			ImGui::TextUnformatted(formatted.c_str());
	}
	catch (...)
	{
		ImGui::TextDisabled("Error");
	}
}
