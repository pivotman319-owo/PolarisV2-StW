#ifndef DIRECTORY_UTILS_H
#define DIRECTORY_UTILS_H

#include <string>

namespace polaris
{
    namespace utilities
    {
        //!  Utilities for getting Windows Directory paths
        class DirectoryUtils
        {
        public:
            static std::string GetConcatPath(const std::string& sFirst, const std::string& sSecond); //!< GetConcatPath links 2 paths together.
            static std::string GetFontsDir(); //!< GetFontsDir returns the Windows Fonts directory's path.
        };
    }
}

#endif // !DIRECTORY_UTILS_H
