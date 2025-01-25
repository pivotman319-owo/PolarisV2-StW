#ifndef ERROR_UTILS_H
#define ERROR_UTILS_H

#include <windows.h>

namespace polaris
{
    namespace utilities
    {
        //!  Utilities for Error handling.
        class ErrorUtils
        {
        public:
            static void ThrowException(LPCWSTR message); //!< ThrowException pauses execution and prompts the user to restart the game.
        };
    }
}

#endif // !ERROR_UTILS_H
