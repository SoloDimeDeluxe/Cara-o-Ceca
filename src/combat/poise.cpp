#include "combat/poise.h"

#include "utils/random.h"

namespace ReyesCuadra
{
    // Cada punto de potencia da este porcentaje de chance de critico
    static const float CRITICAL_CHANCE_PER_POTENCY = 0.05f;

    // La chance de critico nunca pasa del 100%
    static const float MAX_CRITICAL_CHANCE = 1.0f;

    // Tope de potencia y cantidad (igual que en Limbus)
    static const int MAX_POISE_VALUE = 99;

    // Valor con el que arranca la potencia o la cantidad cuando se gana Poise sin tener nada
    static const int MIN_ACTIVE_VALUE = 1;

    // Devuelve el valor recortado entre 0 y el tope de Poise
    static int ClampPoiseValue(int value);

    Poise::Poise()
        : potency(0)
        , count(0)
    {
    }

    int Poise::GetPotency() const
    {
        return potency;
    }

    int Poise::GetCount() const
    {
        return count;
    }

    bool Poise::IsActive() const
    {
        return potency > 0 && count > 0;
    }

    float Poise::GetCriticalChance() const
    {
        if (!IsActive())
        {
            return 0.0f;
        }

        const float chance = static_cast<float>(potency) * CRITICAL_CHANCE_PER_POTENCY;

        return (chance > MAX_CRITICAL_CHANCE) ? MAX_CRITICAL_CHANCE : chance;
    }

    void Poise::AddPotency(int amount)
    {
        potency = ClampPoiseValue(potency + amount);

        if (potency > 0 && count == 0)
        {
            count = MIN_ACTIVE_VALUE;
        }
    }

    void Poise::AddCount(int amount)
    {
        count = ClampPoiseValue(count + amount);

        if (count > 0 && potency == 0)
        {
            potency = MIN_ACTIVE_VALUE;
        }
    }

    bool Poise::RollCritical()
    {
        if (!IsActive() || !RollChance(GetCriticalChance()))
        {
            return false;
        }

        count--;
        ClearIfEmpty();

        return true;
    }

    void Poise::OnTurnEnd()
    {
        if (count > 0)
        {
            count--;
        }

        ClearIfEmpty();
    }

    void Poise::Clear()
    {
        potency = 0;
        count = 0;
    }

    void Poise::ClearIfEmpty()
    {
        if (count <= 0)
        {
            Clear();
        }
    }

    static int ClampPoiseValue(int value)
    {
        if (value < 0)
        {
            return 0;
        }

        return (value > MAX_POISE_VALUE) ? MAX_POISE_VALUE : value;
    }
}
