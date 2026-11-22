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

        // Una skill bloqueada se ve en la lista pero no se puede elegir (por ejemplo Furioso-Replica hasta desbloquearla)
        bool IsLocked() const;
        void SetLocked(bool locked);

    private:
        std::string name;
        std::string shortName;
        SkillType type;
        int basePower;
        std::vector<Coin> coins;
        bool isLocked;
    };
}

#endif // SKILL_H
