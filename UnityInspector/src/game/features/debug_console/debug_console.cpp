#include "pch.h"
#include "debug_console.h"

std::deque<LogEntry> DebugConsole::logBuffer;
std::mutex DebugConsole::logMutex;
float DebugConsole::currentTime = 0.0f;

void DebugConsole::Update(float)
{
}

std::string DebugConsole::GetStackTrace()
{
    auto* stackTraceClass = UR::Get("UnityEngine.CoreModule.dll")->Get("StackTraceUtility", "UnityEngine");
    if (!stackTraceClass) return "";
    
    auto* mExtract = stackTraceClass->Get<UR::Method>("ExtractStackTrace");
    if (!mExtract) return "";
    
    auto* result = mExtract->Invoke<UT::String*>();
    if (!result) return "";
    
    return result->ToString();
}

std::string DebugConsole::GetCallingSource()
{
    auto* stackTraceClass = UR::Get("UnityEngine.CoreModule.dll")->Get("StackTrace", "System.Diagnostics");
    if (!stackTraceClass) return "";
    
    auto* mGetFrame = stackTraceClass->Get<UR::Method>("GetFrame", { "System.Int32" });
    if (!mGetFrame) return "";
    
    auto* frame = mGetFrame->Invoke<void*, int>(0);
    if (!frame) return "";
    
    auto* frameClass = UR::Get("UnityEngine.CoreModule.dll")->Get("StackFrame", "System.Diagnostics");
    if (!frameClass) return "";
    
    auto* mGetMethod = frameClass->Get<UR::Method>("GetMethod");
    if (!mGetMethod) return "";
    
    auto* methodInfo = mGetMethod->Invoke<void*, void*>(frame);
    if (!methodInfo) return "";
    
    auto* methodBaseClass = UR::Get("System.dll")->Get("MethodBase", "System.Reflection");
    if (!methodBaseClass) return "";
    
    auto* mGetName = methodBaseClass->Get<UR::Method>("get_Name");
    auto* mGetDeclaringType = methodBaseClass->Get<UR::Method>("get_DeclaringType");
    
    std::string methodName;
    std::string className;
    
    if (mGetName)
    {
        auto* nameStr = mGetName->Invoke<UT::String*, void*>(methodInfo);
        if (nameStr) methodName = nameStr->ToString();
    }
    
    if (mGetDeclaringType)
    {
        auto* type = mGetDeclaringType->Invoke<void*, void*>(methodInfo);
        if (type)
        {
            auto* typeClass = UR::Get("System.dll")->Get("Type", "System");
            if (typeClass)
            {
                auto* mGetFullName = typeClass->Get<UR::Method>("get_FullName");
                if (mGetFullName)
                {
                    auto* fullNameStr = mGetFullName->Invoke<UT::String*, void*>(type);
                    if (fullNameStr) className = fullNameStr->ToString();
                }
            }
        }
    }
    
    if (!className.empty() && !methodName.empty())
        return className + "." + methodName;
    return "";
}

void DebugConsole::Render()
{
    if (!Core::config->inspector.ShowDebugConsole || !Core::config->ShowImGui) return;
    
    RenderConsoleWindow();
}

void DebugConsole::AddLog(const std::string& message, LogType type, const std::string& stackTrace, const std::string& source)
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    logBuffer.emplace_back(message, stackTrace, type, currentTime, source);
    
    while (logBuffer.size() > MAX_LOGS)
    {
        logBuffer.pop_front();
    }
}

void DebugConsole::ClearLogs()
{
    std::lock_guard<std::mutex> lock(logMutex);
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

bool DebugConsole::PassesFilter(const LogEntry& entry) const
{
    if (filterBuffer[0] == '\0') return true;
    
    std::string filter = filterBuffer;
    std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
    
    std::string message = entry.message;
    std::transform(message.begin(), message.end(), message.begin(), ::tolower);
    
    if (message.find(filter) != std::string::npos) return true;
    
    std::string source = entry.source;
    std::transform(source.begin(), source.end(), source.begin(), ::tolower);
    
    return source.find(filter) != std::string::npos;
}

void DebugConsole::RenderLogEntry(const LogEntry& entry, int index)
{
    ImU32 color = GetLogColor(entry.type);
    const char* typeStr = GetLogTypeString(entry.type);
    
    std::string display;
    if (!entry.source.empty())
        display = std::format("[{}] [{:.2f}] [{}] {}", typeStr, entry.timestamp, entry.source, entry.message);
    else
        display = std::format("[{}] [{:.2f}] {}", typeStr, entry.timestamp, entry.message);
    
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    
    bool hasStackTrace = !entry.stackTrace.empty();
    if (hasStackTrace)
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
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("Debug Console", &Core::config->inspector.ShowDebugConsole, windowFlags))
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
    
    ImVec2 available = ImGui::GetContentRegionAvail();
    float logListWidth = selectedLogIndex >= 0 ? available.x * 0.6f : available.x;
    
    ImGui::BeginChild("LogScroll", ImVec2(logListWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    int index = 0;
    for (const auto& entry : logBuffer)
    {
        if (!ShouldShowLogType(entry.type)) continue;
        if (!PassesFilter(entry)) continue;
        
        RenderLogEntry(entry, index++);
    }
    
    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    
    if (selectedLogIndex >= 0 && selectedLogIndex < static_cast<int>(logBuffer.size()))
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
