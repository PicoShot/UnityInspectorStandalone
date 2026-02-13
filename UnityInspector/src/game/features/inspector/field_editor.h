#pragma once
#include "game/core/core.h"
#include <functional>

struct FieldEditor;

struct EditableFieldValue
{
    std::string name;
    std::string typeName;
    UR::Field* field = nullptr;
    UR::Type* type = nullptr;
    
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
    bool isPointer = false;
    bool isEditable = false;
    
    void* instance = nullptr;
    int32_t offset = 0;
};

struct FieldEditorState
{
    bool showWindow = false;
    std::string windowTitle;
    
    UR::Field* targetField = nullptr;
    void* targetInstance = nullptr;
    
    UR::Class* nestedClass = nullptr;
    void* nestedInstance = nullptr;
    
    char stringBuffer[1024] = {};
    int intValue = 0;
    float floatValue = 0.0f;
    bool boolValue = false;
    
    std::function<void()> onValueChanged;
};

struct FieldEditor final
{
    FieldEditor();
    ~FieldEditor();
    
    void OpenFieldEditor(UR::Field* field, void* instance, const std::string& title);
    
    void Render();
    
    bool IsOpen() const { return state.showWindow; }
    
    void Close();
    
    static bool IsEditableType(const std::string& typeName);
    static bool IsIntegerType(const std::string& typeName);
    static bool IsFloatType(const std::string& typeName);
    static bool IsBoolType(const std::string& typeName);
    static bool IsStringType(const std::string& typeName);
    static bool IsPointerType(const std::string& typeName);
    
private:
    FieldEditorState state;
    
    static UR::Class* GetPointerClass(const std::string& typeName);
    void ReadFieldValue();
    void WriteFieldValue();
    void RenderIntEditor();
    void RenderFloatEditor();
    void RenderBoolEditor();
    void RenderStringEditor();
    
    void RenderNestedInspector();
    
    void RenderNestedFieldValue(UR::Field* field, void* instance);
};
