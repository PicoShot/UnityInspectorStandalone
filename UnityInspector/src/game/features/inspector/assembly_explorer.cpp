#include "pch.h"
#include "assembly_explorer.h"

void AssemblyExplorer::Update(const float deltaTime)
{
    const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer] = Core::config->inspector;
    if (!Enabled) return;
    
    if (!dataLoaded && !UR::assembly.empty())
    {
        LoadAssemblyData();
        dataLoaded = true;
    }
}

void AssemblyExplorer::Render()
{
    if (!Core::config->inspector.Enabled) return;
    
    RenderAssemblyExplorerWindow();
}

void AssemblyExplorer::LoadAssemblyData()
{
    assemblies.clear();
    
    for (const auto& assembly : UR::assembly)
    {
        if (!assembly) continue;
        
        AssemblyInfo info;
        info.name = assembly->name;
        info.fileName = assembly->file;
        info.assemblyHandle = assembly.get();
        info.classCount = static_cast<int>(assembly->classes.size());
        
        std::unordered_map<std::string, std::vector<AssemblyClassInfo>> nsMap;
        
        for (const auto& klass : assembly->classes)
        {
            if (!klass) continue;
            
            AssemblyClassInfo classInfo;
            classInfo.name = klass->name;
            classInfo.parent = klass->parent;
            classInfo.classHandle = klass.get();
            classInfo.fieldCount = static_cast<int>(klass->fields.size());
            classInfo.methodCount = static_cast<int>(klass->methods.size());
            
            if (!klass->namespaze.empty())
                classInfo.fullName = klass->namespaze + "." + klass->name;
            else
                classInfo.fullName = klass->name;
            
            std::string nsName = klass->namespaze.empty() ? "<Global Namespace>" : klass->namespaze;
            nsMap[nsName].push_back(std::move(classInfo));
        }
        
        for (auto& [nsName, classes] : nsMap)
        {
            NamespaceGroup nsGroup;
            nsGroup.name = nsName;
            nsGroup.classes = std::move(classes);
            
            std::ranges::sort(nsGroup.classes, [](const auto& a, const auto& b) {
                return a.name < b.name;
            });
            
            info.namespaces.push_back(std::move(nsGroup));
        }
        
        std::ranges::sort(info.namespaces, [](const auto& a, const auto& b) {
            if (a.name == "<Global Namespace>") return false;
            if (b.name == "<Global Namespace>") return true;
            return a.name < b.name;
        });
        
        assemblies.push_back(std::move(info));
    }
    
    std::ranges::sort(assemblies, [](const auto& a, const auto& b) {
        return a.name < b.name;
    });
}

void AssemblyExplorer::RefreshAssemblyData()
{
    dataLoaded = false;
    selectedAssembly = nullptr;
    selectedNamespace = nullptr;
    selectedClass = nullptr;
    LoadAssemblyData();
}

void AssemblyExplorer::RenderAssemblyExplorerWindow()
{
    if (!Core::config->inspector.ShowAssemblyExplorer) return;
    
    UR::ThreadAttach();
    
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Assembly Explorer", &Core::config->inspector.ShowAssemblyExplorer))
    {
        if (ImGui::Button("Refresh"))
        {
            RefreshAssemblyData();
        }
        
        ImGui::SameLine();
        ImGui::Checkbox("Group by Namespace", &groupByNamespace);
        
        ImGui::SameLine();
        ImGui::Checkbox("Show Details", &showDetailsPanel);
        
        ImGui::Separator();
        
        float availableHeight = ImGui::GetContentRegionAvail().y;
        float availableWidth = ImGui::GetContentRegionAvail().x;
        
        const float minPanelWidth = 150.0f;
        if (assemblyPanelWidth < minPanelWidth) assemblyPanelWidth = minPanelWidth;
        if (classPanelWidth < minPanelWidth) classPanelWidth = minPanelWidth;
        
        float detailsWidth = showDetailsPanel ? 
            std::max(availableWidth - assemblyPanelWidth - classPanelWidth - 16.0f, minPanelWidth) : 0.0f;
        
        ImGui::BeginChild("AssemblyExplorerMain", ImVec2(0, availableHeight), false, ImGuiWindowFlags_NoScrollbar);
        
        RenderAssemblyListPanel();
        
        ImGui::SameLine();
        RenderDivider("AssemblyClassDivider", assemblyPanelWidth, availableHeight);
        
        ImGui::SameLine();
        RenderClassListPanel();
        
        if (showDetailsPanel)
        {
            ImGui::SameLine();
            RenderDivider("ClassDetailsDivider", classPanelWidth, availableHeight);
            
            ImGui::SameLine();
            RenderClassDetailsPanel();
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}

void AssemblyExplorer::RenderDivider(const char* id, float& widthToAdjust, float height)
{
    ImGui::PushID(id);
    
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(8.0f, height);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    ImGui::InvisibleButton("divider", size);
    
    bool isHovered = ImGui::IsItemHovered();
    bool isActive = ImGui::IsItemActive();
    
    if (isHovered || isActive)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    
    ImU32 color = isActive ? IM_COL32(100, 150, 255, 255) : 
                  (isHovered ? IM_COL32(150, 150, 150, 255) : IM_COL32(100, 100, 100, 100));
    
    drawList->AddLine(
        ImVec2(pos.x + 3.5f, pos.y),
        ImVec2(pos.x + 3.5f, pos.y + height),
        color, 2.0f);
    
    if (isActive)
    {
        float delta = ImGui::GetIO().MouseDelta.x;
        widthToAdjust += delta;
    }
    
    ImGui::PopID();
}

void AssemblyExplorer::RenderAssemblyListPanel()
{
    ImGui::BeginChild("AssemblyList", ImVec2(assemblyPanelWidth, 0), true);
    
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##AssemblySearch", "Search assemblies...", assemblySearchBuffer, sizeof(assemblySearchBuffer));
    
    ImGui::Separator();
    
    int visibleCount = 0;
    for (const auto& assembly : assemblies)
    {
        if (assemblySearchBuffer[0] == '\0' || 
            assembly.name.find(assemblySearchBuffer) != std::string::npos)
        {
            visibleCount++;
        }
    }
    
    ImGui::TextDisabled("Assemblies: %d", visibleCount);
    ImGui::Spacing();
    
    ImGui::BeginChild("AssemblyListScroll", ImVec2(0, 0), false);
    
    for (auto& assembly : assemblies)
    {
        if (assemblySearchBuffer[0] != '\0' && 
            assembly.name.find(assemblySearchBuffer) == std::string::npos)
        {
            continue;
        }
        
        RenderAssemblyNode(assembly);
    }
    
    ImGui::EndChild();
    
    ImGui::EndChild();
}

void AssemblyExplorer::RenderAssemblyNode(AssemblyInfo& assembly)
{
    ImGui::PushID(&assembly);
    
    bool isSelected = (selectedAssembly == &assembly);
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | 
                               ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_DefaultOpen;
    
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    
    const char* icon = isSelected ? " > " : "   ";
    
    std::string label = icon + assembly.name;
    
    bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);
    
    if (ImGui::IsItemClicked())
    {
        SelectAssembly(&assembly);
    }
    
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("File: %s", assembly.fileName.c_str());
        ImGui::Text("Classes: %d", assembly.classCount);
        ImGui::EndTooltip();
    }
    
    if (nodeOpen)
    {
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void AssemblyExplorer::RenderClassListPanel()
{
    ImGui::BeginChild("ClassList", ImVec2(classPanelWidth, 0), true);
    
    if (!selectedAssembly)
    {
        ImGui::TextDisabled("Select an assembly to view classes");
        ImGui::EndChild();
        return;
    }
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("%s", selectedAssembly->name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##ClassSearch", "Search classes...", classSearchBuffer, sizeof(classSearchBuffer));
    
    ImGui::Separator();
    
    int totalVisibleClasses = 0;
    int totalClasses = 0;
    
    for (auto& ns : selectedAssembly->namespaces)
    {
        totalClasses += static_cast<int>(ns.classes.size());
        
        if (classSearchBuffer[0] == '\0')
        {
            totalVisibleClasses += static_cast<int>(ns.classes.size());
        }
        else
        {
            for (const auto& klass : ns.classes)
            {
                if (klass.name.find(classSearchBuffer) != std::string::npos ||
                    klass.fullName.find(classSearchBuffer) != std::string::npos)
                {
                    totalVisibleClasses++;
                }
            }
        }
    }
    
    ImGui::TextDisabled("Classes: %d/%d", totalVisibleClasses, totalClasses);
    ImGui::Spacing();
    
    ImGui::BeginChild("ClassListScroll", ImVec2(0, 0), false);
    
    for (auto& ns : selectedAssembly->namespaces)
    {
        if (groupByNamespace)
        {
            bool hasMatch = false;
            if (classSearchBuffer[0] == '\0')
            {
                hasMatch = true;
            }
            else
            {
                for (const auto& klass : ns.classes)
                {
                    if (klass.name.find(classSearchBuffer) != std::string::npos ||
                        klass.fullName.find(classSearchBuffer) != std::string::npos)
                    {
                        hasMatch = true;
                        break;
                    }
                }
            }
            
            if (!hasMatch) continue;
            
            RenderNamespaceNode(ns);
        }
        else
        {
            for (auto& klass : ns.classes)
            {
                if (classSearchBuffer[0] != '\0' &&
                    klass.name.find(classSearchBuffer) == std::string::npos &&
                    klass.fullName.find(classSearchBuffer) == std::string::npos)
                {
                    continue;
                }
                
                RenderClassNode(klass);
            }
        }
    }
    
    ImGui::EndChild();
    
    ImGui::EndChild();
}

void AssemblyExplorer::RenderNamespaceNode(NamespaceGroup& ns)
{
    ImGui::PushID(&ns);
    
    bool isSelected = (selectedNamespace == &ns);
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | 
                               ImGuiTreeNodeFlags_OpenOnArrow;
    
    if (ns.isExpanded) flags |= ImGuiTreeNodeFlags_DefaultOpen;
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    
    const char* icon = (ns.name == "<Global Namespace>") ? "<> " : "{} ";
    
    std::string label = icon + FormatNamespaceName(ns.name) + " (" + std::to_string(ns.classes.size()) + ")";
    
    bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), flags);
    ns.isExpanded = nodeOpen;
    
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        selectedNamespace = &ns;
    }
    
    if (nodeOpen)
    {
        for (auto& klass : ns.classes)
        {
            if (classSearchBuffer[0] != '\0' &&
                klass.name.find(classSearchBuffer) == std::string::npos &&
                klass.fullName.find(classSearchBuffer) == std::string::npos)
            {
                continue;
            }
            
            RenderClassNode(klass);
        }
        
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void AssemblyExplorer::RenderClassNode(AssemblyClassInfo& classInfo)
{
    ImGui::PushID(&classInfo);
    
    bool isSelected = (selectedClass == &classInfo);
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | 
                               ImGuiTreeNodeFlags_Leaf |
                               ImGuiTreeNodeFlags_NoTreePushOnOpen;
    
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    
    ImVec4 color = GetClassColor(classInfo);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    
    std::string label = "C " + classInfo.name;
    ImGui::TreeNodeEx(label.c_str(), flags);
    
    ImGui::PopStyleColor();
    
    if (ImGui::IsItemClicked())
    {
        SelectClass(&classInfo);
    }
    
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Full Name: %s", classInfo.fullName.c_str());
        if (!classInfo.parent.empty())
            ImGui::Text("Parent: %s", classInfo.parent.c_str());
        ImGui::Text("Fields: %d | Methods: %d", classInfo.fieldCount, classInfo.methodCount);
        ImGui::EndTooltip();
    }
    
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Copy Full Name"))
        {
            ImGui::SetClipboardText(classInfo.fullName.c_str());
        }
        if (ImGui::MenuItem("Copy Class Name"))
        {
            ImGui::SetClipboardText(classInfo.name.c_str());
        }
        ImGui::EndPopup();
    }
    
    ImGui::PopID();
}

void AssemblyExplorer::RenderClassDetailsPanel()
{
    ImGui::BeginChild("ClassDetails", ImVec2(0, 0), true);
    
    if (!selectedClass || !selectedClass->classHandle)
    {
        ImGui::TextDisabled("Select a class to view details");
        ImGui::EndChild();
        return;
    }
    
    UR::Class* klass = selectedClass->classHandle;
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
    ImGui::Text("%s", selectedClass->name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    ImGui::TextDisabled("Full Name:");
    ImGui::Text("%s", selectedClass->fullName.c_str());
    
    if (!selectedClass->parent.empty())
    {
        ImGui::TextDisabled("Parent Class:");
        ImGui::Text("%s", selectedClass->parent.c_str());
    }
    
    ImGui::Separator();
    
    ImGui::Columns(2, "ClassStats", false);
    
    ImGui::TextDisabled("Fields:");
    ImGui::Text("%d", selectedClass->fieldCount);
    ImGui::NextColumn();
    
    ImGui::TextDisabled("Methods:");
    ImGui::Text("%d", selectedClass->methodCount);
    ImGui::NextColumn();
    
    ImGui::Columns(1);
    
    ImGui::Separator();
    
    if (!klass->fields.empty())
    {
        if (ImGui::CollapsingHeader("Fields", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();
            
            if (ImGui::BeginTable("FieldsTable", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                
                for (const auto& field : klass->fields)
                {
                    if (!field) continue;
                    
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
                    std::string typeName = field->type->name;
                    if (typeName.length() > 50) typeName = typeName.substr(0, 47) + "...";
                    ImGui::TextUnformatted(typeName.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(2);
                    if (!field->static_field)
                        ImGui::TextDisabled("0x%X", field->offset);
                    else
                        ImGui::TextDisabled("[S]");
                }
                
                ImGui::EndTable();
            }
            
            ImGui::Unindent();
        }
    }
    
    if (!klass->methods.empty())
    {
        if (ImGui::CollapsingHeader("Methods"))
        {
            ImGui::Indent();
            
            if (ImGui::BeginTable("MethodsTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 180.0f);
                ImGui::TableSetupColumn("Return Type", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Parameters", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                
                for (const auto& method : klass->methods)
                {
                    if (!method) continue;
                    
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 color = method->static_function ? 
                        ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                        ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(method->name.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
                    std::string retType = method->return_type->name;
                    if (retType.length() > 40) retType = retType.substr(0, 37) + "...";
                    ImGui::Text("-> %s", retType.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(2);
                    if (!method->args.empty())
                    {
                        std::string params;
                        for (const auto& arg : method->args)
                        {
                            if (!arg) continue;
                            if (!params.empty()) params += ", ";
                            params += arg->pType->name + " " + arg->name;
                        }
                        if (params.length() > 60) params = params.substr(0, 57) + "...";
                        ImGui::TextDisabled("(%s)", params.c_str());
                    }
                    else
                    {
                        ImGui::TextDisabled("()");
                    }
                    
                    ImGui::TableSetColumnIndex(3);
                    std::string flags;
                    if (method->static_function) flags += "S";
                    if (!flags.empty())
                        ImGui::TextDisabled("[%s]", flags.c_str());
                }
                
                ImGui::EndTable();
            }
            
            ImGui::Unindent();
        }
    }
    
    ImGui::EndChild();
}

void AssemblyExplorer::SelectAssembly(AssemblyInfo* assembly)
{
    selectedAssembly = assembly;
    selectedNamespace = nullptr;
    selectedClass = nullptr;
    
    classSearchBuffer[0] = '\0';
}

void AssemblyExplorer::SelectClass(AssemblyClassInfo* classInfo)
{
    selectedClass = classInfo;
}

std::string AssemblyExplorer::FormatClassName(const std::string& name) const
{
    return name;
}

std::string AssemblyExplorer::FormatNamespaceName(const std::string& name) const
{
    if (name == "<Global Namespace>")
        return "Global";
    
    if (name.length() > 40)
        return name.substr(0, 37) + "...";
    
    return name;
}

ImVec4 AssemblyExplorer::GetClassColor(const AssemblyClassInfo& classInfo) const
{
    if (classInfo.parent == "MonoBehaviour")
        return ImVec4(0.4f, 0.8f, 0.4f, 1.0f);  // Green for MonoBehaviour
    if (classInfo.parent == "ScriptableObject")
        return ImVec4(0.8f, 0.6f, 0.4f, 1.0f);  // Orange for ScriptableObject
    if (classInfo.parent == "Component")
        return ImVec4(0.4f, 0.6f, 0.8f, 1.0f);  // Blue for Component
    if (classInfo.parent == "Object")
        return ImVec4(0.8f, 0.8f, 0.4f, 1.0f);  // Yellow for Object
    if (!classInfo.parent.empty())
        return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray for others with parent
    
    return ImVec4(0.9f, 0.9f, 0.9f, 1.0f);      // White for base classes
}
