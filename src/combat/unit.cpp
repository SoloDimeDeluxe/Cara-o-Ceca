#include "combat/unit.h"

namespace ReyesCuadra
{
    // Limites de la cordura (SP), igual que en Limbus
    static const int MIN_SANITY = -45;
    static const int MAX_SANITY = 45;
    static const int STARTING_SANITY = 0;

    // Una unidad staggereada se recupera al final del turno siguiente: o sea, despues de 2 finales de turno
    // (el del turno en que quedo staggereada y el del turno que se pierde)
    static const int STAGGER_TURN_ENDS = 2;

    // La chance de cara es 50% + 1% por cada punto de cordura
    static const float BASE_HEADS_CHANCE = 0.5f;
    static const float HEADS_CHANCE_PER_SANITY = 0.01f;

    Unit::Unit(const std::string& unitName, int unitMaxHp, sf::Color unitColor)
        : name(unitName)
        , maxHp(unitMaxHp)
        , hp(unitMaxHp)
        , sanity(STARTING_SANITY)
        , color(unitColor)
        , brokenThresholds(0)
        , staggerTurnEndsLeft(0)
        , hasPendingStaggerEvent(false)
    {
    }

    const std::string& Unit::GetName() const
    {
        return name;
    }

    int Unit::GetHp() const
    {
        return hp;
    }

    int Unit::GetMaxHp() const
    {
        return maxHp;
    }

    float Unit::GetHpRatio() const
    {
        return static_cast<float>(hp) / static_cast<float>(maxHp);
    }

    sf::Color Unit::GetColor() const
    {
        return color;
    }

    void Unit::TakeDamage(int damage)
    {
        hp -= damage;

        if (hp < 0)
        {
            hp = 0;
        }

        // Se rompen todos los umbrales que la vida haya pasado. Aunque caigan varios de una, es un solo Stagger.
        bool brokeThreshold = false;

        while (brokenThresholds < staggerThresholds.size() && GetHpRatio() <= staggerThresholds[brokenThresholds])
        {
            brokenThresholds++;
            brokeThreshold = true;
        }

        if (brokeThreshold && !IsDead() && !IsStaggered())
        {
            staggerTurnEndsLeft = STAGGER_TURN_ENDS;
            hasPendingStaggerEvent = true;
        }
    }

    void Unit::SetStaggerThresholds(const std::vector<float>& thresholds)
    {
        // Se guardan de mayor a menor, que es el orden en que se van rompiendo al perder vida
        staggerThresholds = thresholds;

        for (size_t outer = 0; outer < staggerThresholds.size(); outer++)
        {
            for (size_t inner = outer + 1; inner < staggerThresholds.size(); inner++)
            {
                if (staggerThresholds[inner] > staggerThresholds[outer])
                {
                    const float swapValue = staggerThresholds[outer];
                    staggerThresholds[outer] = staggerThresholds[inner];
                    staggerThresholds[inner] = swapValue;
                }
            }
        }

        brokenThresholds = 0;
    }

    const std::vector<float>& Unit::GetStaggerThresholds() const
    {
        return staggerThresholds;
    }

    bool Unit::IsThresholdBroken(size_t thresholdIndex) const
    {
        return thresholdIndex < brokenThresholds;
    }

    bool Unit::IsStaggered() const
    {
        return staggerTurnEndsLeft > 0;
    }

    bool Unit::ConsumeStaggerEvent()
    {
        const bool hadEvent = hasPendingStaggerEvent;

        hasPendingStaggerEvent = false;

        return hadEvent;
    }

    bool Unit::UpdateStaggerOnTurnEnd()
    {
        if (!IsStaggered())
        {
            return false;
        }

        staggerTurnEndsLeft--;

        return !IsStaggered();
    }

    bool Unit::IsDead() const
    {
        return hp <= 0;
    }

    Poise& Unit::GetPoise()
    {
        return poise;
    }

    const Poise& Unit::GetPoise() const
    {
        return poise;
    }

    int Unit::GetSanity() const
    {
        return sanity;
    }

    bool Unit::IsAtMinSanity() const
    {
        return sanity <= MIN_SANITY;
    }

    void Unit::ChangeSanity(int amount)
    {
        sanity += amount;

        if (sanity < MIN_SANITY)
        {
            sanity = MIN_SANITY;
        }
        else if (sanity > MAX_SANITY)
        {
            sanity = MAX_SANITY;
        }
    }

    float Unit::GetHeadsChance() const
    {
        return BASE_HEADS_CHANCE + static_cast<float>(sanity) * HEADS_CHANCE_PER_SANITY;
    }

    Sinking& Unit::GetSinking()
    {
        return sinking;
    }

    const Sinking& Unit::GetSinking() const
    {
        return sinking;
    }

    void Unit::Reset()
    {
        hp = maxHp;
        sanity = STARTING_SANITY;
        brokenThresholds = 0;
        staggerTurnEndsLeft = 0;
        hasPendingStaggerEvent = false;
        poise.Clear();
        sinking.Clear();
    }
}
