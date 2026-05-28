#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

REGISTER_FEATURE(Inspector)

void Inspector::Update(const float deltaTime)
{
	s_Instance = this;

	const auto& [Enabled, AutoUpdateObject, AutoRefresh, ShowAssemblyExplorer, ShowDebugConsole, ObjectPickerEnabled] =
		Config::settings.inspector;
	if (!Enabled || !Config::state.showMenu) return;

	UR::ThreadAttach();

	static float timer = 0.0f;
	timer += deltaTime;

	if (timer >= 1.f)
	{
		timer -= 1.f;

		if (AutoRefresh)
			RefreshHierarchy();

		if (AutoUpdateObject && activeTabIndex >= 0 && std::cmp_less(activeTabIndex, openTabs.size()))
		{
			RefreshTabData(openTabs[activeTabIndex]);
		}
	}
}

void Inspector::Render()
{
	if (!Config::settings.inspector.enabled || !Config::state.showMenu) return;

	UR::ThreadAttach();

	ImGui::SetNextWindowSize(ImVec2(380, 600), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Hierarchy", nullptr))
	{
		if (ImGui::BeginTabBar("HierarchyTabs"))
		{
			if (ImGui::BeginTabItem("Scene"))
			{
				if (ImGui::Button("Refresh"))
					RefreshHierarchy();

				ImGui::SameLine();
				ImGui::Checkbox("Auto", &Config::settings.inspector.autoRefresh);

				ImGui::SameLine();
				if (ImGui::SmallButton("Collapse"))
					SetAllNodesExpanded(rootNodes, false);

				ImGui::SameLine();
				if (ImGui::SmallButton("Expand"))
					SetAllNodesExpanded(rootNodes, true);

				ImGui::SameLine();
				ImGui::TextDisabled("| %zu", rootNodes.size());

				ImGui::SameLine();
				{
					const bool pickerActive = Config::settings.inspector.objectPickerEnabled;
					if (pickerActive)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.15f, 0.15f, 1.0f));
					}
					if (ImGui::SmallButton(pickerActive ? "Pick: ON" : "Pick"))
						Config::settings.inspector.objectPickerEnabled = !Config::settings.inspector.
							objectPickerEnabled;
					if (pickerActive)
						ImGui::PopStyleColor(3);
				}

				ImGui::PushItemWidth(-1);
				ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, sizeof(searchBuffer),
				                         ImGuiInputTextFlags_EscapeClearsAll);
				ImGui::PopItemWidth();

				ImGui::Separator();

				if (ImGui::BeginChild("HierarchyTree", ImVec2(0, 0), false))
				{
					if (rootNodes.empty())
					{
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::NewLine();
						float cx = (ImGui::GetContentRegionAvail().x - 180.0f) * 0.5f;
						if (cx > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);
						ImGui::TextDisabled("No scene hierarchy loaded");
						ImGui::Spacing();
						if (cx > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);
						if (ImGui::Button("Load Scene Hierarchy", ImVec2(180, 0)))
							RefreshHierarchy();
					}
					else
					{
						for (auto& node : rootNodes)
						{
							RenderHierarchyNode(node);
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Static Instances"))
			{
				if (ImGui::Button("Refresh"))
					ScanStaticClasses();

				ImGui::SameLine();
				ImGui::Checkbox("Auto", &Config::settings.inspector.autoRefresh);

				ImGui::SameLine();
				if (ImGui::SmallButton("Collapse"))
				{
				}

				ImGui::SameLine();
				if (ImGui::SmallButton("Expand"))
				{
				}

				ImGui::SameLine();
				ImGui::TextDisabled("| %zu", staticInstances.size());

				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::InputTextWithHint("##StaticSearch", "Search instances...", staticSearchBuffer,
				                         sizeof(staticSearchBuffer), ImGuiInputTextFlags_EscapeClearsAll);
				ImGui::PopItemWidth();

				ImGui::Separator();

				if (ImGui::BeginChild("StaticInstanceTree", ImVec2(0, 0), false))
				{
					if (!hasScannedStatic)
					{
						ImGui::Spacing();
						ImGui::TextDisabled("Click Refresh to scan for custom static instances.");
					}
					else if (staticInstances.empty())
					{
						ImGui::TextDisabled("No non-null static instances found.");
					}
					else
					{
						for (const auto& node : staticInstances)
						{
							if (staticSearchBuffer[0] != '\0')
							{
								std::string lowerFullName = node.fullName;
								std::string lowerSearch = staticSearchBuffer;
								std::ranges::transform(lowerFullName, lowerFullName.begin(), tolower);
								std::ranges::transform(lowerSearch, lowerSearch.begin(), tolower);

								if (lowerFullName.find(lowerSearch) == std::string::npos)
									continue;
							}

							if (ImGui::Selectable(node.fullName.c_str()))
							{
								OpenStaticInstanceInNewTab(node);
							}
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	RenderDetailsWindow();
	DrawSelectedObjectBoundingBox();
	ProcessObjectPicker();
}

void Inspector::ScanStaticClasses()
{
	staticInstances.clear();
	printf("[ScanStaticClasses] Started\n");

	for (const auto& assembly : UR::assembly)
	{
		if (!assembly) continue;
		printf("[ScanStaticClasses] Assembly: %s\n", assembly->name.c_str());

		for (const auto& klass : assembly->classes)
		{
			if (!klass) continue;

			for (const auto& field : klass->fields)
			{
				try
				{
					if (field && field->static_field)
					{
						if (!field->type) continue;

						// Mono JIT will abort() if mono_class_vtable is called on an uninflated generic type.
						// Skip generic types (backtick) and compiler-generated types (<, $).
						if (klass->name.find('`') != std::string::npos ||
							klass->name.find('<') != std::string::npos ||
							klass->name.find('$') != std::string::npos)
							continue;

						std::string typeName = field->type->name;
						if (typeName.starts_with("System.") || typeName.starts_with("UnityEngine.") || typeName.
							starts_with("Unity."))
							continue;

						if (typeName == "int" || typeName == "float" || typeName == "bool" || typeName == "double" ||
							typeName == "string")
							continue;

						void* instance = nullptr;

						if (Config::state.unityMode == UnityResolve::Mode::Il2Cpp)
						{
							if (void* static_fields_ptr = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(klass->
								address) + 0xB8))
							{
								instance = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(static_fields_ptr) +
									field->offset);
							}
						}
						else
						{
							if (!Helper::SafeGetStaticFieldPointer(field->address, instance))
							{
								instance = nullptr;
							}
						}

						if (instance != nullptr)
						{
							void* typeClassHandle = Helper::SafeGetObjectClass(instance);
							if (!typeClassHandle) continue;

							StaticInstanceNode node;
							node.instance = instance;
							node.typeClassHandle = typeClassHandle;

							std::string kName = klass->namespaze.empty()
								                    ? klass->name
								                    : klass->namespaze + "." + klass->name;
							node.name = kName + "." + field->name;
							node.fullName = node.name + " (" + typeName + ")";
							staticInstances.push_back(std::move(node));
						}
					}
				}
				catch (...)
				{
				}
			}
		}
	}

	std::ranges::sort(staticInstances, [](const auto& a, const auto& b)
	{
		return a.fullName < b.fullName;
	});

	hasScannedStatic = true;
	printf("[ScanStaticClasses] Finished. Found %zu instances.\n", staticInstances.size());
}

void Inspector::OpenStaticInstanceInNewTab(const StaticInstanceNode& node)
{
	if (openTabs.size() >= maxTabs) return;

	InspectedObjectTab newTab;
	newTab.gameObject = nullptr;
	newTab.tabName = "[S] " + node.name;

	InspectionTarget rootTarget;
	rootTarget.gameObject = nullptr;
	rootTarget.instance = node.instance;
	rootTarget.classHandle = node.typeClassHandle;
	rootTarget.name = node.name;

	rootTarget.cachedComponents.push_back(static_cast<UT::Component*>(node.instance));
	rootTarget.cachedComponentNames.push_back(node.fullName);

	rootTarget.cachedComponentFields.push_back(GetObjectFields(node.instance, node.typeClassHandle));
	rootTarget.cachedComponentProperties.push_back(GetObjectProperties(node.instance, node.typeClassHandle));
	rootTarget.cachedComponentMethods.push_back(GetObjectMethods(node.instance, node.typeClassHandle));

	newTab.navigationStack.push_back(std::move(rootTarget));

	openTabs.push_back(std::move(newTab));
	activeTabIndex = static_cast<int>(openTabs.size()) - 1;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
}

void Inspector::InspectInstance(void* instance, void* classHandle, const std::string& name)
{
	if (openTabs.size() >= maxTabs) return;

	const bool mono = Config::state.unityMode == UnityResolve::Mode::Mono;

	InspectedObjectTab newTab;
	newTab.gameObject = nullptr;
	newTab.tabName = name;

	InspectionTarget rootTarget;
	rootTarget.gameObject = nullptr;
	rootTarget.instance = instance;
	rootTarget.classHandle = classHandle;
	rootTarget.name = name;

	if (instance)
	{
		rootTarget.cachedComponents.push_back(static_cast<UT::Component*>(instance));
		std::string className = "(Unknown)";
		if (const char* cn = UR::Invoke<const char*, void*>(mono ? "mono_class_get_name" : "il2cpp_class_get_name",
		                                                    classHandle))
			className = cn;
		rootTarget.cachedComponentNames.push_back(className);
		rootTarget.cachedComponentFields.push_back(GetObjectFields(instance, classHandle));
		rootTarget.cachedComponentProperties.push_back(GetObjectProperties(instance, classHandle));
		rootTarget.cachedComponentMethods.push_back(GetObjectMethods(instance, classHandle));
	}
	else
	{
		std::string className = "(Unknown)";
		if (const char* cn = UR::Invoke<const char*, void*>(mono ? "mono_class_get_name" : "il2cpp_class_get_name",
		                                                    classHandle))
			className = cn;
		rootTarget.cachedComponentNames.push_back(className + " (static)");
		rootTarget.cachedComponentFields.push_back(GetObjectFields(nullptr, classHandle));
		rootTarget.cachedComponentProperties.push_back(GetObjectProperties(nullptr, classHandle));
		rootTarget.cachedComponentMethods.push_back(GetObjectMethods(nullptr, classHandle));
	}

	newTab.navigationStack.push_back(std::move(rootTarget));
	openTabs.push_back(std::move(newTab));
	activeTabIndex = static_cast<int>(openTabs.size()) - 1;
	showDetailsWindow = true;
	ImGui::SetWindowFocus("Inspector");
}
