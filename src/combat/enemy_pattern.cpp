#include "combat/enemy_pattern.h"

namespace ReyesCuadra
{
    // Cantidad de turnos del arranque (antes de que empiece el ciclo que se repite)
    static const int OPENING_TURNS = 2;

    // El ciclo dura 3 turnos: primero el Cortador de enredos (un solo ataque) y despues 2 turnos de combo con el Tajo
    static const int CYCLE_LENGTH = 3;
    static const int TANGLE_CUTTER_TURN_IN_CYCLE = 0;

    // Cantidad de cada skill en los turnos de arranque
    static const int OPENING_TRIPLE_SLASHES = 2;
    static const int OPENING_DOUBLE_SLASHES = 4;

    // Cantidad de cada skill en los turnos de combo
    static const int COMBO_EXPLOSIVE_SLASHES = 1;
    static const int COMBO_TRIPLE_SLASHES = 2;
    static const int COMBO_DOUBLE_SLASHES = 3;

    // Agrega "amount" veces la skill a la lista de acciones
    static void AddRepeated(std::vector<const Skill*>& actions, const Skill* skill, int amount);

    EnemyPattern::EnemyPattern(const Skill& doubleSlashSkill, const Skill& tripleSlashSkill, const Skill& explosiveSlashSkill, const Skill& tangleCutterSkill)
        : doubleSlash(&doubleSlashSkill)
        , tripleSlash(&tripleSlashSkill)
        , explosiveSlash(&explosiveSlashSkill)
        , tangleCutter(&tangleCutterSkill)
    {
    }

    std::vector<const Skill*> EnemyPattern::GetActionsForTurn(int turn) const
    {
        std::vector<const Skill*> actions;

        if (turn <= OPENING_TURNS)
        {
            AddRepeated(actions, tripleSlash, OPENING_TRIPLE_SLASHES);
            AddRepeated(actions, doubleSlash, OPENING_DOUBLE_SLASHES);

            return actions;
        }

        // Posicion dentro del ciclo: 0 es el turno del Cortador de enredos, 1 y 2 son turnos de combo
        const int turnInCycle = (turn - OPENING_TURNS - 1) % CYCLE_LENGTH;

        if (turnInCycle == TANGLE_CUTTER_TURN_IN_CYCLE)
        {
            actions.push_back(tangleCutter);
        }
        else
        {
            AddRepeated(actions, explosiveSlash, COMBO_EXPLOSIVE_SLASHES);
            AddRepeated(actions, tripleSlash, COMBO_TRIPLE_SLASHES);
            AddRepeated(actions, doubleSlash, COMBO_DOUBLE_SLASHES);
        }

        return actions;
    }

    static void AddRepeated(std::vector<const Skill*>& actions, const Skill* skill, int amount)
    {
        for (int index = 0; index < amount; index++)
        {
            actions.push_back(skill);
        }
    }
}
