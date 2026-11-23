#ifndef ENEMY_PATTERN_H
#define ENEMY_PATTERN_H

#include <vector>

#include "combat/skill.h"

namespace ReyesCuadra
{
    // Patron fijo de Lei Heng: decide que skills usa en cada turno
    //   Turnos 1 y 2: 2 Triple Corte - Explosion + 4 Doble Corte - Explosion
    //   Despues se repite: 2 turnos de (1 Tajo Destrozador Explosivo + 2 Triple Corte + 3 Doble Corte) y 1 turno de Cortador de enredos
    //   Con 40% de vida o menos, el Cortador de enredos se cambia por la Rafaga Savage Tigerslayer
    class EnemyPattern
    {
    public:
        EnemyPattern(const Skill& doubleSlashSkill, const Skill& tripleSlashSkill, const Skill& explosiveSlashSkill, const Skill& tangleCutterSkill, const Skill& tigerslayerFlurrySkill);

        // Devuelve las skills que usa el enemigo en el turno indicado (los turnos empiezan en 1), una por slot
        std::vector<const Skill*> GetActionsForTurn(int turn, float hpRatio) const;

    private:
        const Skill* doubleSlash;
        const Skill* tripleSlash;
        const Skill* explosiveSlash;
        const Skill* tangleCutter;
        const Skill* tigerslayerFlurry;
    };
}

#endif // ENEMY_PATTERN_H
