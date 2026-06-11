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
	void* targetInstance = nullptr;
	ComponentMethodInfo method;
	std::vector<std::string> parameterValues;
	std::string resultText;
	bool hasResult = false;
	void* resultPointer = nullptr;
	EditableType resultEditableType = EditableType::None;
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

struct StaticInstanceNode
{
	void* instance = nullptr;
	void* typeClassHandle = nullptr;
	std::string fullName;
	std::string name;
};

struct InspectionTarget
{
	UT::GameObject* gameObject = nullptr;
	void* instance = nullptr;
	void* classHandle = nullptr;
	std::string name;

	std::vector<UT::Component*> cachedComponents;
	std::vector<std::string> cachedComponentNames;
	std::vector<std::vector<ComponentFieldInfo>> cachedComponentFields;
	std::vector<std::vector<ComponentPropertyInfo>> cachedComponentProperties;
	std::vector<std::vector<ComponentMethodInfo>> cachedComponentMethods;

	char componentSearchBuffer[256] = {};
	std::vector<std::array<char, 256>> fieldSearchBuffers;
	std::vector<std::array<char, 256>> propertySearchBuffers;
	std::vector<std::array<char, 256>> methodSearchBuffers;
};

struct InspectedObjectTab final
{
	UT::GameObject* gameObject = nullptr;
	std::string tabName;
	std::string objectPath;
	bool isActive = false;

	std::vector<InspectionTarget> navigationStack;

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

	static Inspector* GetInstance() { return s_Instance; }
	void InspectInstance(void* instance, void* classHandle, const std::string& name);

private:
	static inline Inspector* s_Instance = nullptr;
	bool showDetailsWindow = false;
	std::vector<HierarchyNode> rootNodes;
	char searchBuffer[256] = {};

	std::vector<StaticInstanceNode> staticInstances;
	char staticSearchBuffer[256] = {};
	bool hasScannedStatic = false;

	std::vector<InspectedObjectTab> openTabs;
	int activeTabIndex = -1;
	bool pendingTabSwitch = false;
	static constexpr int maxTabs = 10;

	std::deque<UT::GameObject*> recentSelections;
	std::vector<UT::GameObject*> pinnedObjects;

	bool objectPickerActive = false;

	MethodInvokeState invokeState;
	std::unique_ptr<FieldEditor> fieldEditor;

	void RefreshHierarchy();
	void BuildHierarchyNode(HierarchyNode& node, UT::Transform* transform);
	void RenderHierarchyNode(HierarchyNode& node, std::string_view lowerSearch = "", int depth = 0);
	bool NodeMatchesSearch(const HierarchyNode& node, std::string_view lowerSearch) const;
	void SetAllNodesExpanded(std::vector<HierarchyNode>& nodes, bool expanded);

	void OpenObjectInNewTab(UT::GameObject* obj);
	void OpenStaticInstanceInNewTab(const StaticInstanceNode& node);
	void ScanStaticClasses();
	void CloseTab(int tabIndex);
	void SwitchToTab(int tabIndex);
	InspectedObjectTab* GetActiveTab() const;
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
	void RenderComponentsSection(InspectionTarget& target, InspectedObjectTab& tab);
	void RenderFieldsSection(void* instance, const std::vector<ComponentFieldInfo>& fields, InspectionTarget& target,
	                         InspectedObjectTab& tab, size_t componentIndex) const;
	void RenderPropertiesSection(void* instance, const std::vector<ComponentPropertyInfo>& properties,
	                             InspectionTarget& target, InspectedObjectTab& tab, size_t componentIndex) const;
	void RenderMethodsSection(void* instance, const std::vector<ComponentMethodInfo>& methods, InspectionTarget& target,
	                          InspectedObjectTab& tab, size_t componentIndex);
	void RenderMethodInvokePopup();
	void DrawSelectedObjectBoundingBox() const;
	void ProcessObjectPicker();

	bool PassesComponentFilter(const std::string& componentName, std::string_view lowerSearch) const;
	bool PassesFieldFilter(const ComponentFieldInfo& field, std::string_view lowerSearch, bool editableOnly,
	                       bool staticOnly, bool instanceOnly) const;
	bool PassesPropertyFilter(const ComponentPropertyInfo& prop, std::string_view lowerSearch, bool editableOnly) const;
	bool PassesMethodFilter(const ComponentMethodInfo& method, std::string_view lowerSearch, bool staticOnly,
	                        bool instanceOnly) const;

	std::string GetComponentTypeName(UT::Component* component) const;
	std::vector<ComponentFieldInfo> GetObjectFields(void* obj, void* klass) const;
	std::vector<ComponentFieldInfo> GetComponentFields(UT::Component* component) const;
	std::vector<ComponentPropertyInfo> GetObjectProperties(void* obj, void* klass) const;
	std::vector<ComponentPropertyInfo> GetComponentProperties(UT::Component* component) const;
	std::vector<ComponentMethodInfo> GetObjectMethods(void* obj, void* klass) const;
	std::vector<ComponentMethodInfo> GetComponentMethods(UT::Component* component) const;

	std::string BuildObjectPath(UT::Transform* transform) const;
	void RenderEditableField(void* instance, const ComponentFieldInfo& field, float itemWidth = -1.0f) const;
	void RenderEditableProperty(void* instance, const ComponentPropertyInfo& prop) const;
	void* InvokeMethod(void* instance, const ComponentMethodInfo& method,
	                   const std::vector<std::string>& paramValues) const;
};
