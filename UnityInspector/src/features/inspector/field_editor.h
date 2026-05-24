#pragma once
#include "features/features.h"
#include "features/inspector/editable_type.h"

EditableType DetermineEditableType(const std::string& typeName, std::string* enumTypeNameOut = nullptr);

std::vector<std::pair<std::string, int>> GetEnumValues(const std::string& enumTypeName);

struct ComponentFieldInfo final
{
	std::string name;
	std::string typeName;
	std::string enumTypeName;
	int offset;
	void* fieldHandle;
	void* classHandle = nullptr;
	void* typeClassHandle = nullptr;
	bool isStatic = false;
	bool isValueType = false;
	EditableType editableType = EditableType::None;
};

struct FieldEditorState
{
	bool showWindow = false;
	std::string windowTitle;

	UR::Field* targetField = nullptr;
	void* targetInstance = nullptr;

	UR::Class* nestedClass = nullptr;
	void* nestedInstance = nullptr;

	std::unique_ptr<UR::Field> ownedField;

	bool isValueType = false;

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
	void OpenFieldEditor(const ComponentFieldInfo& fieldInfo, void* instance, const std::string& title);

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
	std::vector<std::unique_ptr<FieldEditor>> nestedEditors;

	static UR::Class* GetPointerClass(const std::string& typeName);
	void ReadFieldValue();
	void WriteFieldValue();
	void RenderIntEditor();
	void RenderFloatEditor();
	void RenderBoolEditor();
	void RenderStringEditor();

	void RenderNestedInspector();

	void RenderNestedFieldValue(const UR::Field* field, void* instance) const;

	static void ReadValueFromAddress(void* addr, const std::string& typeName, FieldEditorState& state);
	static void WriteValueToAddress(void* addr, const std::string& typeName, const FieldEditorState& state);
	static std::string FormatFieldValue(void* addr, const std::string& typeName);
};
