#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Tope de potencia y cantidad (igual que en Limbus)
    static const int MAX_STATUS_VALUE = 99;

    // Valor con el que arranca la potencia o la cantidad cuando se gana el estado sin tener nada
    static const int MIN_ACTIVE_VALUE = 1;

    // Devuelve el valor recortado entre 0 y el tope
    static int ClampStatusValue(int value);

    StatusEffect::StatusEffect()
        : potency(0)
        , count(0)
    {
    }

    int StatusEffect::GetPotency() const
    {
        return potency;
    }

    int StatusEffect::GetCount() const
    {
        return count;
    }

    bool StatusEffect::IsActive() const
    {
        return potency > 0 && count > 0;
    }

    void StatusEffect::AddPotency(int amount)
    {
        potency = ClampStatusValue(potency + amount);

        if (potency > 0 && count == 0)
        {
            count = MIN_ACTIVE_VALUE;
        }
    }

    void StatusEffect::AddCount(int amount)
    {
        count = ClampStatusValue(count + amount);

        if (count > 0 && potency == 0)
        {
            potency = MIN_ACTIVE_VALUE;
        }
    }

    void StatusEffect::Clear()
    {
        potency = 0;
        count = 0;
    }

    void StatusEffect::ConsumeCount()
    {
        if (count > 0)
        {
            count--;
        }

        if (count <= 0)
        {
            Clear();
        }
    }

    static int ClampStatusValue(int value)
    {
        if (value < 0)
        {
            return 0;
        }

        return (value > MAX_STATUS_VALUE) ? MAX_STATUS_VALUE : value;
    }
}
