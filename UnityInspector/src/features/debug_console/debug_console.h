#pragma once

enum class LogType : uint8_t
{
	Log,
	Warning,
	Error,
	Exception,
	Assert
};

class DebugConsole
{
public:
	static void AddLog(const std::string& message, LogType type);
};
