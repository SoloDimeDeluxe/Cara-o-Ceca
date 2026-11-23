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

namespace ReyesCuadra
{
    // Cantidad de columnas por turno: Lei Heng usa hasta 6 acciones, una por columna
    const int SLOT_COUNT = 6;

    // Cantidad de acciones de Yi Sang por turno: las pone en las columnas que quiera frenar
    const int PLAYER_MAX_ACTIONS = 3;

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

    // Escena del combate: Yi Sang contra Lei Heng, por turnos
    class Gameplay
    {
    public:
        explicit Gameplay(const sf::Font& gameFont);

        // Como guarda punteros a sus propias skills, no se puede copiar
        Gameplay(const Gameplay&) = delete;
        Gameplay& operator=(const Gameplay&) = delete;

        // Recibe los eventos de la ventana (movimiento y clics del mouse)
        void HandleEvent(const sf::Event& event);

        // Avanza las animaciones (la pelea paso a paso, los numeros flotantes, las barras de vida)
        void Update(float deltaSeconds);

        // Dibuja todo el combate
        void Draw(sf::RenderWindow& window) const;

    private:
        // Deja todo como al principio: vida llena, turno 1 y registro vacio
        void StartNewGame();

        // Pide al patron las skills de Lei Heng para el turno actual y le pone S1 por defecto a todos los slots de Yi Sang
        void PrepareTurn();

        // Recalcula el porcentaje de victoria de cada columna (se llama cada vez que cambia alguna skill)
        void UpdateWinChances();

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

        // Dibuja entre las dos filas que tan probable es ganar el clash de esa columna (o si no hay clash)
        void DrawClashPreview(sf::RenderWindow& window, int slotIndex, sf::FloatRect area) const;

        void DrawSkillButtons(sf::RenderWindow& window) const;
        void DrawGameOver(sf::RenderWindow& window) const;

        // Cartel con las reglas del juego
        void DrawHelp(sf::RenderWindow& window) const;

        // Detalle de la skill que esta debajo del mouse (de Yi Sang o de Lei Heng), encima del registro de combate
        void DrawSkillTooltip(sf::RenderWindow& window) const;

        // Dibuja el cartel con el titulo, los numeros y los efectos de una skill
        void DrawSkillDetails(sf::RenderWindow& window, const Skill& skill, const std::string& title) const;

        // Animaciones de la pelea: columna activa, numeros flotantes y carteles grandes
        void DrawActiveColumn(sf::RenderWindow& window) const;
        void DrawFloatingTexts(sf::RenderWindow& window) const;
        void DrawBanner(sf::RenderWindow& window) const;

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

        // Lo que hace cada uno en cada slot este turno (nullptr = slot vacio)
        std::array<const Skill*, SLOT_COUNT> playerActions;
        std::vector<const Skill*> enemyActions;

        // Chance de ganar el clash de cada columna (0 a 1). Un valor negativo significa que en esa columna no hay clash.
        std::array<float, SLOT_COUNT> winChances;

        // true en las columnas sin accion de Yi Sang que cubriria una esquiva activa de una columna anterior
        std::array<bool, SLOT_COUNT> columnCoveredByEvade;

        std::vector<Button> skillButtons;
        Button fightButton;
        Button restartButton;
        Button helpButton;
        Button closeHelpButton;
        CombatLog combatLog;

        GameState state;
        bool isShowingHelp;

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

        // Tarjeta que se sacude cuando recibe un golpe
        int shakeColumn;
        bool shakeIsPlayerRow;
        float shakeTimeLeft;
    };
}

#endif // GAMEPLAY_H
