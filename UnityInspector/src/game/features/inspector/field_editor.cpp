#include "pch.h"
#include "field_editor.h"

FieldEditor::FieldEditor() = default;
FieldEditor::~FieldEditor() = default;

void FieldEditor::OpenFieldEditor(UR::Field* field, void* instance, const std::string& title)
{
    if (!field) return;
    
    state.targetField = field;
    state.targetInstance = instance;
    state.windowTitle = title;
    state.showWindow = true;
    
    // Determine if this is a pointer type
    state.nestedClass = nullptr;
    state.nestedInstance = nullptr;
    
    if (field->type)
    {
        std::string typeName = field->type->name;
        if (IsPointerType(typeName))
        {
            // Try to get the underlying class
            state.nestedClass = GetPointerClass(typeName);
            if (state.nestedClass)
            {
                // Read the pointer value to get the instance
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
    
    // Read current value
    ReadFieldValue();
}

void FieldEditor::Close()
{
    state.showWindow = false;
    state.targetField = nullptr;
    state.targetInstance = nullptr;
    state.nestedClass = nullptr;
    state.nestedInstance = nullptr;
}

void FieldEditor::Render()
{
    if (!state.showWindow || !state.targetField) return;
    
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin(state.windowTitle.c_str(), &state.showWindow))
    {
        UR::Field* field = state.targetField;
        
        // Field info header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
        ImGui::Text("%s", field->name.c_str());
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
        ImGui::Text("(%s)", field->type ? field->type->name.c_str() : "unknown");
        ImGui::PopStyleColor();
        
        ImGui::Separator();
        
        // Check if this is a pointer/nested type
        if (state.nestedClass && state.nestedInstance)
        {
            // Show pointer inspector
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
            // Show simple type editor
            std::string typeName = field->type->name;
            
            if (typeName == "System.Int32" || typeName == "System.Int")
            {
                RenderIntEditor();
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                RenderFloatEditor();
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                RenderBoolEditor();
            }
            else if (typeName == "System.String")
            {
                RenderStringEditor();
            }
        }
        else
        {
            ImGui::TextDisabled("This field type is not directly editable.");
            ImGui::TextDisabled("Type: %s", field->type ? field->type->name.c_str() : "unknown");
        }
        
        ImGui::Separator();
        
        // Buttons
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
}

bool FieldEditor::IsEditableType(const std::string& typeName)
{
    return typeName == "System.Int32" || typeName == "System.Int" ||
           typeName == "System.Int64" || typeName == "System.Long" ||
           typeName == "System.Single" || typeName == "System.Float" ||
           typeName == "System.Double" || typeName == "System.Boolean" ||
           typeName == "System.Bool" || typeName == "System.String";
}

bool FieldEditor::IsPointerType(const std::string& typeName)
{
    // Check if type name indicates a pointer/reference type
    // Unity types usually don't have explicit pointer syntax in type names
    // Instead, we check if it's NOT a primitive type
    return !IsEditableType(typeName) && 
           !typeName.empty() &&
           typeName.find("System.") != 0;  // Not a system type
}

UR::Class* FieldEditor::GetPointerClass(const std::string& typeName)
{
    // Try to find the class for this type name
    // First, try as-is
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
    
    // Try without namespace
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

void FieldEditor::ReadFieldValue()
{
    UR::Field* field = state.targetField;
    if (!field || !field->type) return;
    
    std::string typeName = field->type->name;
    
    try
    {
        if (field->static_field)
        {
            // Static field
            if (typeName == "System.Int32" || typeName == "System.Int")
            {
                int32_t val = 0;
                field->GetStaticValue(&val);
                state.intValue = val;
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                float val = 0.0f;
                field->GetStaticValue(&val);
                state.floatValue = val;
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                bool val = false;
                field->GetStaticValue(&val);
                state.boolValue = val;
            }
        }
        else if (state.targetInstance)
        {
            // Instance field
            void* fieldAddr = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
            
            if (typeName == "System.Int32" || typeName == "System.Int")
            {
                state.intValue = *reinterpret_cast<int32_t*>(fieldAddr);
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                state.floatValue = *reinterpret_cast<float*>(fieldAddr);
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                state.boolValue = *reinterpret_cast<bool*>(fieldAddr);
            }
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
            // Static field
            if (typeName == "System.Int32" || typeName == "System.Int")
            {
                int32_t val = state.intValue;
                field->SetStaticValue(&val);
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                float val = state.floatValue;
                field->SetStaticValue(&val);
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                bool val = state.boolValue;
                field->SetStaticValue(&val);
            }
        }
        else if (state.targetInstance)
        {
            // Instance field
            void* fieldAddr = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(state.targetInstance) + field->offset);
            
            if (typeName == "System.Int32" || typeName == "System.Int")
            {
                *reinterpret_cast<int32_t*>(fieldAddr) = state.intValue;
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                *reinterpret_cast<float*>(fieldAddr) = state.floatValue;
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                *reinterpret_cast<bool*>(fieldAddr) = state.boolValue;
            }
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
    ImGui::InputFloat("##float_value", &state.floatValue, 0.1f, 1.0f, "%.3f");
}

void FieldEditor::RenderBoolEditor()
{
    ImGui::Text("Value:");
    ImGui::Checkbox("##bool_value", &state.boolValue);
}

void FieldEditor::RenderStringEditor()
{
    ImGui::Text("Value:");
    ImGui::InputText("##string_value", state.stringBuffer, sizeof(state.stringBuffer));
    
    ImGui::TextDisabled("Note: String editing is limited in this version.");
}

void FieldEditor::RenderPointerEditor()
{
    // This is handled in RenderNestedInspector
}

void FieldEditor::RenderNestedInspector()
{
    if (!state.nestedClass || !state.nestedInstance) return;
    
    ImGui::TextDisabled("This field points to an instance of:");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("%s", state.nestedClass->name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::Separator();
    
    // Show fields of the nested type
    ImGui::Text("Nested Fields:");
    ImGui::Indent();
    
    for (const auto& field : state.nestedClass->fields)
    {
        if (!field) continue;
        
        ImVec4 color = field->static_field ? 
            ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
            ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(field->name.c_str());
        ImGui::PopStyleColor();
        
        ImGui::SameLine(150);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
        std::string typeName = field->type ? field->type->name : "unknown";
        if (typeName.length() > 30) typeName = typeName.substr(0, 27) + "...";
        ImGui::TextUnformatted(typeName.c_str());
        ImGui::PopStyleColor();
    }
    
    ImGui::Unindent();
    
    ImGui::Separator();
    
    if (ImGui::Button("Inspect Nested Object", ImVec2(200, 0)))
    {
        // TODO: Open a new field editor window for the nested instance
        // This would require a stack of field editors or a more complex system
        ImGui::OpenPopup("Nested inspection not yet implemented");
    }
}
