#include "combat/poise.h"

#include "utils/random.h"

namespace ReyesCuadra
{
    // Cada punto de potencia da este porcentaje de chance de critico
    static const float CRITICAL_CHANCE_PER_POTENCY = 0.05f;

    // La chance de critico nunca pasa del 100%
    static const float MAX_CRITICAL_CHANCE = 1.0f;

    float Poise::GetCriticalChance() const
    {
        if (!IsActive())
        {
            return 0.0f;
        }

        const float chance = static_cast<float>(GetPotency()) * CRITICAL_CHANCE_PER_POTENCY;

        return (chance > MAX_CRITICAL_CHANCE) ? MAX_CRITICAL_CHANCE : chance;
    }

    bool Poise::RollCritical()
    {
        if (!IsActive() || !RollChance(GetCriticalChance()))
        {
            return false;
        }

        ConsumeCount();

        return true;
    }

    void Poise::OnTurnEnd()
    {
        ConsumeCount();
    }
}
