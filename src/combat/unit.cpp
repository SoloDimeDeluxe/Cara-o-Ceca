#include "combat/unit.h"

namespace ReyesCuadra
{
    // Limites de la cordura (SP), igual que en Limbus
    static const int MIN_SANITY = -45;
    static const int MAX_SANITY = 45;
    static const int STARTING_SANITY = 0;

    // La chance de cara es 50% + 1% por cada punto de cordura
    static const float BASE_HEADS_CHANCE = 0.5f;
    static const float HEADS_CHANCE_PER_SANITY = 0.01f;

    Unit::Unit(const std::string& unitName, int unitMaxHp, sf::Color unitColor)
        : name(unitName)
        , maxHp(unitMaxHp)
        , hp(unitMaxHp)
        , sanity(STARTING_SANITY)
        , color(unitColor)
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
        poise.Clear();
        sinking.Clear();
    }
}
