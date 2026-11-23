#include "combat/unit.h"

namespace ReyesCuadra
{
    Unit::Unit(const std::string& unitName, int unitMaxHp, sf::Color unitColor)
        : name(unitName)
        , maxHp(unitMaxHp)
        , hp(unitMaxHp)
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

    void Unit::Reset()
    {
        hp = maxHp;
        poise.Clear();
    }
}
