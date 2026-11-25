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
        Miss,           // un golpe que el objetivo esquivo (no hace dano)
        Heal,           // una unidad recupera vida (damage guarda cuanta)
        ClashRound,     // una ronda del clash: como cayo cada moneda y cuanto poder saco cada uno
        EvadeRoll,      // la tirada de la esquiva de Yi Sang (el numero que hay que superar para pegarle)
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

        // Para las rondas de clash: poder de cada uno y como cayo cada moneda tirada (true = cara).
        // Las monedas que no estan en la lista ya se rompieron en rondas anteriores.
        // En la tirada de esquiva solo se usan playerPower y playerCoinFaces.
        int playerPower = 0;
        int enemyPower = 0;
        std::vector<bool> playerCoinFaces;
        std::vector<bool> enemyCoinFaces;
        int playerTotalCoins = 0;
        int enemyTotalCoins = 0;

        // Texto del cartel o de la linea del registro
        std::string text;

        // Para los carteles de Stagger y Tremor Burst: true si le pasa a alguien (y a quien, con targetIsPlayer)
        bool bannerHasTarget = false;

        // Vida de cada uno justo despues de este paso (la barra de vida se va actualizando con esto)
        int playerHpAfter = 0;
        int enemyHpAfter = 0;

        // Cordura (SP) de cada uno justo despues de este paso (el SP de arriba se va actualizando con esto)
        int playerSanityAfter = 0;
        int enemySanityAfter = 0;

        // Solo en el comienzo de una columna: chance de Yi Sang de ganar ese clash con la cordura y los estados
        // que tienen los dos en ese momento del turno (-1 = no hay clash)
        float columnWinChance = -1.0f;
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

        // Anota en el comienzo de la columna actual la chance de ganar su clash (calculada en ese momento del turno)
        void SetColumnWinChance(float winChance);

        void AddHit(const Unit& target, int damage, bool isCritical);

        // Anota un golpe esquivado (para mostrar "Miss" sobre el que esquivo)
        void AddMiss(const Unit& target);

        // Anota que una unidad recupero vida (para mostrar el "+N" verde)
        void AddHeal(const Unit& target, int amount);

        // Anota una ronda del clash con el poder de cada lado y las caras/cruces de sus monedas
        void AddClashRound(int playerPower, const std::vector<bool>& playerCoinFaces, int playerTotalCoins, int enemyPower, const std::vector<bool>& enemyCoinFaces, int enemyTotalCoins);

        // Anota la tirada de la esquiva (el que esquiva es siempre Yi Sang)
        void AddEvadeRoll(int evadePower, const std::vector<bool>& coinFaces, int totalCoins);
        void AddBanner(const std::string& text);

        // Cartel que le pasa a una unidad (Stagger, Tremor Burst): la arena lo muestra en esa figura
        void AddUnitBanner(const Unit& target, const std::string& text);
        void AddLogLine(const std::string& text);

        // Desde aca, las lineas del registro se guardan aparte (los golpes y carteles siguen en su lugar).
        // Sirve para escribir primero el resumen de un ataque y despues lo que paso durante el (Stagger, etc.)
        void HoldLogLines();

        // Anota la linea de resumen y despues todas las que se guardaron, y vuelve a anotar normal
        void AddSummaryAndReleaseHeld(const std::string& summaryLine);

        const std::vector<CombatEvent>& GetEvents() const;
        void Clear();

    private:
        // Arma un evento con la columna actual y la vida de los dos en este momento
        CombatEvent MakeEvent(CombatEventType type) const;

        const Unit* player;
        const Unit* enemy;
        int currentColumn;
        std::vector<CombatEvent> events;

        // Lineas del registro guardadas mientras isHoldingLogLines es true
        std::vector<CombatEvent> heldLogLines;
        bool isHoldingLogLines;
    };
}

#endif // COMBAT_TIMELINE_H
