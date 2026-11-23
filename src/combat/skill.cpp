#include "combat/skill.h"

namespace ReyesCuadra
{
    // Arma el texto de cuanto estado se gana o inflige, por ejemplo "3 de potencia y +2 de cantidad"
    static std::string DescribeStatusAmount(int potency, int count);

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

    const SinkingEffects& Skill::GetSinkingEffects() const
    {
        return sinkingEffects;
    }

    void Skill::SetSinkingEffects(const SinkingEffects& effects)
    {
        sinkingEffects = effects;
    }

    const TargetStatusEffects& Skill::GetTargetStatusEffects() const
    {
        return targetStatusEffects;
    }

    void Skill::SetTargetStatusEffects(const TargetStatusEffects& effects)
    {
        targetStatusEffects = effects;
    }

    std::vector<std::string> Skill::GetEffectLines() const
    {
        std::vector<std::string> lines;

        // Poise (sobre quien usa la skill)
        if (poiseEffects.onUsePotency > 0 || poiseEffects.onUseCount > 0)
        {
            lines.push_back("[Al usarla] Gana " + DescribeStatusAmount(poiseEffects.onUsePotency, poiseEffects.onUseCount) + " de Poise");
        }

        if (poiseEffects.onHitPotency > 0 || poiseEffects.onHitCount > 0)
        {
            lines.push_back("[Al golpear] Gana " + DescribeStatusAmount(poiseEffects.onHitPotency, poiseEffects.onHitCount) + " de Poise");
        }

        if (poiseEffects.onCriticalCount > 0)
        {
            lines.push_back("[Al hacer crítico] Gana " + DescribeStatusAmount(0, poiseEffects.onCriticalCount) + " de Poise");
        }

        // Sinking (sobre el objetivo)
        if (sinkingEffects.onHitPotency > 0 || sinkingEffects.onHitCount > 0)
        {
            lines.push_back("[Al golpear] Inflige " + DescribeStatusAmount(sinkingEffects.onHitPotency, sinkingEffects.onHitCount) + " de Sinking");
        }

        if (sinkingEffects.onHitWithoutCrackingPotency > 0 || sinkingEffects.onHitWithoutCrackingCount > 0)
        {
            lines.push_back("[Al golpear sin Cracking] Inflige " + DescribeStatusAmount(sinkingEffects.onHitWithoutCrackingPotency, sinkingEffects.onHitWithoutCrackingCount) + " de Sinking");
        }

        if (poiseEffects.gainsCoinPowerFromPoise)
        {
            lines.push_back("Poder de moneda +1 cada 6 de (Poise propio + Sinking del objetivo), máximo +2");
        }

        // Tremor y Burn (sobre el objetivo), moneda por moneda
        const TargetStatusEffects& statusEffects = targetStatusEffects;

        if (statusEffects.statusPerFinalPower > 0)
        {
            lines.push_back("Poder final +1 cada " + std::to_string(statusEffects.statusPerFinalPower) + " de (Burn + Tremor) del objetivo, máximo +" + std::to_string(statusEffects.maxFinalPowerBonus));
        }

        if (statusEffects.statusPerCoinPower > 0)
        {
            lines.push_back("Poder de moneda +1 cada " + std::to_string(statusEffects.statusPerCoinPower) + " de (Burn + Tremor) del objetivo, máximo +" + std::to_string(statusEffects.maxCoinPowerBonus));
        }

        for (size_t coinIndex = 0; coinIndex < statusEffects.perCoin.size(); coinIndex++)
        {
            const CoinStatusEffect& coinEffect = statusEffects.perCoin[coinIndex];
            const std::string coinPrefix = "Moneda " + std::to_string(coinIndex + 1) + ": ";

            if (coinEffect.burnPotency > 0 || coinEffect.burnCount > 0)
            {
                lines.push_back(coinPrefix + "[Al golpear] Inflige " + DescribeStatusAmount(coinEffect.burnPotency, coinEffect.burnCount) + " de Burn");
            }

            if (coinEffect.tremorPotency > 0 || coinEffect.tremorCount > 0)
            {
                lines.push_back(coinPrefix + "[Al golpear] Inflige " + DescribeStatusAmount(coinEffect.tremorPotency, coinEffect.tremorCount) + " de Tremor");
            }

            if (coinEffect.tremorWithoutCrackingPotency > 0 || coinEffect.tremorWithoutCrackingCount > 0)
            {
                lines.push_back(coinPrefix + "[Al golpear sin Cracking] Inflige " + DescribeStatusAmount(coinEffect.tremorWithoutCrackingPotency, coinEffect.tremorWithoutCrackingCount) + " de Tremor");
            }

            if (coinEffect.burstTimes > 0)
            {
                const std::string condition = coinEffect.burstOnlyWithoutCracking ? "[Al golpear sin Cracking]" : "[Al golpear]";
                std::string line = coinPrefix + condition + " Tremor Burst";

                if (coinEffect.burstTimes > 1)
                {
                    line += " " + std::to_string(coinEffect.burstTimes) + " veces";
                }

                if (coinEffect.countLossAfterBurst > 0)
                {
                    line += "; después le saca " + std::to_string(coinEffect.countLossAfterBurst) + " de cantidad de Tremor";
                }

                lines.push_back(line);
            }
        }

        return lines;
    }

    bool Skill::IsLocked() const
    {
        return isLocked;
    }

    void Skill::SetLocked(bool locked)
    {
        isLocked = locked;
    }

    static std::string DescribeStatusAmount(int potency, int count)
    {
        std::string text;

        if (potency > 0)
        {
            text = std::to_string(potency) + " de potencia";
        }

        if (count > 0)
        {
            if (!text.empty())
            {
                text += " y ";
            }

            text += "+" + std::to_string(count) + " de cantidad";
        }

        return text;
    }
}
