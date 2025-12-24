#include "pch.h"
#include "inspector.h"

void Inspector::RenderMethodInvokePopup()
{
	if (!invokeState.showPopup) return;

	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

	std::string title = "Invoke: " + invokeState.method.name + "###MethodInvoke";
	if (ImGui::Begin(title.c_str(), &invokeState.showPopup))
	{
		ImGui::Text("Method: %s", invokeState.method.name.c_str());
		ImGui::Text("Return Type: %s", invokeState.method.returnTypeName.c_str());
		if (invokeState.method.isStatic)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "[Static]");
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
				const auto& [name, typeName] = invokeState.method.parameters[i];
				const auto paramType = invokeState.method.parameterEditableTypes[i];

				ImGui::PushID(static_cast<int>(i));

				std::string label = name + " (" + typeName + ")";
				ImGui::Text("%s", label.c_str());
				ImGui::SameLine();

				char buf[256] = {};
				if (i < invokeState.parameterValues.size() && !invokeState.parameterValues[i].empty())
				{
					strncpy_s(buf, invokeState.parameterValues[i].c_str(), sizeof(buf) - 1);
				}

				ImGui::SetNextItemWidth(-1);
				switch (paramType)
				{
				case EditableType::Int:
				case EditableType::Float:
				case EditableType::Double:
					if (ImGui::InputText("##param", buf, sizeof(buf), ImGuiInputTextFlags_CharsDecimal))
					{
						invokeState.parameterValues[i] = buf;
					}
					break;
				case EditableType::Bool:
				{
					bool val = (invokeState.parameterValues[i] == "true" || invokeState.parameterValues[i] == "1");
					if (ImGui::Checkbox("##param", &val))
					{
						invokeState.parameterValues[i] = val ? "true" : "false";
					}
					break;
				}
				default:
					if (ImGui::InputText("##param", buf, sizeof(buf)))
					{
						invokeState.parameterValues[i] = buf;
					}
					break;
				}

				ImGui::PopID();
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Invoke", ImVec2(100, 0)))
		{
			void* result = InvokeMethod(invokeState.targetComponent, invokeState.method, invokeState.parameterValues);

			invokeState.hasResult = true;
			if (result)
			{
				if (invokeState.method.returnTypeName == "void" || invokeState.method.returnTypeName == "System.Void")
				{
					invokeState.resultText = "(void)";
				}
				else
				{
					const EditableType retType = DetermineEditableType(invokeState.method.returnTypeName);
					void* unboxed = UR::Invoke<void*, void*>(
						Core::config->gameMode == UnityResolve::Mode::Mono ? "mono_object_unbox" : "il2cpp_object_unbox", result);

					if (unboxed)
					{
						switch (retType)
						{
						case EditableType::Int:
							invokeState.resultText = std::to_string(*static_cast<int*>(unboxed));
							break;
						case EditableType::Float:
							invokeState.resultText = std::to_string(*static_cast<float*>(unboxed));
							break;
						case EditableType::Double:
							invokeState.resultText = std::to_string(*static_cast<double*>(unboxed));
							break;
						case EditableType::Bool:
							invokeState.resultText = *static_cast<bool*>(unboxed) ? "true" : "false";
							break;
						default:
							invokeState.resultText = std::format("(object: 0x{:X})", reinterpret_cast<uintptr_t>(result));
							break;
						}
					}
					else
					{
						invokeState.resultText = std::format("(object: 0x{:X})", reinterpret_cast<uintptr_t>(result));
					}
				}
			}
			else
			{
				if (invokeState.method.returnTypeName == "void" || invokeState.method.returnTypeName == "System.Void")
				{
					invokeState.resultText = "(completed)";
				}
				else
				{
					invokeState.resultText = "(null or error)";
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
		}
	}
	ImGui::End();
}