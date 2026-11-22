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
}

#endif // CLASH_H
