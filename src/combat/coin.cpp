#include "combat/coin.h"

#include "utils/random.h"

namespace ReyesCuadra
{
    // Probabilidad de que una moneda salga cara (mas adelante la puede modificar la cordura)
    static const float HEADS_CHANCE = 0.5f;

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

    bool Coin::Flip() const
    {
        return RollChance(HEADS_CHANCE);
    }
}
