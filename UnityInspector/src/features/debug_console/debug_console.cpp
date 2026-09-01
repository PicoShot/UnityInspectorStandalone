#include "pch.h"
#include "debug_console.h"

void DebugConsole::AddLog(const std::string& message, const LogType type)
{
	switch (type)
	{
	case LogType::Log:
		LOG_INFO("[Unity] {}", message);
		break;
	case LogType::Warning:
		LOG_WARNING("[Unity] {}", message);
		break;
	case LogType::Error:
	case LogType::Exception:
	case LogType::Assert:
		LOG_ERROR("[Unity] {}", message);
		break;
	}
}
