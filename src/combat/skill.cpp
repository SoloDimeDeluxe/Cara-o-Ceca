#include "combat/skill.h"

namespace ReyesCuadra
{
    Skill::Skill(const std::string& skillName, const std::string& skillShortName, SkillType skillType, int skillBasePower, int coinCount, int coinPower, bool hasUnbreakableCoins)
        : name(skillName)
        , shortName(skillShortName)
        , type(skillType)
        , basePower(skillBasePower)
        , isLocked(false)
    {
        // Todas las monedas de una skill tienen el mismo poder (asi son las skills que usamos de base)
        for (int coinIndex = 0; coinIndex < coinCount; coinIndex++)
        {
            coins.push_back(Coin(coinPower, hasUnbreakableCoins));
        }
    }

    const std::string& Skill::GetName() const
    {
        return name;
    }

    const std::string& Skill::GetShortName() const
    {
        return shortName;
    }

    SkillType Skill::GetType() const
    {
        return type;
    }

    int Skill::GetBasePower() const
    {
        return basePower;
    }

    const std::vector<Coin>& Skill::GetCoins() const
    {
        return coins;
    }

    int Skill::GetCoinCount() const
    {
        return static_cast<int>(coins.size());
    }

    bool Skill::IsDefensive() const
    {
        return type == SkillType::Evade || type == SkillType::Counter;
    }

    bool Skill::DealsDamageOnWin() const
    {
        return type == SkillType::Attack || type == SkillType::Counter;
    }

    std::string Skill::GetSummary() const
    {
        std::string summary = "Base " + std::to_string(basePower) + " | " + std::to_string(coins.size());

        summary += (coins.size() == 1) ? " moneda" : " monedas";

        if (!coins.empty())
        {
            summary += " +" + std::to_string(coins.front().GetPower());

            if (coins.front().IsUnbreakable())
            {
                summary += " (irrompibles)";
            }
        }

        return summary;
    }

    bool Skill::IsLocked() const
    {
        return isLocked;
    }

    void Skill::SetLocked(bool locked)
    {
        isLocked = locked;
    }
}
