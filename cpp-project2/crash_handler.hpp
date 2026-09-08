#include <string>
#include <windows.h>
#include <winnt.h>

class CrashHandler {
public:
    CrashHandler();
    ~CrashHandler();

private:
    
    bool writeMinidump(EXCEPTION_POINTERS* exceptionPointers);
    LONG WINAPI crashHandler(EXCEPTION_POINTERS* exceptionPointers);

    static CrashHandler* ptr;
    static LONG WINAPI crashHandlerStatic(EXCEPTION_POINTERS* exceptionPointers);
};