#pragma once
#include "features/features.h"
#include "features/inspector/field_editor.h"

struct ComponentPropertyInfo final
{
	std::string name;
	std::string typeName;
	void* getterHandle = nullptr;
	void* setterHandle = nullptr;
	bool canRead = false;
	bool canWrite = false;
	EditableType editableType = EditableType::None;
};

struct ComponentMethodInfo final
{
	std::string name;
	std::string returnTypeName;
	std::vector<std::pair<std::string, std::string>> parameters;
	std::vector<EditableType> parameterEditableTypes;
	int flags = 0;
	bool isStatic = false;
	bool isVirtual = false;
	void* methodHandle = nullptr;
};

struct MethodInvokeState
{
	bool showPopup = false;
	UT::Component* targetComponent = nullptr;
	ComponentMethodInfo method;
	std::vector<std::string> parameterValues;
	std::string resultText;
	bool hasResult = false;
};

struct HierarchyNode final
{
	UT::GameObject* gameObject = nullptr;
	UT::Transform* transform = nullptr;
	std::string name;
	std::vector<HierarchyNode> children;
	bool pendingExpand = false;
	bool pendingExpandValue = false;
};

struct InspectedObjectTab final
{
	UT::GameObject* gameObject = nullptr;
	std::string tabName;
	std::string objectPath;
	bool isActive = false;

	std::vector<UT::Component*> cachedComponents;
	std::vector<std::string> cachedComponentNames;
	std::vector<std::vector<ComponentFieldInfo>> cachedComponentFields;
	std::vector<std::vector<ComponentPropertyInfo>> cachedComponentProperties;
	std::vector<std::vector<ComponentMethodInfo>> cachedComponentMethods;

	char componentSearchBuffer[256] = {};
	std::vector<std::array<char, 256>> fieldSearchBuffers;
	std::vector<std::array<char, 256>> propertySearchBuffers;
	std::vector<std::array<char, 256>> methodSearchBuffers;

	bool filterEditableOnly = false;
	bool filterStaticOnly = false;
	bool filterInstanceOnly = false;

	bool showWorldTransform = true;
	bool showLocalTransform = true;
};

class Inspector final : public IFeature
{
public:
	void Update(float deltaTime) override;
	void Render() override;

private:
	bool showDetailsWindow = false;
	std::vector<HierarchyNode> rootNodes;
	char searchBuffer[256] = {};

	std::vector<InspectedObjectTab> openTabs;
	int activeTabIndex = -1;
	static constexpr int maxTabs = 10;

	std::deque<UT::GameObject*> recentSelections;
	std::vector<UT::GameObject*> pinnedObjects;

	bool objectPickerActive = false;

	MethodInvokeState invokeState;
	std::unique_ptr<FieldEditor> fieldEditor;

	void RefreshHierarchy();
	void BuildHierarchyNode(HierarchyNode& node, UT::Transform* transform);
	void RenderHierarchyNode(HierarchyNode& node, int depth = 0);
	bool NodeMatchesSearch(const HierarchyNode& node) const;
	void SetAllNodesExpanded(std::vector<HierarchyNode>& nodes, bool expanded);

	void OpenObjectInNewTab(UT::GameObject* obj);
	void CloseTab(int tabIndex);
	void SwitchToTab(int tabIndex);
	InspectedObjectTab* GetActiveTab();
	int FindTabForObject(const UT::GameObject* obj) const;
	void AddToRecentSelections(UT::GameObject* obj);
	void PinObject(UT::GameObject* obj);
	void UnpinObject(UT::GameObject* obj);
	bool IsObjectPinned(UT::GameObject* obj);

	void RefreshTabData(InspectedObjectTab& tab) const;

	void RenderDetailsWindow();
	void RenderTabBar();
	void RenderTabContent(InspectedObjectTab& tab);
	void RenderTransformSection(UT::Transform* transform, InspectedObjectTab& tab) const;
	void RenderComponentsSection(InspectedObjectTab& tab);
	void RenderFieldsSection(UT::Component* component, const std::vector<ComponentFieldInfo>& fields, InspectedObjectTab& tab, size_t componentIndex);
	void RenderPropertiesSection(UT::Component* component, const std::vector<ComponentPropertyInfo>& properties, InspectedObjectTab& tab, size_t componentIndex) const;
	void RenderMethodsSection(UT::Component* component, const std::vector<ComponentMethodInfo>& methods, InspectedObjectTab& tab, size_t componentIndex);
	void RenderMethodInvokePopup();
	void DrawSelectedObjectBoundingBox() const;
	void ProcessObjectPicker();

	bool PassesComponentFilter(const std::string& componentName, const char* pSearchBuffer) const;
	bool PassesFieldFilter(const ComponentFieldInfo& field, const char* pSearchBuffer, bool editableOnly, bool staticOnly, bool instanceOnly) const;
	bool PassesPropertyFilter(const ComponentPropertyInfo& prop, const char* pSearchBuffer, bool editableOnly) const;
	bool PassesMethodFilter(const ComponentMethodInfo& method, const char* pSearchBuffer, bool staticOnly, bool instanceOnly) const;

	std::string GetComponentTypeName(UT::Component* component) const;
	std::string GetComponentFullTypeName(UT::Component* component) const;
	std::vector<ComponentFieldInfo> GetComponentFields(UT::Component* component) const;
	std::vector<ComponentPropertyInfo> GetComponentProperties(UT::Component* component) const;
	std::vector<ComponentMethodInfo> GetComponentMethods(UT::Component* component) const;

	std::string BuildObjectPath(UT::Transform* transform) const;
	void RenderEditableField(UT::Component* component, const ComponentFieldInfo& field) const;
	void RenderEditableProperty(UT::Component* component, const ComponentPropertyInfo& prop) const;
	void* InvokeMethod(UT::Component* component, const ComponentMethodInfo& method, const std::vector<std::string>& paramValues) const;

};