#pragma once
#include "game/core/core.h"

struct AssemblyClassInfo
{
    std::string name;
    std::string fullName;
    std::string parent;
    UR::Class* classHandle = nullptr;
    int fieldCount = 0;
    int methodCount = 0;
};

struct NamespaceGroup
{
    std::string name;
    std::vector<AssemblyClassInfo> classes;
    bool isExpanded = false;
};

struct AssemblyInfo
{
    std::string name;
    std::string fileName;
    UR::Assembly* assemblyHandle = nullptr;
    int classCount = 0;
    bool isExpanded = false;
    std::vector<NamespaceGroup> namespaces;
};

struct AssemblyExplorer final : Core::Feature
{
    void Update(float deltaTime) override;
    void Render() override;

private:
    bool dataLoaded = false;
    
    std::vector<AssemblyInfo> assemblies;
    AssemblyInfo* selectedAssembly = nullptr;
    NamespaceGroup* selectedNamespace = nullptr;
    AssemblyClassInfo* selectedClass = nullptr;
    
    char assemblySearchBuffer[256] = {};
    char classSearchBuffer[256] = {};
    
    bool showDetailsPanel = true;
    bool groupByNamespace = true;
    
    float assemblyPanelWidth = 250.0f;
    float classPanelWidth = 300.0f;
    
    void LoadAssemblyData();
    void RefreshAssemblyData();
    
    void RenderAssemblyExplorerWindow();
    void RenderAssemblyListPanel();
    void RenderClassListPanel();
    void RenderClassDetailsPanel();
    void RenderDivider(const char* id, float& widthToAdjust, float height);
    
    void RenderAssemblyNode(AssemblyInfo& assembly);
    void RenderNamespaceNode(NamespaceGroup& ns);
    void RenderClassNode(AssemblyClassInfo& classInfo);
    
    void SelectAssembly(AssemblyInfo* assembly);
    void SelectClass(AssemblyClassInfo* classInfo);
    
    std::string FormatClassName(const std::string& name) const;
    std::string FormatNamespaceName(const std::string& name) const;
    ImVec4 GetClassColor(const AssemblyClassInfo& classInfo) const;
};
