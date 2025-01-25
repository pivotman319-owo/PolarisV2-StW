#ifndef BASE_PLATE_H
#define BASE_PLATE_H

#include "peh_plate.h"

namespace polaris::tables::plates
{
    //!  The base PehPlate.
    /*!
     * This Plate is always at the bottom of the Main PehTable.
     * In charge of initializing Polaris before going into Frontend mode.
     */
    class BasePlate : public PehPlate
    {
    public:
        void OnEnabled() override;
    };
}

#endif // !BASE_PLATE_H