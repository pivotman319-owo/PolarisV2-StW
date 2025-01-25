#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include "SDK.hpp"

#include <vector>
#include <string>

namespace polaris::cache
{
    class CacheManager
    {
    private:
        static std::vector<SDK::UBlueprintGeneratedClass*> v_pBlueprintGeneratedClassCache;
    public:
        static auto GetFromCache(ECache cache, std::string query);
        static void AddToCache(ECache cache, auto newEntry);
    };

    enum ECache
    {
        Cache_BlueprintGeneratedClass = 0
    };
}

#endif // !CACHE_MANAGER_H
