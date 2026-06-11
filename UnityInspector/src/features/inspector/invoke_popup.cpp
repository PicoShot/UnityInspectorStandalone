#include "pch.h"
#include "inspector.h"
#include "helper/helper.h"

void Inspector::RenderMethodInvokePopup()
{
	if (!invokeState.showPopup) return;

	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

	const std::string title = "Invoke: " + invokeState.method.name + "###MethodInvoke";
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

				std::string label = std::format("{} ({})", name, typeName);
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
				case EditableType::Decimal:
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
				case EditableType::Enum:
					{
						const std::string& enumTypeName = invokeState.method.parameters[i].second;
						const auto enumVals = GetEnumValues(enumTypeName);
						if (!enumVals.empty())
						{
							int currentIdx = 0;
							const int currentVal = std::atoi(invokeState.parameterValues[i].c_str());
							for (size_t j = 0; j < enumVals.size(); j++)
							{
								if (enumVals[j].second == currentVal)
								{
									currentIdx = static_cast<int>(j);
									break;
								}
							}
							std::vector<const char*> names;
							for (const auto& kv : enumVals) names.push_back(kv.first.c_str());
							if (ImGui::Combo("##param", &currentIdx, names.data(), static_cast<int>(names.size())))
							{
								invokeState.parameterValues[i] = std::to_string(enumVals[currentIdx].second);
							}
						}
						else
						{
							if (ImGui::InputText("##param", buf, sizeof(buf)))
								invokeState.parameterValues[i] = buf;
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
					const EditableType retType = DetermineEditableType(invokeState.method.returnTypeName);
					invokeState.resultPointer = result;
					invokeState.resultEditableType = retType;
					void* unboxed = UR::Invoke<void*, void*>(
						Config::state.unityMode == UnityResolve::Mode::Mono
							? "mono_object_unbox"
							: "il2cpp_object_unbox", result);

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
				case EditableType::String:
					{
						if (const auto* str = static_cast<UT::String*>(result))
						{
							invokeState.resultText = "\"" + str->ToString() + "\"";
						}
						else
						{
							invokeState.resultText = "(null)";
						}
						break;
					}
				case EditableType::Decimal:
					{
						const auto* parts = static_cast<int32_t*>(unboxed);
						const int scale = (parts[0] >> 16) & 0x1F;
						const bool negative = (parts[0] & 0x80000000) != 0;
						const int64_t lo = static_cast<uint32_t>(parts[2]);
						const int64_t mid = static_cast<uint32_t>(parts[3]);
						const int64_t hi = static_cast<uint32_t>(parts[1]);
						const double unscaled = static_cast<double>(lo) + static_cast<double>(mid) * 4294967296.0 +
							static_cast<double>(hi) * 18446744073709551616.0;
						const double value = unscaled / std::pow(10.0, scale) * (negative ? -1.0 : 1.0);
						invokeState.resultText = std::format("{:.6f}", value);
						break;
					}
				case EditableType::Enum:
					{
						const int val = *static_cast<int*>(unboxed);
						const auto& retTypeName = invokeState.method.returnTypeName;
						const auto enumVals = GetEnumValues(retTypeName);
						std::string enumName;
						for (const auto& [fst, snd] : enumVals)
						{
							if (snd == val) { enumName = fst; break; }
						}
						if (!enumName.empty())
							invokeState.resultText = std::format("{} ({})", enumName, val);
						else
							invokeState.resultText = std::to_string(val);
						break;
					}
				case EditableType::Vector2:
					{
						const auto* v = static_cast<float*>(unboxed);
						invokeState.resultText = std::format("({:.3f}, {:.3f})", v[0], v[1]);
						break;
					}
				case EditableType::Vector3:
					{
						const auto* v = static_cast<float*>(unboxed);
						invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f})", v[0], v[1], v[2]);
						break;
					}
				case EditableType::Vector4:
					{
						const auto* v = static_cast<float*>(unboxed);
						invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v[0], v[1], v[2], v[3]);
						break;
					}
				case EditableType::Quaternion:
					{
						const auto* v = static_cast<float*>(unboxed);
						invokeState.resultText = std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v[0], v[1], v[2], v[3]);
						break;
					}
				case EditableType::Color:
					{
						const auto* v = static_cast<float*>(unboxed);
						invokeState.resultText = std::format("RGBA({:.2f}, {:.2f}, {:.2f}, {:.2f})", v[0], v[1], v[2], v[3]);
						break;
					}
					default:
							invokeState.resultText = std::format("(object: 0x{:X})",
							                                     reinterpret_cast<uintptr_t>(result));
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
			if (invokeState.resultPointer && invokeState.resultEditableType == EditableType::CustomObject)
			{
				ImGui::SameLine();
				if (ImGui::SmallButton("Enter"))
				{
					if (auto activeTab = GetActiveTab())
					{
						void* klass = Helper::SafeGetObjectClass(invokeState.resultPointer);
						InspectionTarget nextTarget;
						nextTarget.instance = invokeState.resultPointer;
						nextTarget.name = invokeState.method.name + "()";
						nextTarget.classHandle = klass;

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
