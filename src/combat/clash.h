#ifndef CLASH_H
#define CLASH_H

#include "combat/skill.h"
#include "combat/unit.h"
#include "ui/combat_log.h"

namespace ReyesCuadra
{
    // Resuelve una columna del turno: la skill del jugador contra la del enemigo en ese mismo slot.
    // Si las dos estan, clashean (salvo que una sea esquiva: ahi el otro ataca igual y se ve que golpes se esquivan).
    // Si hay una sola, ataca sin oposicion. Un nullptr significa que ese slot esta vacio.
    // Antes de pelear se aplican los efectos [Al usarla] de cada skill (por ejemplo, ganar Poise).
    void ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatLog& log);

    // Simula muchos clashes entre las dos skills (sin hacer dano ni cambiar el Poise) y devuelve la probabilidad
    // de que gane la del jugador (0 a 1). Tiene en cuenta el Poise actual de cada unidad.
    // Si el jugador esquiva, la probabilidad es la de esquivar todos los golpes.
    float EstimateClashWinChance(const Unit& player, const Skill& playerSkill, const Unit& enemy, const Skill& enemySkill);
}

#endif // CLASH_H
