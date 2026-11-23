#ifndef SKILL_H
#define SKILL_H

#include <string>
#include <vector>

#include "combat/coin.h"

namespace ReyesCuadra
{
    // Que hace la skill: ataca, esquiva (no pega aunque gane) o contraataca (pega si gana el clash)
    enum class SkillType
    {
        Attack,
        Evade,
        Counter
    };

    // Cuanto Poise genera una skill y en que momento (todo arranca en 0 = no genera nada)
    struct PoiseEffects
    {
        // [Al usarla]: apenas se usa la skill, antes del clash
        int onUsePotency = 0;
        int onUseCount = 0;

        // [Al golpear]: por cada moneda que pega
        int onHitPotency = 0;
        int onHitCount = 0;

        // [Al hacer critico]: por cada golpe critico
        int onCriticalCount = 0;

        // Si es true, la skill gana +1 de poder por moneda cada 6 de (Poise propio + Sinking del objetivo), maximo +2
        bool gainsCoinPowerFromPoise = false;
    };

    // Cuanto Sinking le pone una skill al objetivo (todo arranca en 0 = no pone nada)
    struct SinkingEffects
    {
        // [Al golpear]: por cada moneda que pega
        int onHitPotency = 0;
        int onHitCount = 0;

        // [Al golpear sin Cracking]: igual que el anterior, pero solo si el golpe no hace "Cracking".
        // Como el juego todavia no tiene Cracking, por ahora se aplica en todos los golpes.
        int onHitWithoutCrackingPotency = 0;
        int onHitWithoutCrackingCount = 0;
    };

    // Tremor que pone una moneda al golpear (las skills de Lei Heng cambian moneda por moneda)
    struct CoinTremorEffect
    {
        // [Al golpear]: Tremor que le pone al objetivo
        int potency = 0;
        int count = 0;

        // Extra que solo se pone si el golpe viene de un clash
        int clashOnlyPotency = 0;
        int clashOnlyCount = 0;

        // [Al golpear]: cuantas veces le hace Tremor Burst al objetivo (0 = ninguna)
        int burstTimes = 0;

        // Condiciones para el Burst: que el golpe venga de un clash y/o que el golpe no haga "Cracking"
        bool burstOnlyInClash = false;
        bool burstOnlyWithoutCracking = false;

        // Cantidad de Tremor que le saca al objetivo despues de los Burst
        int countLossAfterBurst = 0;
    };

    // Efectos de Tremor de una skill
    struct TremorEffects
    {
        // Un efecto por moneda, en el mismo orden que las monedas (puede tener menos elementos que monedas)
        std::vector<CoinTremorEffect> perCoin;

        // Poder base +1 cada "tremorPerBasePower" de Tremor del objetivo, hasta maxBasePowerBonus (0 = no tiene este bonus)
        int tremorPerBasePower = 0;
        int maxBasePowerBonus = 0;

        // Poder de moneda +1 cada "tremorPerCoinPower" de Tremor del objetivo, hasta maxCoinPowerBonus (0 = no tiene este bonus)
        int tremorPerCoinPower = 0;
        int maxCoinPowerBonus = 0;
    };

    // Skill que usa una unidad en un slot: poder base mas una lista de monedas
    class Skill
    {
    public:
        Skill(const std::string& skillName, const std::string& skillShortName, SkillType skillType, int skillBasePower, int coinCount, int coinPower, bool hasUnbreakableCoins);

        // Nombre completo de la skill (el que se ve en los botones y en el registro)
        const std::string& GetName() const;

        // Nombre corto para las tarjetas de los slots, donde no entra el nombre completo
        const std::string& GetShortName() const;

        SkillType GetType() const;
        int GetBasePower() const;
        const std::vector<Coin>& GetCoins() const;
        int GetCoinCount() const;

        // Devuelve true si la skill es de defensa (esquivar o contraatacar)
        bool IsDefensive() const;

        // Devuelve true si la skill pega cuando gana un clash (ataque o contraataque)
        bool DealsDamageOnWin() const;

        // Resumen de los numeros para mostrar en pantalla, por ejemplo "Base 4 | 3 monedas +3"
        std::string GetSummary() const;

        // Efectos de Poise de la skill
        const PoiseEffects& GetPoiseEffects() const;
        void SetPoiseEffects(const PoiseEffects& effects);

        // Efectos de Sinking de la skill
        const SinkingEffects& GetSinkingEffects() const;
        void SetSinkingEffects(const SinkingEffects& effects);

        // Efectos de Tremor de la skill
        const TremorEffects& GetTremorEffects() const;
        void SetTremorEffects(const TremorEffects& effects);

        // Lineas de texto que explican los efectos de la skill (Poise, Sinking, bonus de poder), para el cartel de ayuda
        std::vector<std::string> GetEffectLines() const;

        // Una skill bloqueada se ve en la lista pero no se puede elegir (por ejemplo Furioso-Replica hasta desbloquearla)
        bool IsLocked() const;
        void SetLocked(bool locked);

    private:
        std::string name;
        std::string shortName;
        SkillType type;
        int basePower;
        std::vector<Coin> coins;
        PoiseEffects poiseEffects;
        SinkingEffects sinkingEffects;
        TremorEffects tremorEffects;
        bool isLocked;
    };
}

#endif // SKILL_H
