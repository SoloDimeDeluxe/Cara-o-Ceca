#include "combat/burn.h"

namespace ReyesCuadra
{
    int Burn::OnTurnEnd()
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
