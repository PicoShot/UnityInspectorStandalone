#include "pch.h"
#include "inspector.h"

static bool DragFloatLabel(const char* label, float* value, float width = 40.0f, float speed = 0.1f)
{
	ImGui::PushItemWidth(width);
	ImGui::Text("%s", label);
	ImGui::SameLine();
	bool changed = ImGui::DragFloat(("##" + std::string(label)).c_str(), value, speed);
	ImGui::PopItemWidth();
	return changed;
}

static bool DragVector3Compact(const char* id, float* values, float speed = 0.1f)
{
	bool changed = false;
	const float availWidth = ImGui::GetContentRegionAvail().x;
	const float inputWidth = (availWidth - 60.0f) / 3.0f;

	ImGui::PushID(id);

	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##X", &values[0], speed);
	ImGui::PopItemWidth();

	ImGui::SameLine(0, 4);
	ImGui::Text("X");

	ImGui::SameLine(0, 8);
	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##Y", &values[1], speed);
	ImGui::PopItemWidth();

	ImGui::SameLine(0, 4);
	ImGui::Text("Y");

	ImGui::SameLine(0, 8);
	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##Z", &values[2], speed);
	ImGui::PopItemWidth();

	ImGui::SameLine(0, 4);
	ImGui::Text("Z");

	ImGui::PopID();
	return changed;
}

static bool DragVector4Compact(const char* id, float* values, float speed = 0.01f)
{
	bool changed = false;
	const float availWidth = ImGui::GetContentRegionAvail().x;
	const float inputWidth = (availWidth - 80.0f) / 4.0f;

	ImGui::PushID(id);

	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##X", &values[0], speed);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, 2);
	ImGui::Text("X");

	ImGui::SameLine(0, 6);
	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##Y", &values[1], speed);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, 2);
	ImGui::Text("Y");

	ImGui::SameLine(0, 6);
	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##Z", &values[2], speed);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, 2);
	ImGui::Text("Z");

	ImGui::SameLine(0, 6);
	ImGui::PushItemWidth(inputWidth);
	changed |= ImGui::DragFloat("##W", &values[3], speed);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, 2);
	ImGui::Text("W");

	ImGui::PopID();
	return changed;
}

void Inspector::RenderEditableField(UT::Component* component, const ComponentFieldInfo& field) const
{
	if (!component || field.offset < 0) return;

	ImGui::PushID(field.offset);

	const float availWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(availWidth);

	if (field.isStatic)
	{
		switch (field.editableType)
		{
		case EditableType::Int:
		{
			int val;
			if (Helper::SafeGetStaticFieldInt(field.fieldHandle, val))
			{
				if (ImGui::DragInt("##val", &val))
					Helper::SafeSetStaticFieldInt(field.fieldHandle, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Float:
		{
			float val;
			if (Helper::SafeGetStaticFieldFloat(field.fieldHandle, val))
			{
				if (ImGui::DragFloat("##val", &val, 0.1f))
					Helper::SafeSetStaticFieldFloat(field.fieldHandle, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Double:
		{
			if (double val; Helper::SafeGetStaticFieldDouble(field.fieldHandle, val))
			{
				float fVal = static_cast<float>(val);
				if (ImGui::DragFloat("##val", &fVal, 0.01f))
					Helper::SafeSetStaticFieldDouble(field.fieldHandle, static_cast<double>(fVal));
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Bool:
		{
			bool val;
			if (Helper::SafeGetStaticFieldBool(field.fieldHandle, val))
			{
				if (ImGui::Checkbox("##val", &val))
					Helper::SafeSetStaticFieldBool(field.fieldHandle, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Vector3:
		{
			UT::Vector3 val;
			if (Helper::SafeGetStaticFieldVector3(field.fieldHandle, val))
			{
				float arr[3] = { val.x, val.y, val.z };
				if (DragVector3Compact("##val", arr, 0.1f))
				{
					val.x = arr[0]; val.y = arr[1]; val.z = arr[2];
					Helper::SafeSetStaticFieldVector3(field.fieldHandle, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		default:
			ImGui::TextDisabled("[static]");
			break;
		}
	}
	else
	{
		switch (field.editableType)
		{
		case EditableType::Int:
		{
			int val;
			if (Helper::SafeReadInt(component, field.offset, val))
			{
				if (ImGui::DragInt("##val", &val))
					Helper::SafeWriteInt(component, field.offset, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Float:
		{
			float val;
			if (Helper::SafeReadFloat(component, field.offset, val))
			{
				if (ImGui::DragFloat("##val", &val, 0.1f))
					Helper::SafeWriteFloat(component, field.offset, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Double:
		{
			double val;
			if (Helper::SafeReadDouble(component, field.offset, val))
			{
				float fVal = static_cast<float>(val);
				if (ImGui::DragFloat("##val", &fVal, 0.01f))
					Helper::SafeWriteDouble(component, field.offset, static_cast<double>(fVal));
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Bool:
		{
			bool val;
			if (Helper::SafeReadBool(component, field.offset, val))
			{
				if (ImGui::Checkbox("##val", &val))
					Helper::SafeWriteBool(component, field.offset, val);
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::String:
		{
			UT::String* strPtr = nullptr;
			if (Helper::SafeReadStringPtr(component, field.offset, strPtr))
			{
				std::string currentStr = strPtr ? strPtr->ToString() : "(null)";
				ImGui::TextDisabled("\"%s\"", currentStr.c_str());
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Vector2:
		{
			UT::Vector2 val;
			if (Helper::SafeReadVector2(component, field.offset, val))
			{
				float arr[2] = { val.x, val.y };
				if (ImGui::DragFloat2("##val", arr, 0.1f))
				{
					val.x = arr[0]; val.y = arr[1];
					Helper::SafeWriteVector2(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Vector3:
		{
			if (UT::Vector3 val; Helper::SafeReadVector3(component, field.offset, val))
			{
				float arr[3] = { val.x, val.y, val.z };
				if (DragVector3Compact("##val", arr, 0.1f))
				{
					val.x = arr[0]; val.y = arr[1]; val.z = arr[2];
					Helper::SafeWriteVector3(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Vector4:
		{
			UT::Vector4 val;
			if (Helper::SafeReadVector4(component, field.offset, val))
			{
				float arr[4] = { val.x, val.y, val.z, val.w };
				if (ImGui::DragFloat4("##val", arr, 0.1f))
				{
					val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
					Helper::SafeWriteVector4(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Quaternion:
		{
			if (UT::Quaternion val; Helper::SafeReadQuaternion(component, field.offset, val))
			{
				float arr[4] = { val.x, val.y, val.z, val.w };
				if (DragVector4Compact("##val", arr, 0.01f))
				{
					val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
					Helper::SafeWriteQuaternion(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Color:
		{
			if (UT::Color val; Helper::SafeReadColor(component, field.offset, val))
			{
				float arr[4] = { val.r, val.g, val.b, val.a };
				if (ImGui::ColorEdit4("##val", arr, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar))
				{
					val.r = arr[0]; val.g = arr[1]; val.b = arr[2]; val.a = arr[3];
					Helper::SafeWriteColor(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		default:
			ImGui::TextDisabled("...");
			break;
		}
	}

	ImGui::PopID();
}

void Inspector::RenderEditableProperty(UT::Component* component, const ComponentPropertyInfo& prop) const
{
	if (!component) return;

	ImGui::PushID(prop.name.c_str());

	const float availWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(availWidth);

	if (!prop.canRead)
	{
		ImGui::TextDisabled("(write-only)");
		ImGui::PopID();
		return;
	}

	switch (prop.editableType)
	{
	case EditableType::Int:
	{
		int val = 0;
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(int)))
		{
			if (prop.canWrite && ImGui::DragInt("##val", &val))
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			else if (!prop.canWrite)
				ImGui::Text("%d", val);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Float:
	{
		float val = 0;
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(float)))
		{
			if (prop.canWrite && ImGui::DragFloat("##val", &val, 0.1f))
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			else if (!prop.canWrite)
				ImGui::Text("%.3f", val);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Double:
	{
		double val = 0;
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(double)))
		{
			float fVal = static_cast<float>(val);
			if (prop.canWrite && ImGui::DragFloat("##val", &fVal, 0.01f))
			{
				val = static_cast<double>(fVal);
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
				ImGui::Text("%.6f", val);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Bool:
	{
		bool val = false;
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(bool)))
		{
			if (prop.canWrite)
			{
				if (ImGui::Checkbox("##val", &val))
					Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else
				ImGui::Text("%s", val ? "true" : "false");
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Vector2:
	{
		UT::Vector2 val = {};
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(UT::Vector2)))
		{
			float arr[2] = { val.x, val.y };
			if (prop.canWrite && ImGui::DragFloat2("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
				ImGui::Text("(%.2f, %.2f)", val.x, val.y);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Vector3:
	{
		UT::Vector3 val = {};
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(UT::Vector3)))
		{
			float arr[3] = { val.x, val.y, val.z };
			if (prop.canWrite && DragVector3Compact("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
				ImGui::Text("(%.2f, %.2f, %.2f)", val.x, val.y, val.z);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Vector4:
	{
		UT::Vector4 val = {};
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(UT::Vector4)))
		{
			float arr[4] = { val.x, val.y, val.z, val.w };
			if (prop.canWrite && ImGui::DragFloat4("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
				ImGui::Text("(%.2f, %.2f, %.2f, %.2f)", val.x, val.y, val.z, val.w);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Quaternion:
	{
		UT::Quaternion val = {};
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(UT::Quaternion)))
		{
			float arr[4] = { val.x, val.y, val.z, val.w };
			if (prop.canWrite && DragVector4Compact("##val", arr, 0.01f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
				ImGui::Text("(%.3f, %.3f, %.3f, %.3f)", val.x, val.y, val.z, val.w);
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Color:
	{
		UT::Color val = {};
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(UT::Color)))
		{
			float arr[4] = { val.r, val.g, val.b, val.a };
			if (prop.canWrite && ImGui::ColorEdit4("##val", arr, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar))
			{
				val.r = arr[0]; val.g = arr[1]; val.b = arr[2]; val.a = arr[3];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImVec4 col(val.r, val.g, val.b, val.a);
				ImGui::ColorButton("##preview", col, ImGuiColorEditFlags_NoTooltip, ImVec2(20, 14));
			}
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	default:
		ImGui::TextDisabled("...");
		break;
	}

	ImGui::PopID();
}

void Inspector::RenderTransformSection(UT::Transform* transform, InspectedObjectTab& tab) const
{
	if (!transform) return;


	bool open = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen);

	if (open)
	{
		ImGui::Indent(10.0f);

		if (ImGui::Button("World", ImVec2(60, 0))) tab.showWorldTransform = !tab.showWorldTransform;

		ImGui::SameLine();
		if (ImGui::Button("Local", ImVec2(60, 0))) tab.showLocalTransform = !tab.showLocalTransform;

		const float availWidth = ImGui::GetContentRegionAvail().x;
		const bool showBoth = tab.showWorldTransform && tab.showLocalTransform;
		const float childWidth = showBoth ? (availWidth - 10.0f) * 0.5f : availWidth;

		if (tab.showWorldTransform)
		{
			if (showBoth)
			{
				ImGui::BeginChild("WorldTransform", ImVec2(childWidth, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
			}

			ImGui::TextDisabled("World Position");
			const auto pos = transform->GetPosition();
			float posArr[3] = { pos.x, pos.y, pos.z };
			if (DragVector3Compact("WorldPos", posArr, 0.1f))
				transform->SetPosition({ posArr[0], posArr[1], posArr[2] });

			ImGui::Spacing();
			ImGui::TextDisabled("World Rotation");
			const auto rot = transform->GetRotation();
			float rotArr[4] = { rot.x, rot.y, rot.z, rot.w };
			if (DragVector4Compact("WorldRot", rotArr, 0.01f))
				transform->SetRotation({ rotArr[0], rotArr[1], rotArr[2], rotArr[3] });

			if (showBoth)
			{
				ImGui::EndChild();
			}
		}

		if (showBoth)
		{
			ImGui::SameLine();
		}

		if (tab.showLocalTransform)
		{
			if (showBoth)
			{
				ImGui::BeginChild("LocalTransform", ImVec2(childWidth, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
			}

			ImGui::TextDisabled("Local Position");
			const auto localPos = transform->GetLocalPosition();
			float localPosArr[3] = { localPos.x, localPos.y, localPos.z };
			if (DragVector3Compact("LocalPos", localPosArr, 0.1f))
				transform->SetLocalPosition({ localPosArr[0], localPosArr[1], localPosArr[2] });

			ImGui::Spacing();
			ImGui::TextDisabled("Local Rotation");
			const auto localRot = transform->GetLocalRotation();
			float localRotArr[4] = { localRot.x, localRot.y, localRot.z, localRot.w };
			if (DragVector4Compact("LocalRot", localRotArr, 0.01f))
				transform->SetLocalRotation({ localRotArr[0], localRotArr[1], localRotArr[2], localRotArr[3] });

			ImGui::Spacing();
			ImGui::TextDisabled("Local Scale");
			const auto scale = transform->GetLocalScale();
			float scaleArr[3] = { scale.x, scale.y, scale.z };
			if (DragVector3Compact("LocalScale", scaleArr, 0.1f))
				transform->SetLocalScale({ scaleArr[0], scaleArr[1], scaleArr[2] });

			if (showBoth)
			{
				ImGui::EndChild();
			}
		}

		ImGui::Unindent(10.0f);
		ImGui::Spacing();
	}
}

void Inspector::RenderMethodsSection(UT::Component* component, const std::vector<ComponentMethodInfo>& methods, InspectedObjectTab& tab, const size_t componentIndex)
{
	if (methods.empty())
	{
		ImGui::TextDisabled("No accessible methods");
		return;
	}

	if (componentIndex >= tab.methodSearchBuffers.size())
		tab.methodSearchBuffers.resize(componentIndex + 1);
	char* searchBuffer = tab.methodSearchBuffers[componentIndex].data();

	ImGui::SetNextItemWidth(150);
	ImGui::InputTextWithHint("##MethodSearch", "Search...", searchBuffer, 256);

	ImGui::SameLine();
	ImGui::Checkbox("Static", &tab.filterStaticOnly);
	ImGui::SameLine();
	ImGui::Checkbox("Instance", &tab.filterInstanceOnly);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::vector<const ComponentMethodInfo*> filteredMethods;
	for (const auto& method : methods)
	{
		if (PassesMethodFilter(method, searchBuffer, tab.filterStaticOnly, tab.filterInstanceOnly))
			filteredMethods.push_back(&method);
	}

	if (filteredMethods.empty())
	{
		ImGui::TextDisabled("No methods match filters");
		return;
	}

	ImGui::TextDisabled("Showing %zu of %zu methods", filteredMethods.size(), methods.size());
	ImGui::Spacing();

	if (ImGui::BeginTable("MethodsTable", 4, 
		ImGuiTableFlags_Resizable | 
		ImGuiTableFlags_BordersInnerV | 
		ImGuiTableFlags_SizingFixedFit |
		ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
		ImGui::TableSetupColumn("Return", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableSetupColumn("Params", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 50.0f);

		for (const auto* method : filteredMethods)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (method->isStatic)
			{
				ImGui::Text("[S] %s", method->name.c_str());
			}
			else if (method->isVirtual)
			{
				ImGui::Text("[V] %s", method->name.c_str());
			}
			else
			{
				ImGui::Text("%s", method->name.c_str());
			}

			ImGui::TableSetColumnIndex(1);
			std::string retType = method->returnTypeName;
			if (retType.length() > 25) retType = retType.substr(0, 22) + "...";
			ImGui::Text("-> %s", retType.c_str());

			ImGui::TableSetColumnIndex(2);
			if (!method->parameters.empty())
			{
				std::string params;
				for (const auto& [paramName, paramType] : method->parameters)
				{
					if (!params.empty()) params += ", ";
					params += paramType;
				}
				if (params.length() > 40) params = params.substr(0, 37) + "...";
				ImGui::TextDisabled("(%s)", params.c_str());
			}
			else
			{
				ImGui::TextDisabled("()");
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::PushID(method);

			bool canInvoke = true;
			for (const auto& paramType : method->parameterEditableTypes)
			{
				if (paramType == EditableType::None)
				{
					canInvoke = false;
					break;
				}
			}

			if (canInvoke)
			{
				if (ImGui::SmallButton("Invoke"))
				{
					invokeState.showPopup = true;
					invokeState.targetComponent = component;
					invokeState.method = *method;
					invokeState.parameterValues.clear();
					invokeState.parameterValues.resize(method->parameters.size());
					invokeState.resultText.clear();
					invokeState.hasResult = false;
				}
			}
			else
			{
				ImGui::TextDisabled("N/A");
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}
}

void Inspector::RenderPropertiesSection(UT::Component* component, const std::vector<ComponentPropertyInfo>& properties, InspectedObjectTab& tab, const size_t componentIndex) const
{
	if (properties.empty())
	{
		ImGui::TextDisabled("No accessible properties");
		return;
	}

	if (componentIndex >= tab.propertySearchBuffers.size())
		tab.propertySearchBuffers.resize(componentIndex + 1);
	char* searchBuffer = tab.propertySearchBuffers[componentIndex].data();

	ImGui::SetNextItemWidth(150);
	ImGui::InputTextWithHint("##PropertySearch", "Search...", searchBuffer, 256);

	ImGui::SameLine();
	ImGui::Checkbox("Writable", &tab.filterEditableOnly);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::vector<const ComponentPropertyInfo*> filteredProps;
	for (const auto& prop : properties)
	{
		if (PassesPropertyFilter(prop, searchBuffer, tab.filterEditableOnly))
			filteredProps.push_back(&prop);
	}

	if (filteredProps.empty())
	{
		ImGui::TextDisabled("No properties match filters");
		return;
	}

	ImGui::TextDisabled("Showing %zu of %zu properties", filteredProps.size(), properties.size());
	ImGui::Spacing();

	if (ImGui::BeginTable("PropertiesTable", 3,
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_BordersInnerV |
		ImGuiTableFlags_SizingFixedFit |
		ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

		for (const auto* prop : filteredProps)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", prop->name.c_str());

			ImGui::TableSetColumnIndex(1);
			std::string typeName = prop->typeName;
			if (typeName.length() > 30) typeName = typeName.substr(0, 27) + "...";
			ImGui::Text("%s %s", 
				prop->canRead && prop->canWrite ? "[RW]" : (prop->canRead ? "[R]" : "[W]"),
				typeName.c_str());

			ImGui::TableSetColumnIndex(2);
			RenderEditableProperty(component, *prop);
		}

		ImGui::EndTable();
	}
}

void Inspector::RenderFieldsSection(UT::Component* component, const std::vector<ComponentFieldInfo>& fields, InspectedObjectTab& tab, const size_t componentIndex) const
{
	if (fields.empty())
	{
		ImGui::TextDisabled("No accessible fields");
		return;
	}

	if (componentIndex >= tab.fieldSearchBuffers.size())
		tab.fieldSearchBuffers.resize(componentIndex + 1);
	char* searchBuffer = tab.fieldSearchBuffers[componentIndex].data();

	ImGui::SetNextItemWidth(120);
	ImGui::InputTextWithHint("##FieldSearch", "Search...", searchBuffer, 256);

	ImGui::SameLine();
	ImGui::Checkbox("Editable", &tab.filterEditableOnly);
	ImGui::SameLine();
	ImGui::Checkbox("Static", &tab.filterStaticOnly);
	ImGui::SameLine();
	ImGui::Checkbox("Instance", &tab.filterInstanceOnly);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::vector<const ComponentFieldInfo*> filteredFields;
	for (const auto& field : fields)
	{
		if (PassesFieldFilter(field, searchBuffer, tab.filterEditableOnly, tab.filterStaticOnly, tab.filterInstanceOnly))
			filteredFields.push_back(&field);
	}

	if (filteredFields.empty())
	{
		ImGui::TextDisabled("No fields match filters");
		return;
	}

	ImGui::TextDisabled("Showing %zu of %zu fields", filteredFields.size(), fields.size());
	ImGui::Spacing();

	if (ImGui::BeginTable("FieldsTable", 5,
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_BordersInnerV |
		ImGuiTableFlags_SizingFixedFit |
		ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 50.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed, 40.0f);

		for (const auto* field : filteredFields)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (field->isStatic)
			{
				ImGui::TextUnformatted(field->name.c_str());
			}
			else
			{
				ImGui::TextUnformatted(field->name.c_str());
			}

			ImGui::TableSetColumnIndex(1);
			std::string typeName = field->typeName;
			if (typeName.length() > 35) typeName = typeName.substr(0, 32) + "...";
			ImGui::TextUnformatted(typeName.c_str());

			ImGui::TableSetColumnIndex(2);
			if (!field->isStatic)
				ImGui::TextDisabled("0x%X", field->offset);
			else
				ImGui::TextDisabled("[S]");

			ImGui::TableSetColumnIndex(3);
			RenderEditableField(component, *field);

			ImGui::TableSetColumnIndex(4);
			if (field->isStatic)
				ImGui::TextDisabled("S");
		}

		ImGui::EndTable();
	}
}

void Inspector::RenderComponentsSection(InspectedObjectTab& tab)
{
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##ComponentSearch", "Filter components...", tab.componentSearchBuffer, sizeof(tab.componentSearchBuffer));

	ImGui::Spacing();

	std::vector<size_t> filteredComponentIndices;
	for (size_t i = 0; i < tab.cachedComponents.size(); i++)
	{
		const std::string& compName = tab.cachedComponentNames[i];
		if (PassesComponentFilter(compName, tab.componentSearchBuffer))
			filteredComponentIndices.push_back(i);
	}

	if (filteredComponentIndices.empty())
	{
		ImGui::TextDisabled("No components match the filter");
		return;
	}

	ImGui::TextDisabled("Components: %zu of %zu", filteredComponentIndices.size(), tab.cachedComponents.size());
	ImGui::Spacing();

	if (ImGui::BeginTabBar("ComponentsTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
	{
		for (const size_t idx : filteredComponentIndices)
		{
			const auto comp = tab.cachedComponents[idx];
			const std::string& compName = tab.cachedComponentNames[idx];
			const auto& fields = tab.cachedComponentFields[idx];
			const auto& properties = tab.cachedComponentProperties.size() > idx ? tab.cachedComponentProperties[idx] : std::vector<ComponentPropertyInfo>{};
			const auto& methods = tab.cachedComponentMethods.size() > idx ? tab.cachedComponentMethods[idx] : std::vector<ComponentMethodInfo>{};

			std::string tabLabel = compName + "##" + std::to_string(idx);

			if (ImGui::BeginTabItem(tabLabel.c_str()))
			{
				ImGui::Spacing();

				std::string fullName = GetComponentFullTypeName(comp);
				ImGui::TextDisabled("Type: %s", fullName.c_str());

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				std::string innerTabBarId = "InnerTabs_" + std::to_string(idx);

				if (ImGui::BeginTabBar(innerTabBarId.c_str()))
				{
					std::string fieldsLabel = "Fields";
					if (!fields.empty()) fieldsLabel += " (" + std::to_string(fields.size()) + ")";
					if (ImGui::BeginTabItem(fieldsLabel.c_str()))
					{
						ImGui::Spacing();
						RenderFieldsSection(comp, fields, tab, idx);
						ImGui::EndTabItem();
					}

					std::string propsLabel = "Properties";
					if (!properties.empty()) propsLabel += " (" + std::to_string(properties.size()) + ")";
					if (ImGui::BeginTabItem(propsLabel.c_str()))
					{
						ImGui::Spacing();
						RenderPropertiesSection(comp, properties, tab, idx);
						ImGui::EndTabItem();
					}

					std::string methodsLabel = "Methods";
					if (!methods.empty()) methodsLabel += " (" + std::to_string(methods.size()) + ")";
					if (ImGui::BeginTabItem(methodsLabel.c_str()))
					{
						ImGui::Spacing();
						RenderMethodsSection(comp, methods, tab, idx);
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

}

void Inspector::RenderDetailsWindow()
{
	if (!showDetailsWindow || openTabs.empty()) return;

	ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Inspector", &showDetailsWindow))
	{
		RenderTabBar();
	}
	ImGui::End();

	RenderMethodInvokePopup();
}

void Inspector::RenderTabBar()
{

	if (ImGui::BeginTabBar("InspectorTabs",
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_TabListPopupButton))
	{
		for (size_t i = 0; i < openTabs.size(); ++i)
		{
			auto& tab = openTabs[i];

			if (!Core::helper->SafeIsAlive(tab.gameObject))
			{
				continue;
			}

			bool tabOpen = true;
			ImGuiTabItemFlags flags = 0;

			if (IsObjectPinned(tab.gameObject))
				flags |= ImGuiTabItemFlags_NoCloseButton;

			if (ImGui::BeginTabItem(tab.tabName.c_str(), &tabOpen, flags))
			{
				activeTabIndex = static_cast<int>(i);
				tab.isActive = true;
				RenderTabContent(tab);
				ImGui::EndTabItem();
			}
			else
			{
				tab.isActive = false;
			}

			if (!tabOpen)
			{
				CloseTab(static_cast<int>(i));
				break;
			}
		}

		ImGui::EndTabBar();
	}

}

void Inspector::RenderTabContent(InspectedObjectTab& tab)
{
	if (!Core::helper->SafeIsAlive(tab.gameObject))
	{
		ImGui::TextDisabled("Object has been destroyed");
		ImGui::Spacing();
		if (ImGui::Button("Close Tab"))
		{
			int tabIndex = FindTabForObject(tab.gameObject);
			if (tabIndex >= 0)
				CloseTab(tabIndex);
		}
		return;
	}

	if (ImGui::Button("Refresh"))
		RefreshTabData(tab);
	ImGui::SameLine();
	ImGui::Checkbox("Auto Update", &Core::config->inspector.AutoUpdateObject);
	ImGui::SameLine();

	if (ImGui::Button("Close Tab"))
	{
		int tabIndex = FindTabForObject(tab.gameObject);
		if (tabIndex >= 0)
			CloseTab(tabIndex);
	}

	ImGui::Separator();

	std::string objectName = "(Unknown)";
	if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
	{
		UT::String* name = nullptr;
		Core::helper->SafeGetName(tab.gameObject, name);
		if (name)
			objectName = name->ToString();
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Name: %s", objectName.c_str());

	if (bool isActive = true; Core::helper->SafeGetActiveSelf(tab.gameObject, isActive))
	{
		ImGui::SameLine();
		if (ImGui::Checkbox("##Active", &isActive))
			Core::helper->SafeSetActive(tab.gameObject, isActive);
		ImGui::SameLine();
		ImGui::Text("Active");
	}

	ImGui::SameLine();
	if (IsObjectPinned(tab.gameObject))
	{
		if (ImGui::SmallButton("Unpin"))
			UnpinObject(tab.gameObject);
	}
	else
	{
		if (ImGui::SmallButton("Pin"))
			PinObject(tab.gameObject);
	}

	if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
	{
		UT::String* tag;
		if (const auto validTag = Core::helper->SafeGetTag(tab.gameObject, tag); tag && validTag)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("| Tag: %s", tag->ToString().c_str());
		}
	}

	if (bool isStatic = false; Helper::SafeGetIsStatic(tab.gameObject, isStatic))
	{
		ImGui::SameLine();
		ImGui::TextDisabled("| Static: %s", isStatic ? "Yes" : "No");
	}

	if (!tab.objectPath.empty())
	{
		ImGui::TextDisabled("Path: %s", tab.objectPath.c_str());
	}

	ImGui::Separator();

	if (ImGui::BeginChild("Content", ImVec2(0, 0), false))
	{
		UT::Transform* transform = nullptr;
		if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
			Core::helper->SafeGetTransform(tab.gameObject, transform);

		if (transform)
		{
			RenderTransformSection(transform, tab);
		}
		else
		{
			ImGui::TextDisabled("Transform not available");
		}

		ImGui::Spacing();

		RenderComponentsSection(tab);
	}
	ImGui::EndChild();
}
