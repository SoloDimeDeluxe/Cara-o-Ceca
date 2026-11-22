#include "scenes/gameplay.h"

#include <string>

#include "combat/clash.h"
#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // ---------- Datos de las unidades ----------

    static const char* PLAYER_NAME = "Yi Sang";
    static const int PLAYER_MAX_HP = 263;
    static const sf::Color PLAYER_COLOR = sf::Color(70, 140, 150);

    static const char* ENEMY_NAME = "Lei Heng";
    static const int ENEMY_MAX_HP = 4239;
    static const sf::Color ENEMY_COLOR = sf::Color(170, 60, 50);

    // Posicion de cada skill de Yi Sang dentro de playerSkills (y de los botones)
    enum PlayerSkillIndex
    {
        SKILL_1 = 0,
        SKILL_2,
        SKILL_3,
        SKILL_DEFENSE,
        SKILL_FURIOSO
    };

    // ---------- Layout de la pantalla (1280 x 720) ----------

    // Paneles de arriba con el nombre y la vida de cada unidad
    static const float MARGIN = 30.0f;
    static const float PORTRAIT_SIZE = 64.0f;
    static const float PANEL_TOP = 16.0f;
    static const float NAME_OFFSET_X = 80.0f;
    static const float HP_BAR_OFFSET_Y = 40.0f;
    static const float HP_BAR_WIDTH = 420.0f;
    static const float HP_BAR_HEIGHT = 22.0f;
    static const float SCREEN_WIDTH = 1280.0f;
    static const float SCREEN_HEIGHT = 720.0f;
    static const float TURN_LABEL_Y = 24.0f;

    // Tarjetas de los slots: una fila para Lei Heng arriba y otra para Yi Sang abajo
    static const float SLOTS_LEFT = 30.0f;
    static const float SLOT_WIDTH = 195.0f;
    static const float SLOT_HEIGHT = 92.0f;
    static const float SLOT_GAP = 10.0f;
    static const float ENEMY_ROW_Y = 108.0f;
    static const float PLAYER_ROW_Y = 236.0f;
    static const float VS_ROW_Y = 202.0f;
    static const float VS_ROW_HEIGHT = 32.0f;
    static const float SLOT_TEXT_PADDING = 8.0f;
    static const float SLOT_NAME_LINE_HEIGHT = 20.0f;
    static const float SLOT_SUMMARY_LINE_HEIGHT = 16.0f;
    static const float HINT_Y = 334.0f;

    // Botones de skills (abajo a la izquierda) y registro de combate (abajo a la derecha)
    static const float SKILL_BUTTONS_TOP = 364.0f;
    static const float SKILL_BUTTON_WIDTH = 620.0f;
    static const float SKILL_BUTTON_HEIGHT = 44.0f;
    static const float SKILL_BUTTON_GAP = 8.0f;
    static const float FIGHT_BUTTON_Y = 636.0f;
    static const float FIGHT_BUTTON_HEIGHT = 54.0f;
    static const sf::FloatRect COMBAT_LOG_AREA = sf::FloatRect({ 670.0f, 364.0f }, { 580.0f, 326.0f });

    // Cartel de fin de partida
    static const sf::Vector2f GAME_OVER_BOX_SIZE = { 520.0f, 240.0f };
    static const sf::Vector2f RESTART_BUTTON_SIZE = { 260.0f, 54.0f };
    static const float RESTART_BUTTON_OFFSET_Y = 60.0f;

    // ---------- Tamanos de texto ----------

    static const unsigned int NAME_SIZE = 26;
    static const unsigned int HP_TEXT_SIZE = 16;
    static const unsigned int TURN_SIZE = 30;
    static const unsigned int SLOT_NAME_SIZE = 16;
    static const unsigned int SLOT_SUMMARY_SIZE = 13;
    static const unsigned int VS_SIZE = 18;
    static const unsigned int HINT_SIZE = 15;
    static const unsigned int GAME_OVER_TITLE_SIZE = 48;

    // ---------- Colores ----------

    static const sf::Color TEXT_COLOR = sf::Color(235, 230, 220);
    static const sf::Color DIM_TEXT_COLOR = sf::Color(150, 140, 130);
    static const sf::Color GOLD_COLOR = sf::Color(220, 190, 120);
    static const sf::Color HP_BAR_BACKGROUND = sf::Color(50, 20, 20);
    static const sf::Color HP_BAR_FILL = sf::Color(190, 50, 45);
    static const sf::Color ENEMY_SLOT_COLOR = sf::Color(70, 32, 32);
    static const sf::Color PLAYER_SLOT_COLOR = sf::Color(28, 58, 64);
    static const sf::Color EMPTY_SLOT_COLOR = sf::Color(35, 33, 37);
    static const sf::Color SLOT_BORDER_COLOR = sf::Color(90, 80, 70);
    static const sf::Color OVERLAY_COLOR = sf::Color(0, 0, 0, 170);
    static const sf::Color GAME_OVER_BOX_COLOR = sf::Color(30, 27, 33);
    static const float SLOT_BORDER_THICKNESS = 2.0f;
    static const float SELECTED_BORDER_THICKNESS = 4.0f;

    // Etiqueta corta que va delante del nombre de cada skill en los botones (mismo orden que PlayerSkillIndex)
    static const char* PLAYER_SKILL_TAGS[] = { "S1", "S2", "S3", "Defensa", "S3-2" };

    // Dibuja el cuadrado de color, el nombre y la barra de vida de una unidad. Si isRightSide es true, se dibuja espejado a la derecha.
    static void DrawUnitPanel(sf::RenderWindow& window, const sf::Font& font, const Unit& unit, bool isRightSide);

    Gameplay::Gameplay(const sf::Font& gameFont)
        : font(gameFont)
        , player(PLAYER_NAME, PLAYER_MAX_HP, PLAYER_COLOR)
        , enemy(ENEMY_NAME, ENEMY_MAX_HP, ENEMY_COLOR)
        , doubleSlash("Doble corte", "Doble corte", SkillType::Attack, 4, 2, 2, false)
        , tripleSlash("Triple corte", "Triple corte", SkillType::Attack, 3, 3, 2, false)
        , explosiveSlash("Tajo Destrozador Explosivo", "Tajo Destrozador Explosivo", SkillType::Attack, 4, 3, 3, true)
        , tangleCutter("Cortador de enredos", "Cortador de enredos", SkillType::Attack, 8, 1, 12, true)
        , tigerslayerFlurry("Ráfaga de espadas perfeccionada marca Savage Tigerslayer", "Ráfaga Savage Tigerslayer", SkillType::Attack, 3, 6, 3, true)
        , tiantuiReload("Recargando espada tiantui", "Recargando espada tiantui", SkillType::Counter, 4, 2, 4, false)
        , enemyPattern(doubleSlash, tripleSlash, explosiveSlash, tangleCutter, tigerslayerFlurry)
        , playerActions()
        , fightButton(gameFont, sf::FloatRect({ MARGIN, FIGHT_BUTTON_Y }, { SKILL_BUTTON_WIDTH, FIGHT_BUTTON_HEIGHT }), "¡Combatir!", "")
        , restartButton(gameFont, sf::FloatRect({ (SCREEN_WIDTH - RESTART_BUTTON_SIZE.x) / 2.0f, SCREEN_HEIGHT / 2.0f + RESTART_BUTTON_OFFSET_Y - RESTART_BUTTON_SIZE.y / 2.0f }, RESTART_BUTTON_SIZE), "Jugar de nuevo", "")
        , combatLog(gameFont, COMBAT_LOG_AREA)
        , state(GameState::ChoosingSkills)
        , turn(1)
        , selectedSlot(0)
        , mousePosition({ 0.0f, 0.0f })
    {
        // Numeros sacados de Yi Sang Index Nursefather (uptie 4): poder base, cantidad de monedas y poder por moneda
        playerSkills.push_back(Skill("Envolver 330 veces en largas franjas de sangre congelada", "S1: Envolver 330 veces", SkillType::Attack, 3, 2, 4, false));
        playerSkills.push_back(Skill("Regocíjate con aplausos silenciosos, empala en dolor silencioso", "S2: Regocíjate", SkillType::Attack, 4, 3, 4, false));
        playerSkills.push_back(Skill("Alza y ríe la hoja, llora la cascada como el aroma de las hojas caídas", "S3: Alza y ríe la hoja", SkillType::Attack, 4, 4, 3, false));
        playerSkills.push_back(Skill("Por un capricho impredecible", "Esquivar: Por un capricho", SkillType::Evade, 4, 1, 10, false));
        playerSkills.push_back(Skill("Furioso-Replica", "S3-2: Furioso-Replica", SkillType::Attack, 3, 9, 3, false));

        // Furioso-Replica se desbloquea mas adelante (con la mecanica de las 9 skills seguidas)
        playerSkills[SKILL_FURIOSO].SetLocked(true);

        // Un boton por skill, uno abajo del otro
        for (size_t skillIndex = 0; skillIndex < playerSkills.size(); skillIndex++)
        {
            const Skill& skill = playerSkills[skillIndex];
            const float buttonY = SKILL_BUTTONS_TOP + static_cast<float>(skillIndex) * (SKILL_BUTTON_HEIGHT + SKILL_BUTTON_GAP);
            const std::string label = std::string(PLAYER_SKILL_TAGS[skillIndex]) + " · " + skill.GetName();
            const std::string subLabel = skill.IsLocked() ? skill.GetSummary() + "  -  Bloqueada" : skill.GetSummary();

            skillButtons.push_back(Button(gameFont, sf::FloatRect({ MARGIN, buttonY }, { SKILL_BUTTON_WIDTH, SKILL_BUTTON_HEIGHT }), label, subLabel));
            skillButtons.back().SetEnabled(!skill.IsLocked());
        }

        StartNewGame();
    }

    void Gameplay::HandleEvent(const sf::Event& event)
    {
        // Se guarda la posicion del mouse para resaltar el boton o el slot que esta debajo
        if (const sf::Event::MouseMoved* moved = event.getIf<sf::Event::MouseMoved>())
        {
            mousePosition = sf::Vector2f(moved->position);
        }
        else if (const sf::Event::MouseButtonPressed* pressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (pressed->button == sf::Mouse::Button::Left)
            {
                HandleClick(sf::Vector2f(pressed->position));
            }
        }
    }

    void Gameplay::Draw(sf::RenderWindow& window) const
    {
        DrawUnitPanels(window);
        DrawSlots(window);
        DrawSkillButtons(window);
        fightButton.Draw(window, mousePosition);
        combatLog.Draw(window);

        if (state != GameState::ChoosingSkills)
        {
            DrawGameOver(window);
        }
    }

    void Gameplay::StartNewGame()
    {
        player.ResetHp();
        enemy.ResetHp();
        turn = 1;
        state = GameState::ChoosingSkills;

        combatLog.Clear();
        combatLog.AddLine("Elegí una skill para cada slot de Yi Sang y apretá ¡Combatir!");

        PrepareTurn();
    }

    void Gameplay::PrepareTurn()
    {
        enemyActions = enemyPattern.GetActionsForTurn(turn, enemy.GetHpRatio());

        // Para no tener que elegir los 6 slots cada turno, arrancan todos con S1
        playerActions.fill(&playerSkills[SKILL_1]);
        selectedSlot = 0;
    }

    void Gameplay::ResolveTurn()
    {
        combatLog.AddLine("-- Turno " + std::to_string(turn) + " --");

        // Cada slot de Yi Sang se enfrenta con el slot de Lei Heng que tiene justo arriba
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            if (player.IsDead() || enemy.IsDead())
            {
                break;
            }

            const Skill* playerSkill = playerActions[static_cast<size_t>(slotIndex)];
            const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;

            ResolveColumn(player, playerSkill, enemy, enemySkill, combatLog);
        }

        if (enemy.IsDead())
        {
            state = GameState::Victory;
            combatLog.AddLine("¡Lei Heng cayó!");
        }
        else if (player.IsDead())
        {
            state = GameState::Defeat;
            combatLog.AddLine("Yi Sang cayó...");
        }
        else
        {
            turn++;
            PrepareTurn();
        }
    }

    void Gameplay::HandleClick(sf::Vector2f clickPosition)
    {
        // Con la partida terminada, lo unico que responde es el boton de jugar de nuevo
        if (state != GameState::ChoosingSkills)
        {
            if (restartButton.Contains(clickPosition))
            {
                StartNewGame();
            }

            return;
        }

        // Clic en un slot de Yi Sang: lo selecciona para asignarle una skill
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            if (GetSlotBounds(slotIndex, true).contains(clickPosition))
            {
                selectedSlot = slotIndex;
                return;
            }
        }

        // Clic en una skill: se la pone al slot seleccionado y pasa solo al siguiente slot
        for (size_t buttonIndex = 0; buttonIndex < skillButtons.size(); buttonIndex++)
        {
            if (skillButtons[buttonIndex].IsEnabled() && skillButtons[buttonIndex].Contains(clickPosition))
            {
                playerActions[static_cast<size_t>(selectedSlot)] = &playerSkills[buttonIndex];
                selectedSlot = (selectedSlot + 1) % SLOT_COUNT;
                return;
            }
        }

        if (fightButton.Contains(clickPosition))
        {
            ResolveTurn();
        }
    }

    sf::FloatRect Gameplay::GetSlotBounds(int slotIndex, bool isPlayerRow) const
    {
        const float slotX = SLOTS_LEFT + static_cast<float>(slotIndex) * (SLOT_WIDTH + SLOT_GAP);
        const float slotY = isPlayerRow ? PLAYER_ROW_Y : ENEMY_ROW_Y;

        return sf::FloatRect({ slotX, slotY }, { SLOT_WIDTH, SLOT_HEIGHT });
    }

    void Gameplay::DrawUnitPanels(sf::RenderWindow& window) const
    {
        DrawUnitPanel(window, font, player, false);
        DrawUnitPanel(window, font, enemy, true);

        // Numero de turno en el medio de la pantalla
        const sf::FloatRect turnArea({ 0.0f, TURN_LABEL_Y }, { SCREEN_WIDTH, static_cast<float>(TURN_SIZE) });
        DrawTextCentered(window, font, "Turno " + std::to_string(turn), TURN_SIZE, turnArea, GOLD_COLOR);
    }

    void Gameplay::DrawSlots(sf::RenderWindow& window) const
    {
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            // Fila de arriba: lo que hace Lei Heng (puede tener menos acciones que slots, por ejemplo en el turno del ataque fuerte)
            const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;
            const sf::FloatRect enemyBounds = GetSlotBounds(slotIndex, false);

            DrawSlotCard(window, enemyBounds, enemySkill, ENEMY_SLOT_COLOR, false);

            const sf::FloatRect vsArea({ enemyBounds.position.x, VS_ROW_Y }, { SLOT_WIDTH, VS_ROW_HEIGHT });
            DrawTextCentered(window, font, "VS", VS_SIZE, vsArea, DIM_TEXT_COLOR);

            // Fila de abajo: lo que eligio el jugador para Yi Sang
            const sf::FloatRect playerBounds = GetSlotBounds(slotIndex, true);
            const bool isHighlighted = (state == GameState::ChoosingSkills) && (slotIndex == selectedSlot || playerBounds.contains(mousePosition));

            DrawSlotCard(window, playerBounds, playerActions[static_cast<size_t>(slotIndex)], PLAYER_SLOT_COLOR, isHighlighted);
        }

        DrawTextAt(window, font, "Hacé clic en un slot de Yi Sang y después en una skill de la lista para asignársela.", HINT_SIZE, { SLOTS_LEFT, HINT_Y }, DIM_TEXT_COLOR);
    }

    void Gameplay::DrawSlotCard(sf::RenderWindow& window, sf::FloatRect bounds, const Skill* skill, sf::Color fillColor, bool isHighlighted) const
    {
        sf::RectangleShape card(bounds.size);
        card.setPosition(bounds.position);
        card.setFillColor(skill != nullptr ? fillColor : EMPTY_SLOT_COLOR);
        card.setOutlineColor(isHighlighted ? GOLD_COLOR : SLOT_BORDER_COLOR);
        card.setOutlineThickness(isHighlighted ? SELECTED_BORDER_THICKNESS : SLOT_BORDER_THICKNESS);
        window.draw(card);

        if (skill == nullptr)
        {
            DrawTextCentered(window, font, "Sin acción", SLOT_NAME_SIZE, bounds, DIM_TEXT_COLOR);
            return;
        }

        const float textWidth = bounds.size.x - SLOT_TEXT_PADDING * 2.0f;
        float textY = bounds.position.y + SLOT_TEXT_PADDING / 2.0f;

        // Nombre de la skill arriba (en varias lineas si no entra) y los numeros abajo
        for (const std::string& line : WrapText(font, skill->GetShortName(), SLOT_NAME_SIZE, textWidth))
        {
            DrawTextAt(window, font, line, SLOT_NAME_SIZE, { bounds.position.x + SLOT_TEXT_PADDING, textY }, TEXT_COLOR);
            textY += SLOT_NAME_LINE_HEIGHT;
        }

        for (const std::string& line : WrapText(font, skill->GetSummary(), SLOT_SUMMARY_SIZE, textWidth))
        {
            DrawTextAt(window, font, line, SLOT_SUMMARY_SIZE, { bounds.position.x + SLOT_TEXT_PADDING, textY }, DIM_TEXT_COLOR);
            textY += SLOT_SUMMARY_LINE_HEIGHT;
        }
    }

    void Gameplay::DrawSkillButtons(sf::RenderWindow& window) const
    {
        for (const Button& button : skillButtons)
        {
            button.Draw(window, mousePosition);
        }
    }

    void Gameplay::DrawGameOver(sf::RenderWindow& window) const
    {
        // Se oscurece toda la pantalla y encima va el cartel con el resultado
        sf::RectangleShape overlay({ SCREEN_WIDTH, SCREEN_HEIGHT });
        overlay.setFillColor(OVERLAY_COLOR);
        window.draw(overlay);

        const sf::Vector2f boxPosition = { (SCREEN_WIDTH - GAME_OVER_BOX_SIZE.x) / 2.0f, (SCREEN_HEIGHT - GAME_OVER_BOX_SIZE.y) / 2.0f };

        sf::RectangleShape box(GAME_OVER_BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(GAME_OVER_BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(box);

        const std::string title = (state == GameState::Victory) ? "¡Victoria!" : "Derrota";
        const sf::FloatRect titleArea(boxPosition, { GAME_OVER_BOX_SIZE.x, GAME_OVER_BOX_SIZE.y / 2.0f });

        DrawTextCentered(window, font, title, GAME_OVER_TITLE_SIZE, titleArea, GOLD_COLOR);
        restartButton.Draw(window, mousePosition);
    }

    static void DrawUnitPanel(sf::RenderWindow& window, const sf::Font& font, const Unit& unit, bool isRightSide)
    {
        // Por ahora cada personaje es un cuadrado de color; despues se puede cambiar por un sprite
        const float portraitX = isRightSide ? SCREEN_WIDTH - MARGIN - PORTRAIT_SIZE : MARGIN;

        sf::RectangleShape portrait({ PORTRAIT_SIZE, PORTRAIT_SIZE });
        portrait.setPosition({ portraitX, PANEL_TOP });
        portrait.setFillColor(unit.GetColor());
        portrait.setOutlineColor(SLOT_BORDER_COLOR);
        portrait.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(portrait);

        // Nombre y barra de vida al lado del cuadrado (a la derecha para Yi Sang, a la izquierda para Lei Heng)
        const float barX = isRightSide ? SCREEN_WIDTH - MARGIN - NAME_OFFSET_X - HP_BAR_WIDTH : MARGIN + NAME_OFFSET_X;
        const float barY = PANEL_TOP + HP_BAR_OFFSET_Y;

        if (isRightSide)
        {
            DrawTextRightAligned(window, font, unit.GetName(), NAME_SIZE, { barX + HP_BAR_WIDTH, PANEL_TOP - SLOT_TEXT_PADDING / 2.0f }, TEXT_COLOR);
        }
        else
        {
            DrawTextAt(window, font, unit.GetName(), NAME_SIZE, { barX, PANEL_TOP - SLOT_TEXT_PADDING / 2.0f }, TEXT_COLOR);
        }

        sf::RectangleShape barBackground({ HP_BAR_WIDTH, HP_BAR_HEIGHT });
        barBackground.setPosition({ barX, barY });
        barBackground.setFillColor(HP_BAR_BACKGROUND);
        barBackground.setOutlineColor(SLOT_BORDER_COLOR);
        barBackground.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(barBackground);

        sf::RectangleShape barFill({ HP_BAR_WIDTH * unit.GetHpRatio(), HP_BAR_HEIGHT });
        barFill.setPosition({ barX, barY });
        barFill.setFillColor(HP_BAR_FILL);
        window.draw(barFill);

        const std::string hpText = std::to_string(unit.GetHp()) + " / " + std::to_string(unit.GetMaxHp());
        DrawTextCentered(window, font, hpText, HP_TEXT_SIZE, sf::FloatRect({ barX, barY }, { HP_BAR_WIDTH, HP_BAR_HEIGHT }), TEXT_COLOR);
    }
}
