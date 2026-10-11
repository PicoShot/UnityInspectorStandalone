#include "pch.h"
#include "debug_console.h"
#include "helper/helper.h"

REGISTER_FEATURE(DebugConsole)

std::deque<LogEntry> DebugConsole::logBuffer;
std::mutex DebugConsole::logMutex;
size_t DebugConsole::droppedLogs = 0;

void DebugConsole::Update(float)
{
}

std::string DebugConsole::GetStackTrace()
{
	static UR::Method* mExtract = []() -> UR::Method*
	{
		const auto* unityCore = UR::Get("UnityEngine.CoreModule.dll");
		auto* stackTraceClass = unityCore ? unityCore->Get("StackTraceUtility", "UnityEngine") : nullptr;
		return stackTraceClass ? stackTraceClass->Get<UR::Method>("ExtractStackTrace") : nullptr;
	}();
	if (!mExtract) return "";

	const auto* result = mExtract->Invoke<UT::String*>();
	return result ? result->ToString() : "";
}

void DebugConsole::CaptureManagedLog(void* message, const LogType type)
{
	thread_local bool capturing = false;
	if (capturing) return;
	capturing = true;

	try
	{
		std::string stackTrace = GetStackTrace();
		std::string source = stackTrace.substr(0, stackTrace.find_first_of(" (\r\n"));
		AddLog(Helper::SafeObjectToString(message), type, stackTrace, source);
	}
	catch (...)
	{
	}
	capturing = false;
}

void DebugConsole::Render()
{
	if (!Config::settings.inspector.showDebugConsole || !Config::state.showMenu) return;

	RenderConsoleWindow();
}

void DebugConsole::AddLog(const std::string& message, LogType type, const std::string& stackTrace,
                          const std::string& source)
{
	static const auto start = std::chrono::steady_clock::now();
	const float timestamp = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();

	std::scoped_lock lock(logMutex);
	logBuffer.emplace_back(message, stackTrace, type, timestamp, source);

	while (logBuffer.size() > MAX_LOGS)
	{
		logBuffer.pop_front();
		++droppedLogs;
	}
}

void DebugConsole::ClearLogs()
{
	std::scoped_lock lock(logMutex);
	droppedLogs += logBuffer.size();
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

void DebugConsole::RenderLogEntry(const LogEntry& entry, const size_t id)
{
	const std::string_view firstLine = std::string_view(entry.message).substr(0, entry.message.find('\n'));

	std::string display = std::format("[{}] ", GetLogTypeString(entry.type));
	if (showTimestamps) display += std::format("[{:.2f}] ", entry.timestamp);
	if (showSource && !entry.source.empty()) display += std::format("[{}] ", entry.source);
	display += firstLine;

	ImGui::PushStyleColor(ImGuiCol_Text, GetLogColor(entry.type));
	ImGui::PushID(static_cast<int>(id));
	if (ImGui::Selectable(display.c_str(), selectedLogId == id)) selectedLogId = selectedLogId == id ? SIZE_MAX : id;
	ImGui::PopID();
	ImGui::PopStyleColor();
}

void DebugConsole::RenderConsoleWindow()
{
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

	if (constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
	    !ImGui::Begin("Debug Console", &Config::settings.inspector.showDebugConsole, windowFlags))
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
		selectedLogId = SIZE_MAX;
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

	std::scoped_lock lock(logMutex);
	const bool hasSelection = selectedLogId >= droppedLogs && selectedLogId - droppedLogs < logBuffer.size();

	const ImVec2 available = ImGui::GetContentRegionAvail();
	const float logListWidth = hasSelection ? available.x * 0.6f : available.x;

	ImGui::BeginChild("LogScroll", ImVec2(logListWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

	std::string lowerFilter = filterBuffer;
	std::ranges::transform(lowerFilter, lowerFilter.begin(), tolower);

	std::vector<size_t> visible;
	visible.reserve(logBuffer.size());
	for (size_t i = 0; i < logBuffer.size(); ++i)
	{
		if (ShouldShowLogType(logBuffer[i].type) && PassesFilter(logBuffer[i], lowerFilter)) visible.push_back(i);
	}

	ImGuiListClipper clipper;
	clipper.Begin(static_cast<int>(visible.size()));
	while (clipper.Step())
	{
		for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			RenderLogEntry(logBuffer[visible[row]], droppedLogs + visible[row]);
	}

	if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();

	if (hasSelection)
	{
		ImGui::SameLine();
		ImGui::BeginChild("StackTracePanel", ImVec2(0, 0), true);

		const auto it = logBuffer.begin() + static_cast<std::ptrdiff_t>(selectedLogId - droppedLogs);

		ImGui::TextWrapped("%s", it->message.c_str());
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

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
