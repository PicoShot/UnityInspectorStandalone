#pragma once
#include "game/core/core.h"

enum class LogType
{
    Log,
    Warning,
    Error,
    Exception,
    Assert
};

struct LogEntry
{
    std::string message;
    std::string stackTrace;
    std::string source;
    LogType type;
    float timestamp;
    
    LogEntry(const std::string& msg, const std::string& trace, LogType t, float time, const std::string& src = "")
        : message(msg), stackTrace(trace), source(src), type(t), timestamp(time) {}
};

struct DebugConsole : Core::Feature
{   
    void Update(float deltaTime) override;
    void Render() override;
    
    static void AddLog(const std::string& message, LogType type, const std::string& stackTrace = "", const std::string& source = "");
    static void ClearLogs();
    static std::string GetStackTrace();
    static std::string GetCallingSource();
    
private:
    static constexpr size_t MAX_LOGS = 1000;
    static std::deque<LogEntry> logBuffer;
    static std::mutex logMutex;
    static float currentTime;
    
    bool showLog = true;
    bool showWarning = true;
    bool showError = true;
    bool showException = true;
    bool showAssert = true;
    bool autoScroll = true;
    bool showTimestamps = true;
    bool showSource = true;
    bool wordWrap = true;
    int selectedLogIndex = -1;
    
    char filterBuffer[256] = {};
    
    void RenderConsoleWindow();
    void RenderLogEntry(const LogEntry& entry, int index);
    ImU32 GetLogColor(LogType type) const;
    const char* GetLogTypeString(LogType type) const;
    bool ShouldShowLogType(LogType type) const;
    bool PassesFilter(const LogEntry& entry) const;
};
