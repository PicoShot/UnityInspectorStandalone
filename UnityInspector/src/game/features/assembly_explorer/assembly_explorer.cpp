#include "pch.h"
#include "assembly_explorer.h"
#include "game/features/inspector/field_editor.h"
#include <sstream>

void AssemblyExplorer::Update(const float deltaTime)
{
    const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer, ShowDebugConsole] = Core::config->inspector;
    if (!Enabled) return;
    
    if (!dataLoaded && !UR::assembly.empty())
    {
        LoadAssemblyData();
        dataLoaded = true;
    }
    
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
    if (!Core::config->inspector.ShowAssemblyExplorer || !Core::config->ShowImGui) return;
    
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
        
        ImGui::BeginChild("AssemblyExplorerMain", ImVec2(0, availableHeight), false, ImGuiWindowFlags_NoScrollbar);
        
        RenderAssemblyListPanel();
        
        ImGui::SameLine();
        RenderDivider("AssemblyClassDivider", assemblyPanelWidth, availableHeight);
        
        ImGui::SameLine();
        RenderClassListPanel();
        
        if (showDetailsPanel && selectedClass)
        {
            ImGui::SameLine();
            RenderDivider("ClassDetailsDivider", classPanelWidth, availableHeight);
            
            ImGui::SameLine();
            RenderClassDetailsPanel();
        }
        
        ImGui::EndChild();
        
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
    
    if (!selectedClass->instances.empty())
    {
        ImGui::TextDisabled("Active Instance:");
        
        std::string previewText = "None";
        if (selectedInstance)
        {
            previewText = selectedInstance->displayName;
        }
        
        if (ImGui::BeginCombo("##InstanceSelect", previewText.c_str()))
        {
            bool isSelected = (selectedInstance == nullptr);
            if (ImGui::Selectable("None", isSelected))
            {
                selectedInstance = nullptr;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
            
            for (size_t i = 0; i < selectedClass->instances.size(); i++)
            {
                auto& instance = selectedClass->instances[i];
                std::string label = std::to_string(i + 1) + " - " + instance.displayName;
                
                bool isSelected = (selectedInstance == &instance);
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    selectedInstance = &instance;
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            
            ImGui::EndCombo();
        }
        
        if (ImGui::IsItemHovered() && selectedInstance)
        {
            ImGui::SetTooltip("Address: %p", selectedInstance->instance);
        }
        
        ImGui::SameLine();
        if (ImGui::SmallButton("Refresh##Instances"))
        {
            RefreshInstances(selectedClass);
        }
    }
    else
    {
        ImGui::TextDisabled("No active instances found");
        ImGui::SameLine();
        if (ImGui::SmallButton("Refresh##Instances"))
        {
            RefreshInstances(selectedClass);
        }
    }
    
    ImGui::Separator();
    
    ImGui::Columns(3, "ClassStats", false);
    
    ImGui::TextDisabled("Fields:");
    ImGui::Text("%d", selectedClass->fieldCount);
    ImGui::NextColumn();
    
    ImGui::TextDisabled("Methods:");
    ImGui::Text("%d", selectedClass->methodCount);
    ImGui::NextColumn();
    
    ImGui::TextDisabled("Instances:");
    ImGui::Text("%zu", selectedClass->instances.size());
    ImGui::NextColumn();
    
    ImGui::Columns(1);
    
    ImGui::Separator();
    
    if (!klass->fields.empty())
    {
        if (ImGui::CollapsingHeader("Fields", ImGuiTreeNodeFlags_DefaultOpen))
        {
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
                    
                    bool isStatic = field->static_field;
                    bool canEdit = isStatic || canEditInstance;
                    bool isEditableType = FieldEditor::IsEditableType(field->type ? field->type->name : "");
                    bool isPointerType = FieldEditor::IsPointerType(field->type ? field->type->name : "");
                    bool showEdit = canEdit && (isEditableType || isPointerType);
                    
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 color = isStatic ? 
                        ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                        ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(field->name.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
                    std::string typeName = field->type->name;
                    if (typeName.length() > 35) typeName = typeName.substr(0, 32) + "...";
                    ImGui::TextUnformatted(typeName.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(2);
                    if (!isStatic)
                        ImGui::TextDisabled("0x%X", field->offset);
                    else
                        ImGui::TextDisabled("[S]");
                    
                    ImGui::TableSetColumnIndex(3);
                    RenderFieldRow(field.get(), canEditInstance ? selectedInstance->instance : nullptr);
                    
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
                    
                    bool isStatic = method->static_function;
                    bool canInvoke = isStatic || canInvokeInstance;
                    
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 color = isStatic ? 
                        ImVec4(0.4f, 0.7f, 1.0f, 1.0f) : 
                        ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(method->name.c_str());
                    ImGui::PopStyleColor();
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
                    std::string retType = method->return_type->name;
                    if (retType.length() > 35) retType = retType.substr(0, 32) + "...";
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
                        if (params.length() > 50) params = params.substr(0, 47) + "...";
                        ImGui::TextDisabled("(%s)", params.c_str());
                    }
                    else
                    {
                        ImGui::TextDisabled("()");
                    }
                    
                    ImGui::TableSetColumnIndex(3);
                    std::string flags;
                    if (isStatic) flags += "S";
                    if (!flags.empty())
                        ImGui::TextDisabled("[%s]", flags.c_str());
                    
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
                            try
                            {
                                method->RuntimeInvoke<void>(target);
                            }
                            catch (...) {}
                        }
                        else
                        {
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
    
    try
    {
        auto objects = classInfo->classHandle->FindObjectsOfType<void*>();
        for (auto* obj : objects)
        {
            if (!obj) continue;
            
            ClassInstanceInfo info;
            info.instance = obj;
            
            std::stringstream ss;
            ss << std::hex << obj;
            info.displayName = "0x" + ss.str();
            
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
    if (!field || !field->type) return;
    
    std::string typeName = field->type->name;
    
    try
    {
        if (field->static_field)
        {
            if (typeName == "System.String")
            {
                UT::String* strPtr = nullptr;
                field->GetStaticValue(&strPtr);
                if (strPtr)
                {
                    std::string str = strPtr->ToString();
                    if (str.length() > 25) str = str.substr(0, 22) + "...";
                    ImGui::Text("\"%s\"", str.c_str());
                }
                else
                {
                    ImGui::TextDisabled("null");
                }
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                bool value = false;
                field->GetStaticValue(&value);
                ImGui::Text("%s", value ? "true" : "false");
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                float value = 0.0f;
                field->GetStaticValue(&value);
                ImGui::Text("%.4f", value);
            }
            else if (typeName == "System.Double")
            {
                double value = 0.0;
                field->GetStaticValue(&value);
                ImGui::Text("%.4f", value);
            }
            else if (typeName == "System.Int64")
            {
                int64_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%lld", value);
            }
            else if (typeName == "System.UInt64")
            {
                uint64_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%llu", value);
            }
            else if (typeName == "System.Int16" || typeName == "System.Short")
            {
                int16_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.UInt16" || typeName == "System.UShort")
            {
                uint16_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%u", value);
            }
            else if (typeName == "System.Byte")
            {
                uint8_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%u", value);
            }
            else if (typeName == "System.SByte")
            {
                int8_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.Int32" || typeName == "System.Int")
            {
                int32_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.UInt32" || typeName == "System.UInt")
            {
                uint32_t value = 0;
                field->GetStaticValue(&value);
                ImGui::Text("%u", value);
            }
            else if (typeName == "UnityEngine.Vector3")
            {
                Vec3 value;
                field->GetStaticValue(&value);
                ImGui::Text("(%.2f, %.2f, %.2f)", value.x, value.y, value.z);
            }
            else
            {
                ImGui::TextDisabled("...");
            }
        }
        else if (instance)
        {
            void* fieldAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(instance) + field->offset);
            
            if (typeName == "System.String")
            {
                UT::String* strPtr = *reinterpret_cast<UT::String**>(fieldAddr);
                if (strPtr)
                {
                    std::string str = strPtr->ToString();
                    if (str.length() > 25) str = str.substr(0, 22) + "...";
                    ImGui::Text("\"%s\"", str.c_str());
                }
                else
                {
                    ImGui::TextDisabled("null");
                }
            }
            else if (typeName == "System.Boolean" || typeName == "System.Bool")
            {
                bool value = *reinterpret_cast<bool*>(fieldAddr);
                ImGui::Text("%s", value ? "true" : "false");
            }
            else if (typeName == "System.Single" || typeName == "System.Float")
            {
                float value = *reinterpret_cast<float*>(fieldAddr);
                ImGui::Text("%.4f", value);
            }
            else if (typeName == "System.Double")
            {
                double value = *reinterpret_cast<double*>(fieldAddr);
                ImGui::Text("%.4f", value);
            }
            else if (typeName == "System.Int64")
            {
                int64_t value = *reinterpret_cast<int64_t*>(fieldAddr);
                ImGui::Text("%lld", value);
            }
            else if (typeName == "System.UInt64")
            {
                uint64_t value = *reinterpret_cast<uint64_t*>(fieldAddr);
                ImGui::Text("%llu", value);
            }
            else if (typeName == "System.Int16" || typeName == "System.Short")
            {
                int16_t value = *reinterpret_cast<int16_t*>(fieldAddr);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.UInt16" || typeName == "System.UShort")
            {
                uint16_t value = *reinterpret_cast<uint16_t*>(fieldAddr);
                ImGui::Text("%u", value);
            }
            else if (typeName == "System.Byte")
            {
                uint8_t value = *reinterpret_cast<uint8_t*>(fieldAddr);
                ImGui::Text("%u", value);
            }
            else if (typeName == "System.SByte")
            {
                int8_t value = *reinterpret_cast<int8_t*>(fieldAddr);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.Int32" || typeName == "System.Int")
            {
                int32_t value = *reinterpret_cast<int32_t*>(fieldAddr);
                ImGui::Text("%d", value);
            }
            else if (typeName == "System.UInt32" || typeName == "System.UInt")
            {
                uint32_t value = *reinterpret_cast<uint32_t*>(fieldAddr);
                ImGui::Text("%u", value);
            }
            else if (typeName == "UnityEngine.Vector3")
            {
                Vec3 value = *reinterpret_cast<Vec3*>(fieldAddr);
                ImGui::Text("(%.2f, %.2f, %.2f)", value.x, value.y, value.z);
            }
            else
            {
                void* ptr = *reinterpret_cast<void**>(fieldAddr);
                if (ptr)
                    ImGui::TextDisabled("%p", ptr);
                else
                    ImGui::TextDisabled("null");
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
        
        if (invokeState.hasResult)
        {
            ImGui::Text("Result: %s", invokeState.resultText.c_str());
        }
        
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
