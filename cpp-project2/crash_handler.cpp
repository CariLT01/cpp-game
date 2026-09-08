#include "crash_handler.hpp"
#include <dbghelp.h>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <winnt.h>

#pragma comment(lib, "dbghelp.lib")

CrashHandler* CrashHandler::ptr = nullptr;

CrashHandler::CrashHandler() {
    ptr = this;

    SetUnhandledExceptionFilter(CrashHandler::crashHandlerStatic);
}

CrashHandler::~CrashHandler() {}

bool CrashHandler::writeMinidump(EXCEPTION_POINTERS *exceptionPointers) {
    const std::string pathStr("dump.dmp");
    const std::wstring path(pathStr.begin(), pathStr.end());

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo{};
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;

    const auto dumpType = static_cast<MINIDUMP_TYPE>(
        MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

    const BOOL result =
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                          dumpType, &exceptionInfo, nullptr, nullptr);

    CloseHandle(file);

    return result == TRUE;
}

LONG WINAPI CrashHandler::crashHandler(EXCEPTION_POINTERS *exceptionPointers) {
    const bool dumpWritten = writeMinidump(exceptionPointers);

    const std::wstring message =
        dumpWritten ? L"Oops! Game crashed lol.\n\n"
                      L"A crash dump was saved."
                    : L"Oops! Game crashed lol.\n\n"
                      L"Failed to create a crash dump.";

    MessageBoxW(nullptr, message.c_str(), L"Application Crash",
                MB_OK | MB_ICONERROR);

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashHandler::crashHandlerStatic(EXCEPTION_POINTERS* exceptionPointers) {
    CrashHandler* inst = ptr;
    return inst->crashHandler(exceptionPointers);
}