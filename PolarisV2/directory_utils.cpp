#include "directory_utils.h"
#include "error_utils.h"
#include "sdk_utils.h"

#include <windows.h>

namespace polaris
{
    namespace utilities
    {
        std::string DirectoryUtils::GetConcatPath(const std::string& sFirst, const std::string& sSecond)
        {
            std::string sTemp = sFirst;

            if (sFirst[sFirst.length()] != '\\')
            {
                sTemp += '\\';

                return(sTemp + sSecond);
            }
            else
                return(sFirst + sSecond);
        }
        std::string DirectoryUtils::GetFontsDir()
        {
            char winDir[MAX_PATH];
            if (!GetWindowsDirectoryA(winDir, sizeof(winDir)))
                ErrorUtils::ThrowException(L"An unknown platform exception occured.");

            return GetConcatPath(winDir, "Fonts");
        }
    }
}