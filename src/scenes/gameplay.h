#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <array>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "combat/combat_timeline.h"
#include "combat/enemy_pattern.h"
#include "combat/skill.h"
#include "combat/unit.h"
#include "ui/button.h"
#include "ui/combat_log.h"
#include "ui/arena_view.h"
#include "ui/help_panel.h"

namespace ReyesCuadra
{
    // Cantidad de columnas por turno: Lei Heng usa hasta 6 acciones, una por columna
    const int SLOT_COUNT = 6;

    // Cantidad de acciones de Yi Sang por turno: las pone en las columnas que quiera frenar
    const int PLAYER_MAX_ACTIONS = 3;

    // Como en Limbus, cada accion de Yi Sang trae 2 skills sorteadas para elegir (pueden salir repetidas)
    const int OPTIONS_PER_ACTION = 2;

    // Botones de cada accion: las 2 opciones sorteadas y, al final, Esquivar (siempre disponible)
    const int BUTTONS_PER_ACTION = OPTIONS_PER_ACTION + 1;

    // En que parte de la partida estamos
    enum class GameState
    {
        ChoosingSkills,
        PlayingTurn,    // se esta mostrando la pelea del turno, paso a paso
        Victory,
        Defeat
    };

    // Numero de dano que sale flotando sobre una tarjeta cuando alguien recibe un golpe
    struct FloatingText
    {
        std::string text;
        sf::Vector2f position;
        sf::Color color;
        float timeLeft;
    };

    // Numeros de la partida que se muestran en el cartel de fin
    struct GameStats
    {
        int turnsPlayed = 0;
        int damageDealt = 0;
        int damageTaken = 0;
        int clashesWon = 0;
        int clashesLost = 0;
        int hitsEvaded = 0;
        int prescriptsFulfilled = 0;
        int furiosoUses = 0;
    };

    // Chance ya calculada de una skill de Yi Sang contra una de Lei Heng (para no simular lo mismo dos veces en el turno)
    struct WinChanceCacheEntry
    {
        const Skill* playerSkill;
        const Skill* enemySkill;
        float winChance;
    };

    // Escena del combate: Yi Sang contra Lei Heng, por turnos
    class Gameplay
    {
    public:
        explicit Gameplay(const sf::Font& gameFont);

        // Como guarda punteros a sus propias skills, no se puede copiar
        Gameplay(const Gameplay&) = delete;
        Gameplay& operator=(const Gameplay&) = delete;

        // Recibe los eventos de la ventana (movimiento y clics del mouse, Esc para pausar y Espacio / Enter para combatir)
        void HandleEvent(const sf::Event& event);

        // Deja todo como al principio: vida llena, turno 1 y registro vacio (se llama al apretar Jugar en el menu).
        // En modo Normal Yi Sang arranca con +45 de SP, se cura 15 HP por clash ganado y recibe la mitad de Burn; el Dificil no tiene esas ayudas.
        void StartNewGame(bool hardMode);

        // Devuelve true una sola vez si el jugador eligio volver al menu principal (desde la pausa o el fin de partida)
        bool ConsumeReturnToMenuRequest();

        // Avanza las animaciones (la pelea paso a paso, los numeros flotantes, las barras de vida)
        void Update(float deltaSeconds);

        // Dibuja todo el combate
        void Draw(sf::RenderWindow& window) const;

    private:
        // Esc: cierra la ayuda si esta abierta; si no, pausa o despausa (con la partida terminada no hace nada)
        void HandleEscape();

        // Espacio o Enter: hace lo mismo que el boton de Combatir (o Saltear mientras se ve la pelea)
        void HandleFightKey();

        // Suma a las estadisticas lo que paso en el turno que se acaba de resolver (dano, clashes, esquivas)
        void CountTurnStats();

        // Al soltar el clic izquierdo: si se venia arrastrando una skill, se pone en la columna donde se solto;
        // si casi no se movio el mouse, funciona como un clic comun (va a la columna seleccionada)
        void HandleLeftRelease(sf::Vector2f releasePosition);

        // Columna (de las 6) que esta debajo de esa posicion, contando las dos filas de tarjetas; -1 si ninguna
        int GetColumnAt(sf::Vector2f position) const;

        // true si se esta arrastrando una skill (se apreto sobre una y el mouse ya se movio un poco)
        bool IsDraggingSkill() const;

        // Dibuja la columna donde caeria la skill y una tarjeta que sigue al mouse con su nombre
        void DrawDraggedSkill(sf::RenderWindow& window) const;

        // Clic con la pausa abierta: Seguir la cierra, Volver al menu pide ir al menu
        void HandlePauseClick(sf::Vector2f clickPosition);

        // Pide al patron las skills de Lei Heng para el turno actual y pone cada accion de Yi Sang en su columna
        // (la accion 1 en la columna 1, etc.) con la primera de sus dos opciones
        void PrepareTurn();

        // Sortea una skill de ataque para la mano de Yi Sang: S1 60%, S2 30%, S3 10%
        int DrawSkillOption() const;

        // Sortea de nuevo las dos opciones de todas las acciones (al empezar la partida)
        void DealNewHand();

        // Al terminar el turno se gastan las skills usadas y en su lugar salen otras nuevas.
        // La que no se uso se queda para el turno siguiente. Esquivar gasta la primera opcion de esa accion.
        void SpendUsedOptions();

        // Skill que eligio una accion de Yi Sang (una de sus opciones o Esquivar)
        const Skill* GetActionSkill(int actionIndex) const;

        // Pone en playerActions (columna por columna) las skills de las acciones que estan ubicadas
        void RebuildPlayerActions();

        // Pone la accion de Yi Sang en la columna seleccionada con la opcion elegida (o Esquivar)
        void AssignAction(int actionIndex, int choiceIndex);

        // Prescripto: marca hasta 2 skills al azar de la mano (nunca Esquivar ni Furioso-Replica)
        void RollPrescriptMarks();

        // true si la opcion de esa accion tiene la Marca del Prescripto y ademas es la elegida
        bool IsActionUsingMarkedOption(int actionIndex) const;

        // Con 9 cargas de Prescripto, pone una copia de Furioso-Replica en la mano (si todavia no esta)
        void GiveFuriosoIfReady();

        // true si alguna opcion de la mano es Furioso-Replica
        bool HandHasFurioso() const;

        // Cambia los textos de los botones de la mano (nombre de cada opcion sorteada) y marca la elegida
        void RefreshHandButtons();

        // Recalcula el porcentaje de victoria de cada columna (se llama cada vez que cambia alguna skill)
        void UpdateWinChances();

        // Devuelve la chance de ganar el clash. La primera vez la simula y despues la saca de la lista guardada,
        // asi los clics no se traban (mientras se eligen skills, nadie cambia de estado y la chance es la misma)
        float GetWinChance(const Skill& playerSkill, const Skill& enemySkill);

        // Pone al lado de cada boton de skill la chance que tendria esa skill contra la de Lei Heng del slot seleccionado,
        // asi el jugador compara antes de elegir (como en Limbus)
        void UpdateSkillButtonChances();

        // Resuelve todos los clashes del turno de una (la logica) y arranca a mostrarlos de a poco
        void ResolveTurn();

        // Cantidad de acciones que Yi Sang tiene puestas este turno
        int CountPlayerActions() const;

        // Muestra el siguiente paso de la pelea (un golpe, un cartel, una linea del registro...)
        void PlayEvent(const CombatEvent& event);

        // Termina de mostrar la pelea: si se apreto Saltear, muestra todo lo que faltaba de una
        void FinishPlayback();

        // Tarjeta (y fila) donde aparece un golpe: la del que lo recibe en esa columna
        sf::Vector2f GetHitPosition(const CombatEvent& event) const;

        // Al final del turno, la unidad recibe el dano de su Burn (y se avisa en el registro)
        void ApplyBurnAtTurnEnd(Unit& unit);

        // Reacciona a un clic izquierdo segun lo que haya debajo del mouse
        void HandleClick(sf::Vector2f clickPosition);

        // Clic derecho: saca la accion de Yi Sang de esa columna
        void HandleRightClick(sf::Vector2f clickPosition);

        // Rectangulo de la tarjeta de un slot (de Yi Sang o de Lei Heng)
        sf::FloatRect GetSlotBounds(int slotIndex, bool isPlayerRow) const;

        void DrawUnitPanels(sf::RenderWindow& window) const;
        void DrawSlots(sf::RenderWindow& window) const;
        // Dibuja la tarjeta de un slot. Si no hay skill, muestra emptyText (por ejemplo "Sin acción" o "Staggereado").
        void DrawSlotCard(sf::RenderWindow& window, sf::FloatRect bounds, const Skill* skill, sf::Color fillColor, bool isHighlighted, const std::string& emptyText) const;

        // Monedas de la skill abajo de la tarjeta (las irrompibles con borde rojo), para saber de entrada cuantas tiene
        void DrawCardCoins(sf::RenderWindow& window, sf::FloatRect bounds, const Skill& skill) const;

        // Dibuja entre las dos filas que tan probable es ganar el clash de esa columna (o si no hay clash)
        void DrawClashPreview(sf::RenderWindow& window, int slotIndex, sf::FloatRect area) const;

        void DrawSkillButtons(sf::RenderWindow& window) const;
        void DrawGameOver(sf::RenderWindow& window) const;

        // Cartel de pausa con los botones Seguir y Volver al menu
        void DrawPause(sf::RenderWindow& window) const;

        // Detalle de la skill que esta debajo del mouse (de Yi Sang o de Lei Heng), encima del registro de combate
        void DrawSkillTooltip(sf::RenderWindow& window) const;

        // Dibuja el cartel con el titulo, los numeros y los efectos de una skill
        void DrawSkillDetails(sf::RenderWindow& window, const Skill& skill, const std::string& title) const;

        // Animaciones de la pelea: columna activa, numeros flotantes y carteles grandes
        void DrawActiveColumn(sf::RenderWindow& window) const;
        void DrawFloatingTexts(sf::RenderWindow& window) const;
        void DrawBanner(sf::RenderWindow& window) const;

        // Cuando las monedas terminan de girar, muestra "-1 moneda" sobre la tarjeta del que perdio la ronda
        void ShowCoinBreakIfRevealed();

        // Texto que sale flotando y se desvanece (dano, "-1 moneda", "¡Clash!")
        void SpawnFloatingText(const std::string& text, sf::Vector2f position, sf::Color color);

        // Monedas del clash que se esta mostrando, abajo de cada tarjeta de la columna activa
        void DrawClashCoins(sf::RenderWindow& window) const;

        // Fila de monedas de un lado: giran un ratito y despues muestran cara (dorada) o cruz (gris).
        // Las que ya se rompieron se ven vacias; si breaksLastCoin es true, la ultima tirada se marca en rojo (se rompe).
        void DrawCoinRow(sf::RenderWindow& window, sf::FloatRect cardBounds, const std::vector<bool>& coinFaces, int totalCoins, bool breaksLastCoin) const;

        // Poder de cada uno en la ronda del clash (o el numero de la esquiva), entre las dos filas
        void DrawClashNumbers(sf::RenderWindow& window, sf::FloatRect area) const;

        const sf::Font& font;

        Unit player;
        Unit enemy;

        // Skills de Yi Sang, en el mismo orden que los botones
        std::vector<Skill> playerSkills;

        // Skills de Lei Heng
        Skill doubleSlash;
        Skill tripleSlash;
        Skill explosiveSlash;
        Skill tangleCutter;
        Skill tiantuiReload;

        EnemyPattern enemyPattern;

        // ---------- Mano de Yi Sang ----------

        // Indice (dentro de playerSkills) de las 2 skills sorteadas de cada accion
        std::array<std::array<int, OPTIONS_PER_ACTION>, PLAYER_MAX_ACTIONS> actionOptions;

        // Que eligio cada accion: 0 o 1 = una de sus opciones, OPTIONS_PER_ACTION = Esquivar
        std::array<int, PLAYER_MAX_ACTIONS> actionChoices;

        // Columna donde esta puesta cada accion (-1 = no se usa este turno)
        std::array<int, PLAYER_MAX_ACTIONS> actionColumns;

        // Que opciones de la mano tienen la Marca del Prescripto este turno
        std::array<std::array<bool, OPTIONS_PER_ACTION>, PLAYER_MAX_ACTIONS> actionOptionMarked;

        // Boton de la mano que se apreto para arrastrar (-1 = ninguno) y donde estaba el mouse al apretarlo
        int draggedButton;
        sf::Vector2f dragStartPosition;

        // Columnas donde Yi Sang usa una skill marcada (se arma junto con playerActions)
        std::array<bool, SLOT_COUNT> columnUsesMarkedSkill;

        // Cargas de Prescripto: +1 por cada skill marcada que se usa; con 9 aparece Furioso-Replica
        int prescriptStacks;

        // Acciones que puede usar Yi Sang este turno: 3, o 1 si Lei Heng tiene un solo ataque (Cortador de enredos)
        int availableActions;

        // Lo que hace cada uno en cada slot este turno (nullptr = slot vacio)
        std::array<const Skill*, SLOT_COUNT> playerActions;
        std::vector<const Skill*> enemyActions;

        // Chance de ganar el clash de cada columna (0 a 1). Un valor negativo significa que en esa columna no hay clash.
        std::array<float, SLOT_COUNT> winChances;

        // Chances ya calculadas este turno (se vacia en cada PrepareTurn)
        std::vector<WinChanceCacheEntry> winChanceCache;

        // true en las columnas sin accion de Yi Sang que cubriria una esquiva activa de una columna anterior
        std::array<bool, SLOT_COUNT> columnCoveredByEvade;

        // Botones de la mano: BUTTONS_PER_ACTION por cada accion (accion 1 primero)
        std::vector<Button> skillButtons;
        Button fightButton;
        Button restartButton;
        Button gameOverMenuButton;
        Button helpButton;

        // Cambia la velocidad con la que se muestra la pelea (x1 o x2)
        Button speedButton;
        Button resumeButton;
        Button pauseMenuButton;
        HelpPanel helpPanel;
        CombatLog combatLog;

        GameState state;
        // Con la pausa abierta no avanza la pelea y solo responden sus botones
        bool isPaused;

        // Se pidio volver al menu principal (lo lee el loop del juego)
        bool returnToMenuRequested;

        // Cuenta el tiempo desde que se cerro el ultimo cartel, para ignorar el segundo clic de un doble clic
        sf::Clock clickCooldownClock;
        int turn;
        int selectedSlot;
        sf::Vector2f mousePosition;

        // ---------- Pelea paso a paso ----------

        // Todo lo que paso en el turno, en orden, y por cual paso vamos
        CombatTimeline timeline;
        size_t nextEventIndex;
        float eventTimer;

        // Multiplica el tiempo de la pelea: 1 = normal, 2 = el doble de rapido
        float playbackSpeed;

        // true cuando ya se mostro "-1 moneda" para la ronda de clash que se esta viendo
        bool coinBreakShown;

        // Como queda la partida cuando termina de mostrarse el turno (sigue, victoria o derrota)
        GameState stateAfterPlayback;

        // Columna que se esta mostrando (-1 = ninguna)
        int activeColumn;

        // Vida que se ve en las barras: baja de a poco hasta la vida "real" de ese momento de la pelea
        float shownPlayerHp;
        float shownEnemyHp;
        float targetPlayerHp;
        float targetEnemyHp;

        std::vector<FloatingText> floatingTexts;

        std::string bannerText;
        float bannerTimeLeft;

        // Ronda de clash (o tirada de esquiva) que se esta mostrando en la columna activa
        CombatEvent shownClash;
        bool isShowingClash;

        // Tiempo que les queda girando a las monedas antes de mostrar como cayeron
        float coinSpinTimeLeft;

        // Tarjeta que se sacude cuando recibe un golpe
        int shakeColumn;
        bool shakeIsPlayerRow;
        float shakeTimeLeft;

        // Arena con las dos figuras: ocupa el lugar de la mano mientras se ve la pelea
        ArenaView arena;

        // Estadisticas de la partida (se muestran al ganar o perder)
        GameStats stats;

        // Partida en modo Dificil
        bool isHardMode;

        // Iconos de la cara de cada uno (arriba, al lado de las barras de vida)
        sf::Texture playerPortrait;
        sf::Texture enemyPortrait;
        bool isPlayerPortraitLoaded;
        bool isEnemyPortraitLoaded;

        // Cordura que se muestra arriba mientras se ve la pelea: va cambiando paso a paso, con cada clash
        // (la logica ya resolvio todo el turno, asi que la de las unidades es la del final)
        int shownPlayerSanity;
        int shownEnemySanity;
    };
}

#endif // GAMEPLAY_H
