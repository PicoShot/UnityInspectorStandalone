#include "pch.h"
#include "inspector.h"

void Inspector::RenderEditableField(UT::Component* component, const ComponentFieldInfo& field) const
{
	if (!component || field.offset < 0) return;

	ImGui::PushID(field.offset);

	if (field.isStatic)
	{
		switch (field.editableType)
		{
		case EditableType::Int:
		{
			int val;
			if (Helper::SafeGetStaticFieldInt(field.fieldHandle, val))
			{
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragInt("##val", &val))
				{
					Helper::SafeSetStaticFieldInt(field.fieldHandle, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Float:
		{
			float val;
			if (Helper::SafeGetStaticFieldFloat(field.fieldHandle, val))
			{
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat("##val", &val, 0.1f))
				{
					Helper::SafeSetStaticFieldFloat(field.fieldHandle, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Double:
		{
			if (double val; Helper::SafeGetStaticFieldDouble(field.fieldHandle, val))
			{
				float fVal = static_cast<float>(val);
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat("##val", &fVal, 0.01f))
				{
					Helper::SafeSetStaticFieldDouble(field.fieldHandle, static_cast<double>(fVal));
				}
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
				{
					Helper::SafeSetStaticFieldBool(field.fieldHandle, val);
				}
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
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat3("##val", arr, 0.1f))
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
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragInt("##val", &val))
				{
					Helper::SafeWriteInt(component, field.offset, val);
				}
			}
			else { ImGui::TextDisabled("ERROR"); }
			break;
		}
		case EditableType::Float:
		{
			float val;
			if (Helper::SafeReadFloat(component, field.offset, val))
			{
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat("##val", &val, 0.1f))
				{
					Helper::SafeWriteFloat(component, field.offset, val);
				}
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
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat("##val", &fVal, 0.01f))
				{
					Helper::SafeWriteDouble(component, field.offset, static_cast<double>(fVal));
				}
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
				{
					Helper::SafeWriteBool(component, field.offset, val);
				}
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
				ImGui::SetNextItemWidth(-1);
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
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat3("##val", arr, 0.1f))
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
				ImGui::SetNextItemWidth(-1);
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
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragFloat4("##val", arr, 0.01f))
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
				ImGui::SetNextItemWidth(-1);
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragInt("##val", &val))
			{
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("%d", val);
			}
		}
		else { ImGui::TextDisabled("ERROR"); }
		break;
	}
	case EditableType::Float:
	{
		float val = 0;
		if (Helper::SafeInvokeGetter(component, prop.getterHandle, &val, sizeof(float)))
		{
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat("##val", &val, 0.1f))
			{
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("%.3f", val);
			}
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat("##val", &fVal, 0.01f))
			{
				val = static_cast<double>(fVal);
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("%.6f", val);
			}
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
				{
					Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
				}
			}
			else
			{
				ImGui::Text("%s", val ? "true" : "false");
			}
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat2("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("(%.2f, %.2f)", val.x, val.y);
			}
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat3("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("(%.2f, %.2f, %.2f)", val.x, val.y, val.z);
			}
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat4("##val", arr, 0.1f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("(%.2f, %.2f, %.2f, %.2f)", val.x, val.y, val.z, val.w);
			}
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
			ImGui::SetNextItemWidth(-1);
			if (prop.canWrite && ImGui::DragFloat4("##val", arr, 0.01f))
			{
				val.x = arr[0]; val.y = arr[1]; val.z = arr[2]; val.w = arr[3];
				Helper::SafeInvokeSetter(component, prop.setterHandle, &val);
			}
			else if (!prop.canWrite)
			{
				ImGui::Text("(%.3f, %.3f, %.3f, %.3f)", val.x, val.y, val.z, val.w);
			}
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
			ImGui::SetNextItemWidth(-1);
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

	ImGui::PushID(id);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
	changed |= DragFloatLabel("X", &values[0], 45.0f, speed);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 8);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
	changed |= DragFloatLabel("Y", &values[1], 45.0f, speed);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 8);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
	changed |= DragFloatLabel("Z", &values[2], 45.0f, speed);
	ImGui::PopStyleColor();

	ImGui::PopID();
	return changed;
}

static bool DragVector4Compact(const char* id, float* values, float speed = 0.01f)
{
	bool changed = false;

	ImGui::PushID(id);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
	changed |= DragFloatLabel("X", &values[0], 40.0f, speed);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 6);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
	changed |= DragFloatLabel("Y", &values[1], 40.0f, speed);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 6);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
	changed |= DragFloatLabel("Z", &values[2], 40.0f, speed);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 6);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
	changed |= DragFloatLabel("W", &values[3], 40.0f, speed);
	ImGui::PopStyleColor();

	ImGui::PopID();
	return changed;
}

void Inspector::RenderTransformSection(UT::Transform* transform, InspectedObjectTab& tab) const
{
	if (!transform) return;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::Checkbox("World", &tab.showWorldTransform);
		ImGui::SameLine(0, 12);
		ImGui::Checkbox("Local", &tab.showLocalTransform);
		ImGui::PopStyleColor();

		ImGui::Separator();
		ImGui::Spacing();

		const float childWidth = ImGui::GetContentRegionAvail().x * 0.48f;

		if (tab.showWorldTransform)
		{
			ImGui::BeginChild("WorldTransform", ImVec2(childWidth, 180), true);

			ImGui::TextDisabled("World");
			ImGui::Separator();

			ImGui::Text("Position");
			ImGui::Indent();
			const auto pos = transform->GetPosition();
			float posArr[3] = { pos.x, pos.y, pos.z };
			if (DragVector3Compact("WorldPos", posArr, 0.1f))
			{
				transform->SetPosition({ posArr[0], posArr[1], posArr[2] });
			}
			ImGui::Unindent();

			ImGui::Text("Rotation");
			ImGui::Indent();
			const auto rot = transform->GetRotation();
			float rotArr[4] = { rot.x, rot.y, rot.z, rot.w };
			if (DragVector4Compact("WorldRot", rotArr, 0.01f))
			{
				transform->SetRotation({ rotArr[0], rotArr[1], rotArr[2], rotArr[3] });
			}
			ImGui::Unindent();

			ImGui::EndChild();
		}

		ImGui::SameLine();

		if (tab.showLocalTransform)
		{
			ImGui::BeginChild("LocalTransform", ImVec2(childWidth, 180), true);

			ImGui::TextDisabled("Local");
			ImGui::Separator();

			ImGui::Text("Position");
			ImGui::Indent();
			const auto localPos = transform->GetLocalPosition();
			float localPosArr[3] = { localPos.x, localPos.y, localPos.z };
			if (DragVector3Compact("LocalPos", localPosArr, 0.1f))
			{
				transform->SetLocalPosition({ localPosArr[0], localPosArr[1], localPosArr[2] });
			}
			ImGui::Unindent();

			ImGui::Text("Rotation");
			ImGui::Indent();
			const auto localRot = transform->GetLocalRotation();
			float localRotArr[4] = { localRot.x, localRot.y, localRot.z, localRot.w };
			if (DragVector4Compact("LocalRot", localRotArr, 0.01f))
			{
				transform->SetLocalRotation({ localRotArr[0], localRotArr[1], localRotArr[2], localRotArr[3] });
			}
			ImGui::Unindent();

			ImGui::Text("Scale");
			ImGui::Indent();
			const auto scale = transform->GetLocalScale();
			float scaleArr[3] = { scale.x, scale.y, scale.z };
			if (DragVector3Compact("LocalScale", scaleArr, 0.1f))
			{
				transform->SetLocalScale({ scaleArr[0], scaleArr[1], scaleArr[2] });
			}
			ImGui::Unindent();

			ImGui::EndChild();
		}

		ImGui::Unindent();
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
	{
		tab.methodSearchBuffers.resize(componentIndex + 1);
	}
	char* searchBuffer3 = tab.methodSearchBuffers[componentIndex].data();

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
	ImGui::SetNextItemWidth(180);
	ImGui::InputTextWithHint("##MethodSearch", "Search methods...", searchBuffer3, 256);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 8);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::Checkbox("Static", &tab.filterStaticOnly);
	ImGui::SameLine(0, 12);
	ImGui::Checkbox("Instance", &tab.filterInstanceOnly);
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::vector<const ComponentMethodInfo*> filteredMethods;
	for (const auto& method : methods)
	{
		if (PassesMethodFilter(method, searchBuffer3, tab.filterStaticOnly, tab.filterInstanceOnly))
		{
			filteredMethods.push_back(&method);
		}
	}

	if (filteredMethods.empty())
	{
		ImGui::TextDisabled("No methods match the current filters");
		return;
	}

	ImGui::TextDisabled("Showing %zu of %zu methods", filteredMethods.size(), methods.size());
	ImGui::Spacing();

	for (size_t i = 0; i < filteredMethods.size(); ++i)
	{
		const auto* method = filteredMethods[i];
		ImGui::PushID(static_cast<int>(i));

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

		ImGui::BeginChild(method->name.c_str(), ImVec2(0, ImGui::GetFrameHeight() * 4.f), true);

		if (method->isStatic)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
			ImGui::Text("[S] %s", method->name.c_str());
			ImGui::PopStyleColor();
		}
		else if (method->isVirtual)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.5f, 0.9f, 1.0f));
			ImGui::Text("[V] %s", method->name.c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Text("%s", method->name.c_str());
		}

		ImGui::SameLine(0, 12);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.5f, 1.0f));
		ImGui::TextDisabled("-> %s", method->returnTypeName.c_str());
		ImGui::PopStyleColor();

		if (!method->parameters.empty())
		{
			ImGui::Spacing();
			ImGui::TextDisabled("(%zu parameter%s)", method->parameters.size(),
				method->parameters.size() == 1 ? "" : "s");
		}
		else
		{
			ImGui::SameLine(0, 12);
			ImGui::TextDisabled("(no parameters)");
		}

		ImGui::SameLine(0, 12);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50);

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
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::TextDisabled("N/A");
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Details", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoAutoOpenOnLog))
		{
			ImGui::Spacing();

			if (method->parameters.empty())
			{
				ImGui::TextDisabled("No parameters");
			}
			else
			{
				ImGui::TextDisabled("Parameters:");
				ImGui::Indent();
				for (size_t j = 0; j < method->parameters.size(); j++)
				{
					const auto& [paramName, paramType] = method->parameters[j];
					const auto paramEditableType = method->parameterEditableTypes[j];

					ImGui::Text("  %s", paramName.c_str());
					ImGui::SameLine(0, 8);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
					ImGui::TextDisabled("(%s)", paramType.c_str());
					ImGui::PopStyleColor();

					if (paramEditableType == EditableType::None)
					{
						ImGui::SameLine(0, 8);
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
						ImGui::TextDisabled("[not editable]");
						ImGui::PopStyleColor();
					}
				}
				ImGui::Unindent();
			}

			ImGui::TreePop();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		ImGui::Spacing();
		ImGui::PopID();
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
	{
		tab.propertySearchBuffers.resize(componentIndex + 1);
	}
	char* searchBuffer1 = tab.propertySearchBuffers[componentIndex].data();

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
	ImGui::SetNextItemWidth(180);
	ImGui::InputTextWithHint("##PropertySearch", "Search properties...", searchBuffer1, 256);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 8);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::Checkbox("Writable Only", &tab.filterEditableOnly);
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	int filteredCount = 0;
	for (const auto& prop : properties)
	{
		if (!PassesPropertyFilter(prop, searchBuffer1, tab.filterEditableOnly))
			continue;
		filteredCount++;
	}

	if (filteredCount == 0)
	{
		ImGui::TextDisabled("No properties match the current filters");
		return;
	}

	ImGui::TextDisabled("Showing %d of %zu properties", filteredCount, properties.size());
	ImGui::Spacing();

	for (const auto& prop : properties)
	{
		if (!PassesPropertyFilter(prop, searchBuffer1, tab.filterEditableOnly))
			continue;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

		ImGui::BeginChild(prop.name.c_str(), ImVec2(0, ImGui::GetFrameHeight() * 3.f), true);

		ImGui::Text("%s", prop.name.c_str());

		ImGui::SameLine(0, 12);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
		ImGui::TextDisabled("%s", prop.typeName.c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine(0, 12);
		if (prop.canRead && prop.canWrite)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
			ImGui::TextDisabled("[RW]");
			ImGui::PopStyleColor();
		}
		else if (prop.canRead)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.4f, 1.0f));
			ImGui::TextDisabled("[R]");
			ImGui::PopStyleColor();
		}
		else if (prop.canWrite)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
			ImGui::TextDisabled("[W]");
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();
		RenderEditableProperty(component, prop);

		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		ImGui::Spacing();
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
	{
		tab.fieldSearchBuffers.resize(componentIndex + 1);
	}
	char* searchBuffer2 = tab.fieldSearchBuffers[componentIndex].data();

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
	ImGui::SetNextItemWidth(180);
	ImGui::InputTextWithHint("##FieldSearch", "Search fields...", searchBuffer2, 256);
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 8);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::Checkbox("Editable", &tab.filterEditableOnly);
	ImGui::SameLine(0, 12);
	ImGui::Checkbox("Static", &tab.filterStaticOnly);
	ImGui::SameLine(0, 12);
	ImGui::Checkbox("Instance", &tab.filterInstanceOnly);
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	int filteredCount = 0;
	for (const auto& field : fields)
	{
		if (PassesFieldFilter(field, searchBuffer2, tab.filterEditableOnly, tab.filterStaticOnly, tab.filterInstanceOnly))
			filteredCount++;
	}

	if (filteredCount == 0)
	{
		ImGui::TextDisabled("No fields match the current filters");
		return;
	}

	ImGui::TextDisabled("Showing %d of %zu fields", filteredCount, fields.size());
	ImGui::Spacing();

	for (const auto& field : fields)
	{
		if (!PassesFieldFilter(field, searchBuffer2, tab.filterEditableOnly, tab.filterStaticOnly, tab.filterInstanceOnly))
			continue;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

		ImGui::BeginChild(field.name.c_str(), ImVec2(0, ImGui::GetFrameHeight() * 3.f), true);

		if (field.isStatic)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
			ImGui::Text("[S] %s", field.name.c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Text("%s", field.name.c_str());
		}

		ImGui::SameLine(0, 12);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
		ImGui::TextDisabled("%s", field.typeName.c_str());
		ImGui::PopStyleColor();

		ImGui::SameLine(0, 12);
		if (field.isStatic)
		{
			ImGui::TextDisabled("[static]");
		}
		else
		{
			ImGui::TextDisabled("0x%X", field.offset);
		}

		ImGui::Spacing();
		RenderEditableField(component, field);

		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		ImGui::Spacing();
	}
}

void Inspector::RenderComponentsSection(InspectedObjectTab& tab)
{
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##ComponentSearch", "Filter components...", tab.componentSearchBuffer, sizeof(tab.componentSearchBuffer));
	ImGui::PopStyleColor();

	ImGui::Spacing();

	std::vector<size_t> filteredComponentIndices;
	for (size_t i = 0; i < tab.cachedComponents.size(); i++)
	{
		const std::string& compName = tab.cachedComponentNames[i];
		if (PassesComponentFilter(compName, tab.componentSearchBuffer))
		{
			filteredComponentIndices.push_back(i);
		}
	}

	if (filteredComponentIndices.empty())
	{
		ImGui::TextDisabled("No components match the filter");
		return;
	}

	ImGui::TextDisabled("Components: %zu of %zu", filteredComponentIndices.size(), tab.cachedComponents.size());
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));

	if (ImGui::BeginTabBar("ComponentsTabs", ImGuiTabBarFlags_Reorderable))
	{
		for (const size_t idx : filteredComponentIndices)
		{
			const auto comp = tab.cachedComponents[idx];
			const std::string& compName = tab.cachedComponentNames[idx];
			const auto& fields = tab.cachedComponentFields[idx];
			const auto& properties = tab.cachedComponentProperties.size() > idx ? tab.cachedComponentProperties[idx] : std::vector<ComponentPropertyInfo>{};
			const auto& methods = tab.cachedComponentMethods.size() > idx ? tab.cachedComponentMethods[idx] : std::vector<ComponentMethodInfo>{};

			std::string tabLabel = compName;
			if (!fields.empty() || !properties.empty() || !methods.empty())
			{
				tabLabel += "##" + std::to_string(idx);
			}

			if (ImGui::BeginTabItem(tabLabel.c_str()))
			{
				ImGui::Spacing();

				std::string fullName = GetComponentFullTypeName(comp);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.7f, 0.9f, 1.0f));
				ImGui::TextDisabled("Type: %s", fullName.c_str());
				ImGui::PopStyleColor();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::TextDisabled("Fields: %zu | Properties: %zu | Methods: %zu",
					fields.size(), properties.size(), methods.size());

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));

				std::string innerTabBarId = "InnerTabs_" + std::to_string(idx);

				if (ImGui::BeginTabBar(innerTabBarId.c_str()))
				{
					std::string fieldsTabLabel = " Fields";
					if (!fields.empty())
					{
						fieldsTabLabel += " (" + std::to_string(fields.size()) + ")";
					}
					if (ImGui::BeginTabItem(fieldsTabLabel.c_str()))
					{
						ImGui::Spacing();
						RenderFieldsSection(comp, fields, tab, idx);
						ImGui::EndTabItem();
					}

					std::string propsTabLabel = " Properties";
					if (!properties.empty())
					{
						propsTabLabel += " (" + std::to_string(properties.size()) + ")";
					}
					if (ImGui::BeginTabItem(propsTabLabel.c_str()))
					{
						ImGui::Spacing();
						RenderPropertiesSection(comp, properties, tab, idx);
						ImGui::EndTabItem();
					}

					std::string methodsTabLabel = " Methods";
					if (!methods.empty())
					{
						methodsTabLabel += " (" + std::to_string(methods.size()) + ")";
					}
					if (ImGui::BeginTabItem(methodsTabLabel.c_str()))
					{
						ImGui::Spacing();
						RenderMethodsSection(comp, methods, tab, idx);
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::PopStyleColor(3);

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::PopStyleColor(3);
}

void Inspector::RenderDetailsWindow()
{
	if (!showDetailsWindow || openTabs.empty()) return;

	ImGui::SetNextWindowSize(ImVec2(850, 700), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Inspector", &showDetailsWindow))
	{
		if (!pinnedObjects.empty() || !recentSelections.empty())
		{
			ImGui::Text("Quick Access:");
			ImGui::SameLine();

			if (!pinnedObjects.empty())
			{
				if (ImGui::BeginCombo("##Pinned", "Pinned"))
				{
					for (auto* obj : pinnedObjects)
					{
						if (!Core::helper->SafeIsAlive(obj)) continue;

						std::string name = "(Unknown)";
						if (const auto n = obj->GetName()) name = n->ToString();

						if (ImGui::Selectable(name.c_str()))
						{
							OpenObjectInNewTab(obj);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
			}

			if (!recentSelections.empty())
			{
				if (ImGui::BeginCombo("##Recent", "Recent"))
				{
					for (auto* obj : recentSelections)
					{
						if (!Core::helper->SafeIsAlive(obj)) continue;

						std::string name = "(Unknown)";
						if (const auto n = obj->GetName()) name = n->ToString();

						if (ImGui::Selectable(name.c_str()))
						{
							OpenObjectInNewTab(obj);
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Separator();
		}

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
			{
				flags |= ImGuiTabItemFlags_NoCloseButton;
			}

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
	{
		RefreshTabData(tab);
	}
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

	if (!tab.objectPath.empty())
	{
		ImGui::TextWrapped("Path: %s", tab.objectPath.c_str());
		ImGui::Separator();
	}

	std::string objectName = "(Unknown)";

	if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
	{
		UT::String* name = nullptr;
		Core::helper->SafeGetName(tab.gameObject, name);
		if (name)
			objectName = name->ToString();
	}

	ImGui::Text("Name: %s", objectName.c_str());

	if (bool isActive = true; Core::helper->SafeGetActiveSelf(tab.gameObject, isActive))
	{
		ImGui::SameLine();
		if (ImGui::Checkbox("##Active", &isActive))
		{
			Core::helper->SafeSetActive(tab.gameObject, isActive);
		}
		ImGui::SameLine();
		ImGui::Text("Active");
	}

	if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
	{
		UT::String* tag;
		if (const auto validTag = Core::helper->SafeGetTag(tab.gameObject, tag); tag && validTag)
		{
			ImGui::Text("Tag: %s", tag->ToString().c_str());
		}
	}

	if (bool isStatic = false; Helper::SafeGetIsStatic(tab.gameObject, isStatic))
	{
		ImGui::SameLine();
		ImGui::Text("| Static: %s", isStatic ? "Yes" : "No");
	}

	ImGui::SameLine();
	if (IsObjectPinned(tab.gameObject))
	{
		if (ImGui::SmallButton("Unpin"))
		{
			UnpinObject(tab.gameObject);
		}
	}
	else
	{
		if (ImGui::SmallButton("Pin"))
		{
			PinObject(tab.gameObject);
		}
	}

	ImGui::Separator();

	if (ImGui::BeginChild("Content", ImVec2(0, 0), false))
	{
		UT::Transform* transform = nullptr;

		if (tab.gameObject && Core::helper->SafeIsAlive(tab.gameObject))
		{
			Core::helper->SafeGetTransform(tab.gameObject, transform);
		}


		if (transform)
		{
			RenderTransformSection(transform, tab);
		}
		else
		{
			ImGui::TextDisabled("Transform not available (object may be destroyed)");
		}

		ImGui::Spacing();

		RenderComponentsSection(tab);
	}
	ImGui::EndChild();
}
