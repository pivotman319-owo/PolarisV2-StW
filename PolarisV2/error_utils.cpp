#include "error_utils.h"

#include <string>

namespace polaris
{
    namespace utilities
    {
        void ErrorUtils::ThrowException(LPCWSTR message)
        {
            LPCWSTR base = L"An unhandled exception occurred during the execution. Please review the logs for more information about the error and where it origininated.\n\nException Details:\n";
            std::wstring combined = std::wstring(base) + message;
            LPCWSTR combinedLPC = combined.c_str();

            MessageBox(0, combinedLPC, L"Error", MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }
    }
}