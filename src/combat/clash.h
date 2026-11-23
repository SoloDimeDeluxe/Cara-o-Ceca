#ifndef CLASH_H
#define CLASH_H

#include "combat/skill.h"
#include "combat/unit.h"
#include "ui/combat_log.h"

namespace ReyesCuadra
{
    // Resuelve una columna del turno: la skill del jugador contra la del enemigo en ese mismo slot.
    // Si las dos estan, clashean; si hay una sola, ataca sin oposicion. Un nullptr significa que ese slot esta vacio.
    void ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatLog& log);

    // Simula muchos clashes entre las dos skills (sin hacer dano) y devuelve la probabilidad de que gane la del jugador (0 a 1).
    // Sirve para mostrarle al jugador, antes de pelear, que tan buena es la skill que eligio contra la del enemigo.
    float EstimateClashWinChance(const Skill& playerSkill, const Skill& enemySkill);
}

#endif // CLASH_H
