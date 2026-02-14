#include "pch.h"
#include "hooks.h"
#include "game/features/debug_console/debug_console.h"
//#include "game/core/core.h"


void UNITY_CALLING_CONVENTION HDebugLogObject(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Log, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogObject, message);
}

void UNITY_CALLING_CONVENTION HDebugLogString(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Log, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogString, message);
}

void UNITY_CALLING_CONVENTION HDebugLogFormat(void* message, void* args)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Log, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogFormat, message, args);
}

void UNITY_CALLING_CONVENTION HDebugLogWarningObject(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Warning, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogWarningObject, message);
}

void UNITY_CALLING_CONVENTION HDebugLogWarningString(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Warning, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogWarningString, message);
}

void UNITY_CALLING_CONVENTION HDebugLogErrorObject(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Error, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogErrorObject, message);
}

void UNITY_CALLING_CONVENTION HDebugLogErrorString(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Error, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogErrorString, message);
}

void UNITY_CALLING_CONVENTION HDebugLogException(void* exception)
{
    if (exception)
    {
        auto* str = (UT::String*)exception;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Exception, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogException, exception);
}

void UNITY_CALLING_CONVENTION HDebugLogAssertion(void* message)
{
    if (message)
    {
        auto* str = (UT::String*)message;
        std::string msg = str->ToString();
        std::string stackTrace = DebugConsole::GetStackTrace();
        std::string source = DebugConsole::GetCallingSource();
        DebugConsole::AddLog(msg, LogType::Assert, stackTrace, source);
    }
    HookManager::Fcall(HDebugLogAssertion, message);
}

void Hooks::Init()
{
    auto* debugClass = UR::Get("UnityEngine.CoreModule.dll")->Get("Debug", "UnityEngine");
    if (!debugClass) return;

    if (auto* mLog = debugClass->Get<UR::Method>("Log", { "System.Object" }))
    {
        if (auto* casted = mLog->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogObject);
    }

    if (auto* mLogStr = debugClass->Get<UR::Method>("Log", { "System.String" }))
    {
        if (auto* casted = mLogStr->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogString);
    }

    if (auto* mWarn = debugClass->Get<UR::Method>("LogWarning", { "System.Object" }))
    {
        if (auto* casted = mWarn->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogWarningObject);
    }

    if (auto* mWarnStr = debugClass->Get<UR::Method>("LogWarning", { "System.String" }))
    {
        if (auto* casted = mWarnStr->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogWarningString);
    }

    if (auto* mError = debugClass->Get<UR::Method>("LogError", { "System.Object" }))
    {
        if (auto* casted = mError->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogErrorObject);
    }

    if (auto* mErrorStr = debugClass->Get<UR::Method>("LogError", { "System.String" }))
    {
        if (auto* casted = mErrorStr->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogErrorString);
    }

    if (auto* mExc = debugClass->Get<UR::Method>("LogException", { "System.Exception" }))
    {
        if (auto* casted = mExc->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogException);
    }

    if (auto* mAssert = debugClass->Get<UR::Method>("LogAssertion", { "System.Object" }))
    {
        if (auto* casted = mAssert->Cast<void, void*>())
            HookManager::Install(casted, HDebugLogAssertion);
    }
}