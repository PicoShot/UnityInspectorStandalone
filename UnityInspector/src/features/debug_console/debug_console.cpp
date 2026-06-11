#include "pch.h"
#include "debug_console.h"
#include "helper/helper.h"

REGISTER_FEATURE(DebugConsole)

std::deque<LogEntry> DebugConsole::logBuffer;
std::mutex DebugConsole::logMutex;
float DebugConsole::currentTime = 0.0f;

void DebugConsole::Update(float)
{
}

std::string DebugConsole::GetStackTrace()
{
	static auto* unityCore = UR::Get("UnityEngine.CoreModule.dll");
	if (!unityCore) return "";

	static auto* stackTraceClass = unityCore->Get("StackTraceUtility", "UnityEngine");
	if (!stackTraceClass) return "";

	static auto* mExtract = stackTraceClass->Get<UR::Method>("ExtractStackTrace");
	if (!mExtract) return "";

	const auto* result = mExtract->Invoke<UT::String*>();
	if (!result) return "";

	return result->ToString();
}

std::string DebugConsole::GetCallingSource()
{
	static auto* unityCore = UR::Get("UnityEngine.CoreModule.dll");
	if (!unityCore) return "";

	static auto* stackTraceClass = unityCore->Get("StackTrace", "System.Diagnostics");
	if (!stackTraceClass) return "";

	static auto* mGetFrame = stackTraceClass->Get<UR::Method>("GetFrame", {"System.Int32"});
	if (!mGetFrame) return "";

	auto* frame = mGetFrame->Invoke<void*, int>(0);
	if (!frame) return "";

	static auto* frameClass = unityCore->Get("StackFrame", "System.Diagnostics");
	if (!frameClass) return "";

	static auto* mGetMethod = frameClass->Get<UR::Method>("GetMethod");
	if (!mGetMethod) return "";

	auto* methodInfo = mGetMethod->Invoke<void*, void*>(frame);
	if (!methodInfo) return "";

	static auto* systemCore = UR::Get("System.dll");
	if (!systemCore) return "";

	static auto* methodBaseClass = systemCore->Get("MethodBase", "System.Reflection");
	if (!methodBaseClass) return "";

	static auto* mGetName = methodBaseClass->Get<UR::Method>("get_Name");
	static auto* mGetDeclaringType = methodBaseClass->Get<UR::Method>("get_DeclaringType");

	static auto* typeClass = systemCore->Get("Type", "System");
	static auto* mGetFullName = typeClass ? typeClass->Get<UR::Method>("get_FullName") : nullptr;

	std::string methodName;
	std::string className;

	if (mGetName)
	{
		if (const auto* nameStr = mGetName->Invoke<UT::String*, void*>(methodInfo)) methodName = nameStr->ToString();
	}

	if (mGetDeclaringType)
	{
		if (auto* type = mGetDeclaringType->Invoke<void*, void*>(methodInfo))
		{
			if (mGetFullName)
			{
				if (const auto* fullNameStr = mGetFullName->Invoke<UT::String*, void*>(type)) className =
					fullNameStr->ToString();
			}
		}
	}

	if (!className.empty() && !methodName.empty())
		return className + "." + methodName;
	return "";
}

void DebugConsole::Render()
{
	if (!Config::settings.inspector.showDebugConsole || !Config::state.showMenu) return;

	RenderConsoleWindow();
}

void DebugConsole::AddLog(const std::string& message, LogType type, const std::string& stackTrace,
                          const std::string& source)
{
	std::scoped_lock lock(logMutex);

	logBuffer.emplace_back(message, stackTrace, type, currentTime, source);

	while (logBuffer.size() > MAX_LOGS)
	{
		logBuffer.pop_front();
	}
}

void DebugConsole::ClearLogs()
{
	std::scoped_lock lock(logMutex);
	logBuffer.clear();
}

ImU32 DebugConsole::GetLogColor(LogType type) const
{
	switch (type)
	{
	case LogType::Log: return IM_COL32(200, 200, 200, 255);
	case LogType::Warning: return IM_COL32(255, 200, 0, 255);
	case LogType::Error: return IM_COL32(255, 50, 50, 255);
	case LogType::Exception: return IM_COL32(255, 0, 0, 255);
	case LogType::Assert: return IM_COL32(255, 100, 100, 255);
	default: return IM_COL32(200, 200, 200, 255);
	}
}

const char* DebugConsole::GetLogTypeString(LogType type) const
{
	switch (type)
	{
	case LogType::Log: return "LOG";
	case LogType::Warning: return "WARN";
	case LogType::Error: return "ERROR";
	case LogType::Exception: return "EXCEPTION";
	case LogType::Assert: return "ASSERT";
	default: return "?";
	}
}

bool DebugConsole::ShouldShowLogType(LogType type) const
{
	switch (type)
	{
	case LogType::Log: return showLog;
	case LogType::Warning: return showWarning;
	case LogType::Error: return showError;
	case LogType::Exception: return showException;
	case LogType::Assert: return showAssert;
	default: return true;
	}
}

bool DebugConsole::PassesFilter(const LogEntry& entry, std::string_view lowerFilter) const
{
	if (lowerFilter.empty()) return true;

	if (Helper::CaseInsensitiveFind(entry.message, lowerFilter)) return true;
	return Helper::CaseInsensitiveFind(entry.source, lowerFilter);
}

void DebugConsole::RenderLogEntry(const LogEntry& entry, int index)
{
	const ImU32 color = GetLogColor(entry.type);
	const char* typeStr = GetLogTypeString(entry.type);

	std::string display;
	if (!entry.source.empty())
		display = std::format("[{}] [{:.2f}] [{}] {}", typeStr, entry.timestamp, entry.source, entry.message);
	else
		display = std::format("[{}] [{:.2f}] {}", typeStr, entry.timestamp, entry.message);

	ImGui::PushStyleColor(ImGuiCol_Text, color);

	if (!entry.stackTrace.empty())
	{
		ImGui::PushID(index);
		if (ImGui::Selectable(display.c_str(), selectedLogIndex == index))
		{
			selectedLogIndex = (selectedLogIndex == index) ? -1 : index;
		}
		ImGui::PopID();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Click to view stack trace");
		}
	}
	else
	{
		ImGui::TextUnformatted(display.c_str());
	}

	ImGui::PopStyleColor();
}

void DebugConsole::RenderConsoleWindow()
{
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

	if (constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar; !ImGui::Begin(
		"Debug Console", &Config::settings.inspector.showDebugConsole, windowFlags))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Show Timestamps", nullptr, &showTimestamps);
			ImGui::MenuItem("Show Source", nullptr, &showSource);
			ImGui::MenuItem("Word Wrap", nullptr, &wordWrap);
			ImGui::Separator();
			if (ImGui::MenuItem("Clear Logs")) ClearLogs();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::Button("Clear"))
	{
		ClearLogs();
		selectedLogIndex = -1;
	}
	ImGui::SameLine();

	ImGui::Checkbox("Auto-scroll", &autoScroll);
	ImGui::SameLine();

	ImGui::PushItemWidth(200);
	ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();

	ImGui::Checkbox("Log", &showLog);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
	ImGui::Checkbox("Warning", &showWarning);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255));
	ImGui::Checkbox("Error", &showError);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	ImGui::Checkbox("Exception", &showException);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::Checkbox("Assert", &showAssert);

	ImGui::Separator();

	const ImVec2 available = ImGui::GetContentRegionAvail();
	const float logListWidth = selectedLogIndex >= 0 ? available.x * 0.6f : available.x;

	ImGui::BeginChild("LogScroll", ImVec2(logListWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

	std::scoped_lock lock(logMutex);

	std::string lowerFilter = filterBuffer;
	std::ranges::transform(lowerFilter, lowerFilter.begin(), ::tolower);

	int index = 0;
	for (const auto& entry : logBuffer)
	{
		if (!ShouldShowLogType(entry.type)) continue;
		if (!PassesFilter(entry, lowerFilter)) continue;

		RenderLogEntry(entry, index++);
	}

	if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();

	if (selectedLogIndex >= 0 && std::cmp_less(selectedLogIndex, logBuffer.size()))
	{
		ImGui::SameLine();
		ImGui::BeginChild("StackTracePanel", ImVec2(0, 0), true);

		auto it = logBuffer.begin();
		std::advance(it, selectedLogIndex);

		ImGui::Text("Source:");
		ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "%s", it->source.empty() ? "Unknown" : it->source.c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Stack Trace:");
		if (it->stackTrace.empty())
		{
			ImGui::TextDisabled("No stack trace available");
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			if (wordWrap)
			{
				ImGui::TextWrapped("%s", it->stackTrace.c_str());
			}
			else
			{
				ImGui::TextUnformatted(it->stackTrace.c_str());
			}
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();
	}

	ImGui::End();
}
