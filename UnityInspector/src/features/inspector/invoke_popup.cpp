#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

void Inspector::RenderMethodInvokePopup()
{
	if (!invokeState.showPopup) return;

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Invoke Method###MethodInvoke", &invokeState.showPopup, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Method: %s", invokeState.method.name.c_str());
		ImGui::Text("Return Type: %s", invokeState.method.returnTypeName.c_str());
		if (invokeState.method.isStatic)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(static)");
		}
		if (invokeState.method.isVirtual)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(virtual)");
		}

		ImGui::Separator();

		if (invokeState.method.parameters.empty())
		{
			ImGui::TextDisabled("No parameters");
		}
		else
		{
			ImGui::Text("Parameters:");
			for (size_t i = 0; i < invokeState.method.parameters.size(); i++)
			{
				ImGui::PushID(static_cast<int>(i));

				const auto& [name, typeName] = invokeState.method.parameters[i];
				std::string label = std::format("{} ({})", name, typeName);
				ImGui::Text("%s", label.c_str());

				const auto paramType = invokeState.method.parameterEditableTypes[i];

				char buf[256] = {};
				if (i < invokeState.parameterValues.size() && !invokeState.parameterValues[i].empty())
					strncpy_s(buf, invokeState.parameterValues[i].c_str(), sizeof(buf) - 1);

				ImGui::PushItemWidth(-1);
				switch (paramType)
				{
				case EditableType::Int:
				case EditableType::Float:
				case EditableType::Double:
				case EditableType::Decimal:
					if (ImGui::InputText("##param", buf, sizeof(buf), ImGuiInputTextFlags_CharsDecimal))
					{
						if (i < invokeState.parameterValues.size())
							invokeState.parameterValues[i] = buf;
					}
					break;
				case EditableType::Bool:
				{
					bool val = (buf[0] == '1' || buf[0] == 't' || buf[0] == 'T');
					if (ImGui::Checkbox("##param", &val))
					{
						if (i < invokeState.parameterValues.size())
							invokeState.parameterValues[i] = val ? "true" : "false";
					}
					break;
				}
				case EditableType::Enum:
				{
					const std::string enumTypeName = invokeState.method.parameters[i].second;
					if (const auto enumVals = GetEnumValues(enumTypeName); !enumVals.empty())
					{
						int currentVal = 0;
						try { currentVal = std::stoi(buf); } catch (...) {}
						int selectedIdx = 0;
						for (size_t j = 0; j < enumVals.size(); j++)
						{
							if (enumVals[j].second == currentVal)
							{
								selectedIdx = static_cast<int>(j);
								break;
							}
						}
						std::string comboLabel = std::format("{} ({})", enumVals[selectedIdx].first, enumVals[selectedIdx].second);
						if (ImGui::BeginCombo("##param", comboLabel.c_str()))
						{
							for (size_t j = 0; j < enumVals.size(); j++)
							{
								bool isSelected = (j == selectedIdx);
								std::string itemLabel = std::format("{} ({})", enumVals[j].first, enumVals[j].second);
								if (ImGui::Selectable(itemLabel.c_str(), isSelected))
								{
									if (i < invokeState.parameterValues.size())
										invokeState.parameterValues[i] = std::to_string(enumVals[j].second);
								}
								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}
					else
					{
						if (ImGui::InputText("##param", buf, sizeof(buf)))
						{
							if (i < invokeState.parameterValues.size())
								invokeState.parameterValues[i] = buf;
						}
					}
					break;
				}
				default:
					if (ImGui::InputText("##param", buf, sizeof(buf)))
					{
						if (i < invokeState.parameterValues.size())
							invokeState.parameterValues[i] = buf;
					}
					break;
				}
				ImGui::PopItemWidth();

				ImGui::PopID();
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Invoke", ImVec2(100, 0)))
		{
			void* result = InvokeMethod(invokeState.targetInstance, invokeState.method, invokeState.parameterValues);

			invokeState.hasResult = true;
			invokeState.resultPointer = nullptr;
			invokeState.resultEditableType = EditableType::None;
			if (result)
			{
				if (invokeState.method.returnTypeName == "void" || invokeState.method.returnTypeName == "System.Void")
				{
					invokeState.resultText = "(void)";
				}
				else
				{
					const std::string returnTypeName = invokeState.method.returnTypeName;
					const EditableType retType = DetermineEditableType(returnTypeName);

					if (void* unboxed = UR::Invoke<void*, void*>(
						Config::state.unityMode == UnityResolve::Mode::Mono ? "mono_object_unbox" : "il2cpp_object_unbox",
						result))
					{
						switch (retType)
						{
						case EditableType::Int:
							invokeState.resultText = std::to_string(*static_cast<int32_t*>(unboxed));
							break;
						case EditableType::Float:
							invokeState.resultText = std::format("{:.6f}", *static_cast<float*>(unboxed));
							break;
						case EditableType::Double:
							invokeState.resultText = std::format("{:.6f}", *static_cast<double*>(unboxed));
							break;
						case EditableType::Bool:
							invokeState.resultText = *static_cast<bool*>(unboxed) ? "true" : "false";
							break;
						case EditableType::String:
						{
							if (const UT::String* str = *static_cast<UT::String**>(unboxed))
								invokeState.resultText = str->ToString();
							else
								invokeState.resultText = "null";
							break;
						}
						case EditableType::Decimal:
							invokeState.resultText = std::format("{:.6f}", *static_cast<float*>(unboxed));
							break;
						case EditableType::Enum:
							invokeState.resultText = std::to_string(*static_cast<int32_t*>(unboxed));
							break;
						case EditableType::Vector2:
						{
							const auto& v = *static_cast<UT::Vector2*>(unboxed);
							invokeState.resultText = std::format("({:.3f}, {:.3f})", v.x, v.y);
							break;
						}
						case EditableType::Vector3:
						{
							const auto& v = *static_cast<UT::Vector3*>(unboxed);
							invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
							break;
						}
						case EditableType::Vector4:
						{
							const auto& v = *static_cast<UT::Vector4*>(unboxed);
							invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
							break;
						}
						case EditableType::Quaternion:
						{
							const auto& v = *static_cast<UT::Quaternion*>(unboxed);
							invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
							break;
						}
						case EditableType::Color:
						{
							const auto& v = *static_cast<UT::Color*>(unboxed);
							invokeState.resultText = std::format("RGBA({:.2f}, {:.2f}, {:.2f}, {:.2f})", v.r, v.g, v.b, v.a);
							break;
						}
						default:
							invokeState.resultText = std::format("0x{:X}", reinterpret_cast<uintptr_t>(unboxed));
							invokeState.resultPointer = unboxed;
							invokeState.resultEditableType = retType;
							break;
						}
					}
					else
					{
						invokeState.resultText = std::format("0x{:X}", reinterpret_cast<uintptr_t>(result));
					}
				}
			}
			else
			{
				invokeState.resultText = "(null)";
				if (!invokeState.method.returnTypeName.empty() &&
					invokeState.method.returnTypeName != "void" &&
					invokeState.method.returnTypeName != "System.Void")
				{
					invokeState.resultText += " or error";
				}
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Close", ImVec2(100, 0)))
		{
			invokeState.showPopup = false;
		}

		if (invokeState.hasResult)
		{
			ImGui::Separator();
			ImGui::Text("Result: %s", invokeState.resultText.c_str());
			if (invokeState.resultPointer && invokeState.resultEditableType == EditableType::CustomObject)
			{
				ImGui::SameLine();
				if (ImGui::SmallButton("Enter"))
				{
					if (auto activeTab = GetActiveTab())
					{
						void* klass = Helper::SafeGetObjectClass(invokeState.resultPointer);
						InspectionTarget nextTarget;
						nextTarget.gameObject = nullptr;
						nextTarget.instance = invokeState.resultPointer;
						nextTarget.classHandle = klass;
						nextTarget.name = invokeState.method.returnTypeName;

						nextTarget.cachedComponents.push_back(static_cast<UT::Component*>(invokeState.resultPointer));
						nextTarget.cachedComponentNames.push_back(invokeState.method.returnTypeName);

						nextTarget.cachedComponentFields.push_back(GetObjectFields(invokeState.resultPointer, klass));
						nextTarget.cachedComponentProperties.push_back(GetObjectProperties(invokeState.resultPointer, klass));
						nextTarget.cachedComponentMethods.push_back(GetObjectMethods(invokeState.resultPointer, klass));

						activeTab->navigationStack.push_back(std::move(nextTarget));
					}
				}
			}
		}
	}
	ImGui::End();
}
