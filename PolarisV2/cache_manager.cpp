#include "cache_manager.h"

namespace polaris::cache
{
    auto CacheManager::GetFromCache(ECache cache, std::string query)
    {
        switch (cache)
        {
        default:
            return nullptr;

        case ECache::Cache_BlueprintGeneratedClass:
            for (auto& entry : v_pBlueprintGeneratedClassCache)
            {
                if (entry != nullptr && entry->GetFullName() == query)
                    return entry;
            }
            return nullptr;
            break;
        }
    }
    void CacheManager::AddToCache(ECache cache, auto newEntry)
    {
        switch (cache)
        {
        case ECache::Cache_BlueprintGeneratedClass:
            v_pBlueprintGeneratedClassCache.push_back(newEntry);
            break;
        }
    }
}