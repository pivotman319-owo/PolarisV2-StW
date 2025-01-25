#include <Windows.h>

#include "program.h"

static polaris::Program* pProgram;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason,
    LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        pProgram = new polaris::Program;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        delete pProgram;

    return TRUE;
}