#include "combat/sinking.h"

namespace ReyesCuadra
{
    int Sinking::Trigger()
    {
        if (!IsActive())
        {
            return 0;
        }

        const int damage = GetPotency();

        ConsumeCount();

        return damage;
    }
}
