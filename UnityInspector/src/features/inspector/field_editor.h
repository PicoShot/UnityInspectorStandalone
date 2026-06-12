#pragma once
#include "features/features.h"
#include "features/inspector/editable_type.h"

EditableType DetermineEditableType(std::string_view typeName, std::string* enumTypeNameOut = nullptr);

std::vector<std::pair<std::string, int>> GetEnumValues(std::string_view enumTypeName);

bool IsUInt64WrappingType(std::string_view typeName);

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
	std::string lastTitle;
	int windowId = 0;

	UR::Field* targetField = nullptr;
	void* targetInstance = nullptr;

	UR::Class* nestedClass = nullptr;
	void* nestedInstance = nullptr;

	std::unique_ptr<UR::Field> ownedField;

	bool isValueType = false;

	char stringBuffer[1024] = {};
	long long intValue = 0;
	float floatValue = 0.0f;
	bool boolValue = false;

	std::function<void()> onValueChanged;
};

struct FieldEditor final
{
	FieldEditor();
	~FieldEditor();

	void OpenFieldEditor(UR::Field* field, void* instance, std::string_view title);
	void OpenFieldEditor(const ComponentFieldInfo& fieldInfo, void* instance, std::string_view title);

	void Render();

	bool IsOpen() const { return state.showWindow; }

	void Close();

	static bool IsEditableType(std::string_view typeName);
	static bool IsIntegerType(std::string_view typeName);
	static bool IsFloatType(std::string_view typeName);
	static bool IsBoolType(std::string_view typeName);
	static bool IsStringType(std::string_view typeName);
	static bool IsPointerType(std::string_view typeName);

	static std::string FormatFieldValue(void* addr, std::string_view typeName);

private:
	FieldEditorState state;
	std::vector<std::unique_ptr<FieldEditor>> nestedEditors;
	std::vector<std::unique_ptr<FieldEditor>> pendingEditors;

	void TakePendingEditors(std::vector<std::unique_ptr<FieldEditor>>& out);

	static UR::Class* GetPointerClass(std::string_view typeName);
	void ReadFieldValue();
	void WriteFieldValue();
	void RenderIntEditor(std::string_view typeName);
	void RenderFloatEditor();
	void RenderBoolEditor();
	void RenderStringEditor();

	void RenderNestedInspector();

	void RenderNestedFieldValue(const UR::Field* field, void* instance) const;

	static void ReadValueFromAddress(void* addr, std::string_view typeName, FieldEditorState& state);
	static void WriteValueToAddress(void* addr, std::string_view typeName, const FieldEditorState& state);
};
