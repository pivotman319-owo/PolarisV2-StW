#ifndef CREATIVE_PLATE_H
#define CREATIVE_PLATE_H

#include "athena_plate.h"

#include <string>
#include <filesystem>

namespace polaris::tables::plates
{
    class CreativePlate : public AthenaPlate
    {
    public:
        void Initialize() override;

        void DestroyAllBuilds();

        void LoadCreation(std::filesystem::path path);
        void SaveCreation(std::string name);
    };
}

#endif // !CREATIVE_PLATE_H
