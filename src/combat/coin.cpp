#include "combat/coin.h"

#include "utils/random.h"

namespace ReyesCuadra
{
    Coin::Coin(int coinPower, bool coinIsUnbreakable)
        : power(coinPower)
        , isUnbreakable(coinIsUnbreakable)
    {
    }

    int Coin::GetPower() const
    {
        return power;
    }

    bool Coin::IsUnbreakable() const
    {
        return isUnbreakable;
    }

    bool Coin::Flip(float headsChance) const
    {
        return RollChance(headsChance);
    }
}
