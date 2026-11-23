#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <array>
#include <vector>

#include <SFML/Graphics.hpp>

#include "combat/enemy_pattern.h"
#include "combat/skill.h"
#include "combat/unit.h"
#include "ui/button.h"
#include "ui/combat_log.h"

namespace ReyesCuadra
{
    // Cantidad de slots por turno (Yi Sang tiene los mismos que Lei Heng)
    const int SLOT_COUNT = 6;

    // En que parte de la partida estamos
    enum class GameState
    {
        ChoosingSkills,
        Victory,
        Defeat
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

        // Hace los clashes de todas las columnas y despues se fija si alguien murio
        void ResolveTurn();

        // Reacciona a un clic izquierdo segun lo que haya debajo del mouse
        void HandleClick(sf::Vector2f clickPosition);

        // Rectangulo de la tarjeta de un slot (de Yi Sang o de Lei Heng)
        sf::FloatRect GetSlotBounds(int slotIndex, bool isPlayerRow) const;

        void DrawUnitPanels(sf::RenderWindow& window) const;
        void DrawSlots(sf::RenderWindow& window) const;
        void DrawSlotCard(sf::RenderWindow& window, sf::FloatRect bounds, const Skill* skill, sf::Color fillColor, bool isHighlighted) const;

        // Dibuja entre las dos filas que tan probable es ganar el clash de esa columna (o si no hay clash)
        void DrawClashPreview(sf::RenderWindow& window, int slotIndex, sf::FloatRect area) const;

        void DrawSkillButtons(sf::RenderWindow& window) const;
        void DrawGameOver(sf::RenderWindow& window) const;

        // Cartel con las reglas del juego
        void DrawHelp(sf::RenderWindow& window) const;

        // Detalle de la skill que esta debajo del mouse (efectos de Poise, Sinking, etc.), encima del registro de combate
        void DrawSkillTooltip(sf::RenderWindow& window) const;

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
        Skill tigerslayerFlurry;
        Skill tiantuiReload;

        EnemyPattern enemyPattern;

        // Lo que hace cada uno en cada slot este turno (nullptr = slot vacio)
        std::array<const Skill*, SLOT_COUNT> playerActions;
        std::vector<const Skill*> enemyActions;

        // Chance de ganar el clash de cada columna (0 a 1). Un valor negativo significa que en esa columna no hay clash.
        std::array<float, SLOT_COUNT> winChances;

        std::vector<Button> skillButtons;
        Button fightButton;
        Button restartButton;
        Button helpButton;
        Button closeHelpButton;
        CombatLog combatLog;

        GameState state;
        bool isShowingHelp;
        int turn;
        int selectedSlot;
        sf::Vector2f mousePosition;
    };
}

#endif // GAMEPLAY_H
