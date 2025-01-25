#ifndef PLATE_H
#define PLATE_H

namespace polaris::tables::plates
{
    //!  Base for Plate behavior.
    class Plate
    {
    public:
        virtual void Update(); //!< Update is called every tick while the plate is primary on the stack.

        virtual void OnEnabled(); //!< OnEnabled is called when the plate becomes primary on the stack.
        virtual void OnDisabled(); //!< OnDisabled is called when the plate is not primary on the stack anymore.
    };
}

#endif // PLATE_H