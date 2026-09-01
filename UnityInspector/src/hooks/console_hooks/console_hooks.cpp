#include "pch.h"
#include "console_hooks.h"

REGISTER_HOOK(ConsoleHooks)

namespace
{
	enum class LogType
	{
		Log,
		Warning,
		Error,
		Exception,
		Assert
	};

	void AddLog(const std::string &message, const LogType type)
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

	void UNITY_CALLING_CONVENTION HDebugLogObject(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Log);
		}
		HookManager::Fcall(HDebugLogObject, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogString(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Log);
		}
		HookManager::Fcall(HDebugLogString, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogFormat(void *message, void *args)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Log);
		}
		HookManager::Fcall(HDebugLogFormat, message, args);
	}

	void UNITY_CALLING_CONVENTION HDebugLogWarningObject(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Warning);
		}
		HookManager::Fcall(HDebugLogWarningObject, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogWarningString(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Warning);
		}
		HookManager::Fcall(HDebugLogWarningString, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogErrorObject(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Error);
		}
		HookManager::Fcall(HDebugLogErrorObject, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogErrorString(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Error);
		}
		HookManager::Fcall(HDebugLogErrorString, message);
	}

	void UNITY_CALLING_CONVENTION HDebugLogException(void *exception)
	{
		if (exception)
		{
			const auto *str = static_cast<UT::String *>(exception);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Exception);
		}
		HookManager::Fcall(HDebugLogException, exception);
	}

	void UNITY_CALLING_CONVENTION HDebugLogAssertion(void *message)
	{
		if (message)
		{
			const auto *str = static_cast<UT::String *>(message);
			const std::string msg = str->ToString();
			AddLog(msg, LogType::Assert);
		}
		HookManager::Fcall(HDebugLogAssertion, message);
	}
}

void ConsoleHooks::Install()
{
	const auto *unityCoreModule = UR::Get("UnityEngine.CoreModule.dll");
	if (!unityCoreModule)
		return;

	auto *debugClass = unityCoreModule->Get("Debug", "UnityEngine");
	if (!debugClass)
		return;

	if (auto *mLog = debugClass->Get<UR::Method>("Log", {"System.Object"}))
	{
		if (auto *casted = mLog->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogObject);
	}

	if (auto *mLogStr = debugClass->Get<UR::Method>("Log", {"System.String"}))
	{
		if (auto *casted = mLogStr->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogString);
	}

	if (auto *mWarn = debugClass->Get<UR::Method>("LogWarning", {"System.Object"}))
	{
		if (auto *casted = mWarn->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogWarningObject);
	}

	if (auto *mWarnStr = debugClass->Get<UR::Method>("LogWarning", {"System.String"}))
	{
		if (auto *casted = mWarnStr->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogWarningString);
	}

	if (auto *mError = debugClass->Get<UR::Method>("LogError", {"System.Object"}))
	{
		if (auto *casted = mError->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogErrorObject);
	}

	if (auto *mErrorStr = debugClass->Get<UR::Method>("LogError", {"System.String"}))
	{
		if (auto *casted = mErrorStr->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogErrorString);
	}

	if (auto *mExc = debugClass->Get<UR::Method>("LogException", {"System.Exception"}))
	{
		if (auto *casted = mExc->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogException);
	}

	if (auto *mAssert = debugClass->Get<UR::Method>("LogAssertion", {"System.Object"}))
	{
		if (auto *casted = mAssert->Cast<void, void *>())
			HookManager::Install(casted, HDebugLogAssertion);
	}
}
