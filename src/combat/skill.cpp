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

    const PoiseEffects& Skill::GetPoiseEffects() const
    {
        return poiseEffects;
    }

    void Skill::SetPoiseEffects(const PoiseEffects& effects)
    {
        poiseEffects = effects;
    }

    std::string Skill::GetPoiseSummary() const
    {
        std::vector<std::string> parts;

        if (poiseEffects.onUsePotency > 0 || poiseEffects.onUseCount > 0)
        {
            std::string part = "usar";

            if (poiseEffects.onUsePotency > 0)
            {
                part += " +" + std::to_string(poiseEffects.onUsePotency) + " pot.";
            }

            if (poiseEffects.onUseCount > 0)
            {
                part += " +" + std::to_string(poiseEffects.onUseCount) + " cant.";
            }

            parts.push_back(part);
        }

        if (poiseEffects.onHitPotency > 0 || poiseEffects.onHitCount > 0)
        {
            std::string part = "golpe";

            if (poiseEffects.onHitPotency > 0)
            {
                part += " +" + std::to_string(poiseEffects.onHitPotency) + " pot.";
            }

            if (poiseEffects.onHitCount > 0)
            {
                part += " +" + std::to_string(poiseEffects.onHitCount) + " cant.";
            }

            parts.push_back(part);
        }

        if (poiseEffects.onCriticalCount > 0)
        {
            parts.push_back("crítico +" + std::to_string(poiseEffects.onCriticalCount) + " cant.");
        }

        if (parts.empty())
        {
            return "";
        }

        std::string summary = "Poise: " + parts.front();

        for (size_t partIndex = 1; partIndex < parts.size(); partIndex++)
        {
            summary += " · " + parts[partIndex];
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
