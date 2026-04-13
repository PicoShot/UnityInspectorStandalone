#include "pch.h"
#include "field_editor.h"

EditableType DetermineEditableType(const std::string& typeName)
{
    if (typeName == "System.Int16" || typeName == "System.Int32" || typeName == "System.Int64" ||
        typeName == "System.UInt16" || typeName == "System.UInt32" || typeName == "System.UInt64" ||
        typeName == "System.Byte" || typeName == "System.SByte")
        return EditableType::Int;
    if (typeName == "System.Single")
        return EditableType::Float;
    if (typeName == "System.Double")
        return EditableType::Double;
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
    return EditableType::None;
}

FieldEditor::FieldEditor() = default;
FieldEditor::~FieldEditor() = default;

void FieldEditor::OpenFieldEditor(UR::Field* field, void* instance, const std::string& title)
{
    if (!field) return;
    
    state.targetField = field;
    state.targetInstance = instance;
    state.windowTitle = title;
    state.showWindow = true;
    
    state.nestedClass = nullptr;
    state.nestedInstance = nullptr;
    
    if (field->type)
    {
        std::string typeName = field->type->name;
        if (IsPointerType(typeName))
        {
            state.nestedClass = GetPointerClass(typeName);
            if (state.nestedClass)
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
    
    ReadFieldValue();
}

void FieldEditor::Close()
{
    state.showWindow = false;
    state.targetField = nullptr;
    state.targetInstance = nullptr;
    state.nestedClass = nullptr;
    state.nestedInstance = nullptr;
    nestedEditors.clear();
}

void FieldEditor::Render()
{
    if (!state.showWindow || !state.targetField) return;
    
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin(state.windowTitle.c_str(), &state.showWindow))
    {
        UR::Field* field = state.targetField;
        
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
            ImGui::TextDisabled("This field is a pointer to %s", state.nestedClass->name.c_str());
            ImGui::TextDisabled("Current value: null");
            
            ImGui::Separator();
            ImGui::TextDisabled("Cannot inspect null pointer.");
        }
        else if (IsEditableType(field->type ? field->type->name : ""))
        {
            std::string typeName = field->type->name;
            
            if (typeName == "System.String")
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
                RenderIntEditor();
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

    for (auto& editor : nestedEditors)
    {
        if (editor) editor->Render();
    }

    std::erase_if(nestedEditors, [](const std::unique_ptr<FieldEditor>& e) {
        return !e || !e->IsOpen();
    });
}

bool FieldEditor::IsEditableType(const std::string& typeName)
{
    return IsIntegerType(typeName) || IsFloatType(typeName) || IsBoolType(typeName) || IsStringType(typeName);
}

bool FieldEditor::IsIntegerType(const std::string& typeName)
{
    return typeName == "System.Int16" || typeName == "System.UInt16" ||
           typeName == "System.Int32" || typeName == "System.Int" ||
           typeName == "System.UInt32" || typeName == "System.UInt" ||
           typeName == "System.Int64" || typeName == "System.UInt64" ||
           typeName == "System.Long" || typeName == "System.ULong" ||
           typeName == "System.Byte" || typeName == "System.SByte" ||
           typeName == "System.Short" || typeName == "System.UShort";
}

bool FieldEditor::IsFloatType(const std::string& typeName)
{
    return typeName == "System.Single" || typeName == "System.Float" ||
           typeName == "System.Double" || typeName == "System.Decimal";
}

bool FieldEditor::IsBoolType(const std::string& typeName)
{
    return typeName == "System.Boolean" || typeName == "System.Bool";
}

bool FieldEditor::IsStringType(const std::string& typeName)
{
    return typeName == "System.String";
}

bool FieldEditor::IsPointerType(const std::string& typeName)
{
    return !IsEditableType(typeName) && 
           !typeName.empty() &&
           typeName.find("System.") != 0 &&
           typeName.find("UnityEngine.") != 0;
}

UR::Class* FieldEditor::GetPointerClass(const std::string& typeName)
{
    for (const auto& assembly : UR::assembly)
    {
        if (!assembly) continue;
        for (const auto& klass : assembly->classes)
        {
            if (!klass) continue;
            if (klass->name == typeName)
                return klass.get();
        }
    }
    
    size_t lastDot = typeName.rfind('.');
    if (lastDot != std::string::npos)
    {
        std::string shortName = typeName.substr(lastDot + 1);
        for (const auto& assembly : UR::assembly)
        {
            if (!assembly) continue;
            for (const auto& klass : assembly->classes)
            {
                if (!klass) continue;
                if (klass->name == shortName)
                    return klass.get();
            }
        }
    }
    
    return nullptr;
}

void FieldEditor::ReadValueFromAddress(void* addr, const std::string& typeName, FieldEditorState& state)
{
    if (IsStringType(typeName))
    {
        UT::String* strPtr = *reinterpret_cast<UT::String**>(addr);
        if (strPtr)
        {
            std::string str = strPtr->ToString();
            strncpy_s(state.stringBuffer, str.c_str(), sizeof(state.stringBuffer) - 1);
        }
        else
        {
            state.stringBuffer[0] = '\0';
        }
    }
    else if (IsBoolType(typeName))
    {
        state.boolValue = *reinterpret_cast<bool*>(addr);
    }
    else if (typeName == "System.Double")
    {
        state.floatValue = static_cast<float>(*reinterpret_cast<double*>(addr));
    }
    else if (IsFloatType(typeName))
    {
        state.floatValue = *reinterpret_cast<float*>(addr);
    }
    else if (typeName == "System.Int64" || typeName == "System.UInt64")
    {
        state.intValue = static_cast<int>(*reinterpret_cast<int64_t*>(addr));
    }
    else if (typeName == "System.Int16" || typeName == "System.UInt16")
    {
        state.intValue = static_cast<int>(*reinterpret_cast<int16_t*>(addr));
    }
    else if (typeName == "System.Byte" || typeName == "System.SByte")
    {
        state.intValue = static_cast<int>(*reinterpret_cast<uint8_t*>(addr));
    }
    else
    {
        state.intValue = *reinterpret_cast<int32_t*>(addr);
    }
}

void FieldEditor::WriteValueToAddress(void* addr, const std::string& typeName, const FieldEditorState& state)
{
    if (IsStringType(typeName))
    {
        *reinterpret_cast<UT::String**>(addr) = UT::String::New(state.stringBuffer);
    }
    else if (IsBoolType(typeName))
    {
        *reinterpret_cast<bool*>(addr) = state.boolValue;
    }
    else if (typeName == "System.Double")
    {
        *reinterpret_cast<double*>(addr) = state.floatValue;
    }
    else if (IsFloatType(typeName))
    {
        *reinterpret_cast<float*>(addr) = state.floatValue;
    }
    else if (typeName == "System.Int64")
    {
        *reinterpret_cast<int64_t*>(addr) = state.intValue;
    }
    else if (typeName == "System.UInt64")
    {
        *reinterpret_cast<uint64_t*>(addr) = static_cast<uint64_t>(state.intValue);
    }
    else if (typeName == "System.Int16")
    {
        *reinterpret_cast<int16_t*>(addr) = static_cast<int16_t>(state.intValue);
    }
    else if (typeName == "System.UInt16")
    {
        *reinterpret_cast<uint16_t*>(addr) = static_cast<uint16_t>(state.intValue);
    }
    else if (typeName == "System.Byte")
    {
        *reinterpret_cast<uint8_t*>(addr) = static_cast<uint8_t>(state.intValue);
    }
    else if (typeName == "System.SByte")
    {
        *reinterpret_cast<int8_t*>(addr) = static_cast<int8_t>(state.intValue);
    }
    else
    {
        *reinterpret_cast<int32_t*>(addr) = state.intValue;
    }
}

std::string FieldEditor::FormatFieldValue(void* addr, const std::string& typeName)
{
    if (IsStringType(typeName))
    {
        UT::String* strPtr = *reinterpret_cast<UT::String**>(addr);
        if (strPtr)
        {
            std::string str = strPtr->ToString();
            if (str.length() > 30) str = str.substr(0, 27) + "...";
            return "\"" + str + "\"";
        }
        return "null";
    }
    if (IsBoolType(typeName))
        return *reinterpret_cast<bool*>(addr) ? "true" : "false";
    if (typeName == "System.Double")
        return std::format("{:.4f}", *reinterpret_cast<double*>(addr));
    if (IsFloatType(typeName))
        return std::format("{:.4f}", *reinterpret_cast<float*>(addr));
    if (typeName == "System.Int64")
        return std::to_string(*reinterpret_cast<int64_t*>(addr));
    if (typeName == "System.UInt64")
        return std::to_string(*reinterpret_cast<uint64_t*>(addr));
    if (typeName == "System.Int16")
        return std::to_string(*reinterpret_cast<int16_t*>(addr));
    if (typeName == "System.UInt16")
        return std::to_string(*reinterpret_cast<uint16_t*>(addr));
    if (typeName == "System.Byte")
        return std::to_string(*reinterpret_cast<uint8_t*>(addr));
    if (typeName == "System.SByte")
        return std::to_string(*reinterpret_cast<int8_t*>(addr));
    if (IsIntegerType(typeName))
        return std::to_string(*reinterpret_cast<int32_t*>(addr));

    void* ptr = *reinterpret_cast<void**>(addr);
    if (ptr)
        return std::format("0x{:X}", reinterpret_cast<uintptr_t>(ptr));
    return "null";
}

void FieldEditor::ReadFieldValue()
{
    UR::Field* field = state.targetField;
    if (!field || !field->type) return;

    std::string typeName = field->type->name;

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
            else
            {
                int64_t val = 0;
                field->GetStaticValue(&val);
                state.intValue = static_cast<int>(val);
            }
        }
        else if (state.targetInstance)
        {
            void* fieldAddr = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
            ReadValueFromAddress(fieldAddr, typeName, state);
        }
    }
    catch (...) {}
}

void FieldEditor::WriteFieldValue()
{
    UR::Field* field = state.targetField;
    if (!field || !field->type) return;

    std::string typeName = field->type->name;

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
            else
            {
                int32_t val = state.intValue;
                field->SetStaticValue(&val);
            }
        }
        else if (state.targetInstance)
        {
            void* fieldAddr = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
            WriteValueToAddress(fieldAddr, typeName, state);
        }
    }
    catch (...) {}
}

void FieldEditor::RenderIntEditor()
{
    ImGui::Text("Value:");
    ImGui::InputInt("##int_value", &state.intValue);
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

void FieldEditor::RenderNestedInspector()
{
    if (!state.nestedClass || !state.nestedInstance) return;
    
    ImGui::TextDisabled("This field points to an instance of:");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("%s", state.nestedClass->name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::TextDisabled("Address: %p", state.nestedInstance);
    
    ImGui::Separator();
    
    ImGui::Text("Nested Type Fields:");
    
    if (ImGui::BeginTable("NestedFieldsTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        
        for (const auto& field : state.nestedClass->fields)
        {
            if (!field) continue;
            
            bool isEditable = IsEditableType(field->type ? field->type->name : "");
            bool isPointer = IsPointerType(field->type ? field->type->name : "");
            
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            ImVec4 color = field->static_field ? 
                ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
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
                    std::string title = "Edit " + state.nestedClass->name + "." + field->name;
                    auto editor = std::make_unique<FieldEditor>();
                    editor->OpenFieldEditor(field.get(), state.nestedInstance, title);
                    nestedEditors.push_back(std::move(editor));
                }
                ImGui::PopID();
            }
        }
        
        ImGui::EndTable();
    }
}

void FieldEditor::RenderNestedFieldValue(UR::Field* field, void* instance)
{
    if (!field || !field->type || !instance)
    {
        ImGui::TextDisabled("-");
        return;
    }

    std::string typeName = field->type->name;
    void* fieldAddr = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(instance) + field->offset);

    try
    {
        std::string formatted = FormatFieldValue(fieldAddr, typeName);
        bool isNull = (formatted == "null");
        if (isNull || (!IsEditableType(typeName) && !IsPointerType(typeName)))
            ImGui::TextDisabled("%s", formatted.c_str());
        else
            ImGui::TextUnformatted(formatted.c_str());
    }
    catch (...)
    {
        ImGui::TextDisabled("Error");
    }
}
