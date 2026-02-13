#pragma once
#include "game/core/core.h"
#include <functional>

// Forward declarations
struct FieldEditor;

// Represents a field value that can be edited
struct EditableFieldValue
{
    std::string name;
    std::string typeName;
    UR::Field* field = nullptr;
    UR::Type* type = nullptr;
    
    // For simple types, we store the value directly
    union SimpleValue
    {
        int32_t int32;
        int64_t int64;
        float float32;
        double float64;
        bool boolean;
        void* pointer;
    } value;
    
    bool isStatic = false;
    bool isPointer = false;     // Is this a reference/pointer to another type?
    bool isEditable = false;    // Can we edit this type directly?
    
    void* instance = nullptr;   // For instance fields
    int32_t offset = 0;         // Field offset
};

// State for the field editor window
struct FieldEditorState
{
    bool showWindow = false;
    std::string windowTitle;
    
    // The field being edited
    UR::Field* targetField = nullptr;
    void* targetInstance = nullptr;  // null for static fields
    
    // For nested inspection (when field is a pointer to another type)
    UR::Class* nestedClass = nullptr;
    void* nestedInstance = nullptr;
    
    // Input buffers for editing
    char stringBuffer[1024] = {};
    int intValue = 0;
    float floatValue = 0.0f;
    bool boolValue = false;
    
    // Callback when value is changed
    std::function<void()> onValueChanged;
};

// Field Editor component - handles editing of field values
struct FieldEditor final
{
    FieldEditor();
    ~FieldEditor();
    
    // Open editor for a field
    void OpenFieldEditor(UR::Field* field, void* instance, const std::string& title);
    
    // Render the editor window (call every frame)
    void Render();
    
    // Check if editor is currently open
    bool IsOpen() const { return state.showWindow; }
    
    // Close the editor
    void Close();
    
public:
    // Utility methods for checking type properties (public for external use)
    static bool IsEditableType(const std::string& typeName);
    static bool IsPointerType(const std::string& typeName);
    
private:
    FieldEditorState state;
    
    // Get the underlying class for a pointer type
    static UR::Class* GetPointerClass(const std::string& typeName);
    
    // Read current value from field
    void ReadFieldValue();
    
    // Write value to field
    void WriteFieldValue();
    
    // Render editors for different types
    void RenderIntEditor();
    void RenderFloatEditor();
    void RenderBoolEditor();
    void RenderStringEditor();
    void RenderPointerEditor();
    
    // Render window for nested type inspection
    void RenderNestedInspector();
};
