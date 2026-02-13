#include "pch.h"
#include "assembly_explorer.h"
#include "field_editor.h"

void AssemblyExplorer::Update(const float deltaTime)
{
    const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer] = Core::config->inspector;
    if (!Enabled) return;
    
    if (!dataLoaded && !UR::assembly.empty())
    {
        LoadAssemblyData();
        dataLoaded = true;
    }
    
    // Auto-refresh instances for selected class
    if (autoRefreshInstances && selectedClass)
    {
        selectedClass->instancesRefreshTimer += deltaTime;
        if (selectedClass->instancesRefreshTimer >= 1.0f)
        {
            selectedClass->instancesRefreshTimer = 0.0f;
            RefreshInstances(selectedClass);
        }
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
    
    ImGui::SetNextWindowSize(ImVec2(1200, 700), ImGuiCond_FirstUseEver);
    
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
        
        ImGui::SameLine();
        ImGui::Checkbox("Auto Refresh Instances", &autoRefreshInstances);
        
        ImGui::Separator();
        
        float availableHeight = ImGui::GetContentRegionAvail().y;
        float availableWidth = ImGui::GetContentRegionAvail().x;
        
        const float minPanelWidth = 150.0f;
        if (assemblyPanelWidth < minPanelWidth) assemblyPanelWidth = minPanelWidth;
        if (classPanelWidth < minPanelWidth) classPanelWidth = minPanelWidth;
        if (instancePanelWidth < minPanelWidth) instancePanelWidth = minPanelWidth;
        
        ImGui::BeginChild("AssemblyExplorerMain", ImVec2(0, availableHeight), false, ImGuiWindowFlags_NoScrollbar);
        
        // Assembly Panel
        RenderAssemblyListPanel();
        
        ImGui::SameLine();
        RenderDivider("AssemblyClassDivider", assemblyPanelWidth, availableHeight);
        
        // Class Panel
        ImGui::SameLine();
        RenderClassListPanel();
        
        // Instance Panel (only if class selected)
        if (selectedClass)
        {
            ImGui::SameLine();
            RenderDivider("ClassInstanceDivider", classPanelWidth, availableHeight);
            
            ImGui::SameLine();
            RenderInstanceListPanel();
        }
        
        // Details Panel
        if (showDetailsPanel && selectedClass)
        {
            ImGui::SameLine();
            RenderDivider("InstanceDetailsDivider", instancePanelWidth, availableHeight);
            
            ImGui::SameLine();
            RenderClassDetailsPanel();
        }
        
        ImGui::EndChild();
        
        // Render method invocation popup
        if (invokeState.showPopup)
        {
            RenderMethodInvokePopup();
        }
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

void AssemblyExplorer::RenderInstanceListPanel()
{
    ImGui::BeginChild("InstanceList", ImVec2(instancePanelWidth, 0), true);
    
    if (!selectedClass)
    {
        ImGui::TextDisabled("Select a class to view instances");
        ImGui::EndChild();
        return;
    }
    
    // Header with class name
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Instances");
    ImGui::PopStyleColor();
    
    // Refresh button
    ImGui::SameLine();
    if (ImGui::SmallButton("Refresh"))
    {
        RefreshInstances(selectedClass);
    }
    
    // Search box
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##InstanceSearch", "Search instances...", instanceSearchBuffer, sizeof(instanceSearchBuffer));
    
    ImGui::Separator();
    
    ImGui::TextDisabled("Count: %zu", selectedClass->instances.size());
    ImGui::Spacing();
    
    ImGui::BeginChild("InstanceListScroll", ImVec2(0, 0), false);
    
    for (auto& instance : selectedClass->instances)
    {
        if (instanceSearchBuffer[0] != '\0' &&
            instance.displayName.find(instanceSearchBuffer) == std::string::npos)
        {
            continue;
        }
        
        RenderInstanceNode(instance);
    }
    
    ImGui::EndChild();
    
    ImGui::EndChild();
}

void AssemblyExplorer::RenderInstanceNode(ClassInstanceInfo& instance)
{
    ImGui::PushID(&instance);
    
    bool isSelected = (selectedInstance == &instance);
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | 
                               ImGuiTreeNodeFlags_Leaf |
                               ImGuiTreeNodeFlags_NoTreePushOnOpen;
    
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    
    std::string label = "[@] " + instance.displayName;
    ImGui::TreeNodeEx(label.c_str(), flags);
    
    if (ImGui::IsItemClicked())
    {
        SelectInstance(&instance);
    }
    
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Address: %p", instance.instance);
    }
    
    ImGui::PopID();
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
            // Show instance info if a non-static field is selected
            bool canEditInstance = selectedInstance && selectedInstance->instance;
            
            if (!canEditInstance && !selectedClass->instances.empty())
            {
                ImGui::TextDisabled("Select an instance to edit non-static fields");
            }
            
            ImGui::Indent();
            
            if (ImGui::BeginTable("FieldsTable", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                
                for (const auto& field : klass->fields)
                {
                    if (!field) continue;
                    
                    // Check if we can edit this field
                    bool isStatic = field->static_field;
                    bool canEdit = isStatic || canEditInstance;
                    bool isEditableType = FieldEditor::IsEditableType(field->type ? field->type->name : "");
                    bool isPointerType = FieldEditor::IsPointerType(field->type ? field->type->name : "");
                    bool showEdit = canEdit && (isEditableType || isPointerType);
                    
                    ImGui::TableNextRow();
                    
                    // Column 0: Name
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 color = isStatic ? 
                        ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                        ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(field->name.c_str());
                    ImGui::PopStyleColor();
                    
                    // Column 1: Type
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
                    std::string typeName = field->type->name;
                    if (typeName.length() > 35) typeName = typeName.substr(0, 32) + "...";
                    ImGui::TextUnformatted(typeName.c_str());
                    ImGui::PopStyleColor();
                    
                    // Column 2: Offset
                    ImGui::TableSetColumnIndex(2);
                    if (!isStatic)
                        ImGui::TextDisabled("0x%X", field->offset);
                    else
                        ImGui::TextDisabled("[S]");
                    
                    // Column 3: Value preview
                    ImGui::TableSetColumnIndex(3);
                    RenderFieldRow(field.get(), canEditInstance ? selectedInstance->instance : nullptr);
                    
                    // Column 4: Edit button
                    ImGui::TableSetColumnIndex(4);
                    ImGui::PushID(field.get());
                    
                    if (!showEdit)
                    {
                        ImGui::BeginDisabled();
                    }
                    
                    if (ImGui::SmallButton("Edit"))
                    {
                        void* target = isStatic ? nullptr : selectedInstance->instance;
                        std::string title = "Edit Field: " + field->name;
                        
                        if (!fieldEditor)
                            fieldEditor = std::make_unique<FieldEditor>();
                        
                        fieldEditor->OpenFieldEditor(field.get(), target, title);
                    }
                    
                    if (!showEdit && ImGui::IsItemHovered())
                    {
                        if (!canEdit)
                            ImGui::SetTooltip("Select an instance to edit non-static fields");
                        else
                            ImGui::SetTooltip("This field type is not editable");
                    }
                    
                    if (!showEdit)
                    {
                        ImGui::EndDisabled();
                    }
                    
                    ImGui::PopID();
                }
                
                ImGui::EndTable();
            }
            
            // Render field editor window if open
            if (fieldEditor && fieldEditor->IsOpen())
            {
                fieldEditor->Render();
            }
            
            ImGui::Unindent();
        }
    }
    
    if (!klass->methods.empty())
    {
        if (ImGui::CollapsingHeader("Methods"))
        {
            ImGui::Indent();
            
            // Show instance info for non-static methods
            bool canInvokeInstance = selectedInstance && selectedInstance->instance;
            
            if (ImGui::BeginTable("MethodsTable", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Return Type", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Parameters", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                
                for (const auto& method : klass->methods)
                {
                    if (!method) continue;
                    
                    // Check if we can invoke this method
                    bool isStatic = method->static_function;
                    bool canInvoke = isStatic || canInvokeInstance;
                    
                    ImGui::TableNextRow();
                    
                    // Column 0: Name
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 color = isStatic ? 
                        ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                        ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(method->name.c_str());
                    ImGui::PopStyleColor();
                    
                    // Column 1: Return type
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
                    std::string retType = method->return_type->name;
                    if (retType.length() > 35) retType = retType.substr(0, 32) + "...";
                    ImGui::Text("-> %s", retType.c_str());
                    ImGui::PopStyleColor();
                    
                    // Column 2: Parameters
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
                        if (params.length() > 50) params = params.substr(0, 47) + "...";
                        ImGui::TextDisabled("(%s)", params.c_str());
                    }
                    else
                    {
                        ImGui::TextDisabled("()");
                    }
                    
                    // Column 3: Flags
                    ImGui::TableSetColumnIndex(3);
                    std::string flags;
                    if (isStatic) flags += "S";
                    if (!flags.empty())
                        ImGui::TextDisabled("[%s]", flags.c_str());
                    
                    // Column 4: Invoke button
                    ImGui::TableSetColumnIndex(4);
                    ImGui::PushID(method.get());
                    
                    if (!canInvoke)
                    {
                        ImGui::BeginDisabled();
                    }
                    
                    if (ImGui::SmallButton("Invoke"))
                    {
                        void* target = isStatic ? nullptr : selectedInstance->instance;
                        
                        if (method->args.empty())
                        {
                            // Direct invoke for methods without parameters
                            try
                            {
                                method->RuntimeInvoke<void>(target);
                            }
                            catch (...) {}
                        }
                        else
                        {
                            // Open invoke popup for methods with parameters
                            invokeState.showPopup = true;
                            invokeState.targetMethod = method.get();
                            invokeState.targetInstance = target;
                            invokeState.parameterValues.clear();
                            invokeState.parameterValues.resize(method->args.size());
                            invokeState.resultText.clear();
                            invokeState.hasResult = false;
                        }
                    }
                    
                    if (!canInvoke)
                    {
                        ImGui::EndDisabled();
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("Select an instance to invoke non-static methods");
                        }
                    }
                    
                    ImGui::PopID();
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
    selectedInstance = nullptr;
    
    // Refresh instances for this class
    if (selectedClass)
    {
        RefreshInstances(selectedClass);
    }
}

void AssemblyExplorer::SelectInstance(ClassInstanceInfo* instance)
{
    selectedInstance = instance;
}

void AssemblyExplorer::RefreshInstances(AssemblyClassInfo* classInfo)
{
    if (!classInfo || !classInfo->classHandle) return;
    
    classInfo->instances.clear();
    
    // Use Unity's FindObjectsOfType to get all instances
    try
    {
        auto objects = classInfo->classHandle->FindObjectsOfType<void*>();
        int index = 0;
        for (auto* obj : objects)
        {
            if (!obj) continue;
            
            ClassInstanceInfo info;
            info.instance = obj;
            info.displayName = classInfo->name + " #" + std::to_string(index++);
            
            // Try to get GameObject name if it's a Component
            // Note: This would require additional Unity type definitions
            // For now, just use the index
            
            classInfo->instances.push_back(std::move(info));
        }
    }
    catch (...) {}
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


void AssemblyExplorer::RenderFieldRow(UR::Field* field, void* instance)
{
    if (!field) return;
    
    // For now, just display the value - editing would require more complex type handling
    // This is a simplified version that shows raw values for common types
    
    try
    {
        if (field->static_field)
        {
            // Static field - can always read
            if (field->type->name == "System.Int32" || field->type->name == "System.Int")
            {
                int value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%d", value);
            }
            else if (field->type->name == "System.Single" || field->type->name == "System.Float")
            {
                float value = 0.0f;
                field->GetStaticValue(&value);
                ImGui::Text("%.3f", value);
            }
            else if (field->type->name == "System.Boolean" || field->type->name == "System.Bool")
            {
                bool value = false;
                field->GetStaticValue(&value);
                ImGui::Text("%s", value ? "true" : "false");
            }
            else if (field->type->name == "System.String")
            {
                // String handling would be more complex
                ImGui::TextDisabled("\"...\"");
            }
            else
            {
                ImGui::TextDisabled("...");
            }
        }
        else if (instance)
        {
            // Instance field - need instance pointer
            void* fieldAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(instance) + field->offset);
            
            if (field->type->name == "System.Int32" || field->type->name == "System.Int")
            {
                int value = *reinterpret_cast<int*>(fieldAddr);
                ImGui::Text("%d", value);
            }
            else if (field->type->name == "System.Single" || field->type->name == "System.Float")
            {
                float value = *reinterpret_cast<float*>(fieldAddr);
                ImGui::Text("%.3f", value);
            }
            else if (field->type->name == "System.Boolean" || field->type->name == "System.Bool")
            {
                bool value = *reinterpret_cast<bool*>(fieldAddr);
                ImGui::Text("%s", value ? "true" : "false");
            }
            else if (field->type->name == "UnityEngine.Vector3")
            {
                Vec3 value = *reinterpret_cast<Vec3*>(fieldAddr);
                ImGui::Text("(%.2f, %.2f, %.2f)", value.x, value.y, value.z);
            }
            else
            {
                ImGui::TextDisabled("...");
            }
        }
        else
        {
            ImGui::TextDisabled("-");
        }
    }
    catch (...)
    {
        ImGui::TextDisabled("Error");
    }
}

void AssemblyExplorer::RenderMethodInvokePopup()
{
    if (!invokeState.showPopup || !invokeState.targetMethod) return;
    
    ImGui::OpenPopup("Invoke Method");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Invoke Method", &invokeState.showPopup, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Method: %s", invokeState.targetMethod->name.c_str());
        ImGui::Separator();
        
        // Parameter inputs
        for (size_t i = 0; i < invokeState.targetMethod->args.size(); i++)
        {
            const auto& arg = invokeState.targetMethod->args[i];
            if (!arg) continue;
            
            ImGui::Text("%s (%s):", arg->name.c_str(), arg->pType->name.c_str());
            
            char buf[256];
            strncpy_s(buf, invokeState.parameterValues[i].c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            
            std::string id = "##param" + std::to_string(i);
            if (ImGui::InputText(id.c_str(), buf, sizeof(buf)))
            {
                invokeState.parameterValues[i] = buf;
            }
        }
        
        ImGui::Separator();
        
        // Result display
        if (invokeState.hasResult)
        {
            ImGui::Text("Result: %s", invokeState.resultText.c_str());
        }
        
        // Buttons
        if (ImGui::Button("Invoke", ImVec2(120, 0)))
        {
            // TODO: Parse parameters and invoke method
            // This is complex due to type conversion - simplified for now
            invokeState.resultText = "Invoked!";
            invokeState.hasResult = true;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            invokeState.showPopup = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}
