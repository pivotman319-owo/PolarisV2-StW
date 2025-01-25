#ifndef INVENTORY_OFFSET_FIXES_H
#define INVENTORY_OFFSET_FIXES_H

#include "SDK.hpp"

namespace polaris::models::offsetfixes
{
    struct OutpostInventory
    {
        unsigned char UnknownData00[0x1D30];
        class SDK::AFortInventory* OutpostInventory;                                         // 0x1D30
    };
    struct WorldInventory
    {
        unsigned char UnknownData00[0x1D28];
        class SDK::AFortInventory* WorldInventory;
    };
    struct ParentInventory
    {
        unsigned char UnknownData00[0x0098];
        class SDK::AFortInventory               ParentInventory;
    };
    struct OwnerInventory
    {
        unsigned char UnknownData00[0x01D8];
        class SDK::AFortInventory* OwnerInventory;
    };
}

#endif // !INVENTORY_OFFSET_FIXES