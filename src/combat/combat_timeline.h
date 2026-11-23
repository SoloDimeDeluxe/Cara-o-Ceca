#ifndef COMBAT_TIMELINE_H
#define COMBAT_TIMELINE_H

#include <string>
#include <vector>

namespace ReyesCuadra
{
    class Unit;

    // Que tipo de cosa paso en el combate (para mostrarla en pantalla de a poco)
    enum class CombatEventType
    {
        ColumnStart,    // empieza a resolverse una columna
        Hit,            // un golpe que hace dano a la vida
        Banner,         // cartel grande en el medio (Stagger, Tremor Burst)
        LogLine         // linea de texto para el registro de combate
    };

    // Un paso de la pelea. La logica del combate ya lo resolvio todo de golpe;
    // esto guarda lo que paso, en orden, para que la pantalla lo vaya mostrando con pausas.
    struct CombatEvent
    {
        CombatEventType type = CombatEventType::LogLine;

        // Columna en la que pasa (-1 = fuera de las columnas, por ejemplo la quemada del final del turno)
        int column = -1;

        // Para los golpes: a quien le pegaron, cuanto y si fue critico
        bool targetIsPlayer = false;
        int damage = 0;
        bool isCritical = false;

        // Texto del cartel o de la linea del registro
        std::string text;

        // Vida de cada uno justo despues de este paso (la barra de vida se va actualizando con esto)
        int playerHpAfter = 0;
        int enemyHpAfter = 0;
    };

    // Lista ordenada de todo lo que paso en un turno
    class CombatTimeline
    {
    public:
        // Necesita saber quien es el jugador y quien el enemigo para anotar la vida de cada uno en cada paso
        CombatTimeline(const Unit& player, const Unit& enemy);

        // La columna que se esta resolviendo ahora (los golpes y carteles que se anoten van en esa columna)
        void StartColumn(int column);
        void EndColumn();

        void AddHit(const Unit& target, int damage, bool isCritical);
        void AddBanner(const std::string& text);
        void AddLogLine(const std::string& text);

        const std::vector<CombatEvent>& GetEvents() const;
        void Clear();

    private:
        // Arma un evento con la columna actual y la vida de los dos en este momento
        CombatEvent MakeEvent(CombatEventType type) const;

        const Unit* player;
        const Unit* enemy;
        int currentColumn;
        std::vector<CombatEvent> events;
    };
}

#endif // COMBAT_TIMELINE_H
