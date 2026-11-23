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
        bool isLocked;
    };
}

#endif // SKILL_H
