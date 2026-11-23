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

    // Umbrales de Stagger (proporcion de la vida maxima): Yi Sang a la mitad, Lei Heng al 70% y al 20%
    static const float PLAYER_STAGGER_THRESHOLD = 0.5f;
    static const float ENEMY_FIRST_STAGGER_THRESHOLD = 0.7f;
    static const float ENEMY_SECOND_STAGGER_THRESHOLD = 0.2f;

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
    static const float POISE_TEXT_OFFSET_Y = 4.0f;
    static const float THRESHOLD_MARK_WIDTH = 3.0f;
    static const float THRESHOLD_MARK_OVERHANG = 4.0f;

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

    // Cartel de "Como se juega" y el boton que lo abre
    static const sf::Vector2f HELP_BOX_SIZE = { 940.0f, 700.0f };
    static const float HELP_PADDING = 30.0f;
    static const float HELP_TITLE_HEIGHT = 50.0f;
    static const float HELP_LINE_HEIGHT = 23.0f;
    static const float HELP_RULE_GAP = 8.0f;
    static const sf::Vector2f CLOSE_HELP_BUTTON_SIZE = { 220.0f, 48.0f };
    static const sf::FloatRect HELP_BUTTON_BOUNDS = sf::FloatRect({ 1100.0f, 330.0f }, { 150.0f, 28.0f });

    // Cartel con el detalle de una skill (se muestra al pasar el mouse por su boton)
    static const float TOOLTIP_PADDING = 16.0f;
    static const float TOOLTIP_TITLE_LINE_HEIGHT = 26.0f;
    static const float TOOLTIP_LINE_HEIGHT = 23.0f;
    static const float TOOLTIP_SECTION_GAP = 8.0f;

    // A partir de que porcentaje un clash se considera favorable o desfavorable
    static const float FAVORABLE_CHANCE = 0.6f;
    static const float UNFAVORABLE_CHANCE = 0.4f;
    static const float PERCENT_MULTIPLIER = 100.0f;

    // ---------- Tamanos de texto ----------

    static const unsigned int NAME_SIZE = 26;
    static const unsigned int HP_TEXT_SIZE = 16;
    static const unsigned int POISE_TEXT_SIZE = 15;
    static const unsigned int TURN_SIZE = 30;
    static const unsigned int SLOT_NAME_SIZE = 16;
    static const unsigned int SLOT_SUMMARY_SIZE = 13;
    static const unsigned int VS_SIZE = 18;
    static const unsigned int HINT_SIZE = 15;
    static const unsigned int GAME_OVER_TITLE_SIZE = 48;
    static const unsigned int HELP_TITLE_SIZE = 34;
    static const unsigned int HELP_RULE_SIZE = 18;
    static const unsigned int TOOLTIP_TITLE_SIZE = 21;
    static const unsigned int TOOLTIP_TEXT_SIZE = 17;

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
    static const sf::Color STAGGER_COLOR = sf::Color(235, 110, 60);
    static const sf::Color BROKEN_THRESHOLD_COLOR = sf::Color(90, 85, 80);
    static const sf::Color FAVORABLE_COLOR = sf::Color(120, 200, 120);
    static const sf::Color EVEN_COLOR = sf::Color(225, 195, 95);
    static const sf::Color UNFAVORABLE_COLOR = sf::Color(215, 95, 85);
    static const float SLOT_BORDER_THICKNESS = 2.0f;
    static const float SELECTED_BORDER_THICKNESS = 4.0f;

    // Etiqueta corta que va delante del nombre de cada skill en los botones (mismo orden que PlayerSkillIndex)
    static const char* PLAYER_SKILL_TAGS[] = { "S1", "S2", "S3", "Defensa", "S3-2" };

    // Reglas que se muestran en el cartel de "Como se juega", una por parrafo
    static const char* HELP_RULES[] =
    {
        "1. Cada turno elegís una skill para cada uno de los 6 slots de Yi Sang (fila de abajo).",
        "2. Cada slot choca contra la skill de Lei Heng que tiene justo arriba. Eso es un clash.",
        "3. En el clash los dos tiran sus monedas: cada cara suma poder. El que suma más le rompe una moneda al otro, y se repite hasta que uno se queda sin monedas.",
        "4. El que gana el clash pega con las monedas que le quedan. El porcentaje que aparece entre las dos filas es tu chance de ganar ese clash: buscá los verdes.",
        "5. Esquivar no es un clash: tirás una sola vez y cada golpe de Lei Heng que no supere tu número no te hace daño. Ahí el % es tu chance de esquivar todos los golpes. Ojo: las monedas irrompibles de Lei Heng pegan aunque él pierda un clash.",
        "6. Poise: las skills de Yi Sang lo van cargando. Cada punto de potencia da 5% de chance de crítico (+20% de daño) y cada 6 de potencia suman +1 al poder de cada moneda. La cantidad baja 1 por crítico y 1 al final del turno; si llega a 0, se pierde.",
        "7. Sinking: las skills de Yi Sang se lo ponen a Lei Heng. Cada vez que lo golpean pierde cordura igual a la potencia (si ya está en -45, pierde vida) y la cantidad baja 1.",
        "8. Cordura (SP): va de -45 a +45 y cambia la chance de sacar cara: 50% + SP. Ganar un clash da +10 SP (más si duró varias rondas). Pasá el mouse por una skill para ver todos sus efectos.",
        "9. Stagger: las marcas en las barras de vida son umbrales. Si la vida baja de uno, esa unidad queda staggereada: pierde lo que le quedaba del turno, no actúa el turno siguiente y mientras tanto recibe el doble de daño."
    };

    // Color segun que tan buena es la chance: verde si es favorable, amarillo si es pareja y rojo si es desfavorable
    static sf::Color GetChanceColor(float winChance);

    // Palabra que acompana al porcentaje: "Favorable", "Parejo" o "Desfavorable"
    static std::string GetChanceVerdict(float winChance);

    // Pasa la chance (0 a 1) a un texto con porcentaje redondeado, por ejemplo "62%"
    static std::string FormatPercent(float winChance);

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
        , winChances()
        , fightButton(gameFont, sf::FloatRect({ MARGIN, FIGHT_BUTTON_Y }, { SKILL_BUTTON_WIDTH, FIGHT_BUTTON_HEIGHT }), "¡Combatir!", "")
        , restartButton(gameFont, sf::FloatRect({ (SCREEN_WIDTH - RESTART_BUTTON_SIZE.x) / 2.0f, SCREEN_HEIGHT / 2.0f + RESTART_BUTTON_OFFSET_Y - RESTART_BUTTON_SIZE.y / 2.0f }, RESTART_BUTTON_SIZE), "Jugar de nuevo", "")
        , helpButton(gameFont, HELP_BUTTON_BOUNDS, "¿Cómo se juega?", "")
        , closeHelpButton(gameFont, sf::FloatRect({ (SCREEN_WIDTH - CLOSE_HELP_BUTTON_SIZE.x) / 2.0f, (SCREEN_HEIGHT + HELP_BOX_SIZE.y) / 2.0f - HELP_PADDING - CLOSE_HELP_BUTTON_SIZE.y }, CLOSE_HELP_BUTTON_SIZE), "¡Entendido!", "")
        , combatLog(gameFont, COMBAT_LOG_AREA)
        , state(GameState::ChoosingSkills)
        , isShowingHelp(true)
        , turn(1)
        , selectedSlot(0)
        , mousePosition({ 0.0f, 0.0f })
    {
        player.SetStaggerThresholds({ PLAYER_STAGGER_THRESHOLD });
        enemy.SetStaggerThresholds({ ENEMY_FIRST_STAGGER_THRESHOLD, ENEMY_SECOND_STAGGER_THRESHOLD });

        // Numeros sacados de Yi Sang Index Nursefather (uptie 4): poder base, cantidad de monedas y poder por moneda
        playerSkills.push_back(Skill("Envolver 330 veces en largas franjas de sangre congelada", "S1: Envolver 330 veces", SkillType::Attack, 3, 2, 4, false));
        playerSkills.push_back(Skill("Regocíjate con aplausos silenciosos, empala en dolor silencioso", "S2: Regocíjate", SkillType::Attack, 4, 3, 4, false));
        playerSkills.push_back(Skill("Alza y ríe la hoja, llora la cascada como el aroma de las hojas caídas", "S3: Alza y ríe la hoja", SkillType::Attack, 4, 4, 3, false));
        playerSkills.push_back(Skill("Por un capricho impredecible", "Esquivar: Por un capricho", SkillType::Evade, 4, 1, 10, false));
        playerSkills.push_back(Skill("Furioso-Replica", "S3-2: Furioso-Replica", SkillType::Attack, 3, 9, 3, false));

        // Furioso-Replica se desbloquea mas adelante (con la mecanica de las 9 skills seguidas)
        playerSkills[SKILL_FURIOSO].SetLocked(true);

        // Poise que genera cada skill de Yi Sang (sacado de la wiki, uptie 4). Todas sus skills de ataque ganan
        // +1 de poder por moneda cada 6 de potencia de Poise (maximo +2).
        PoiseEffects skill1Poise;
        skill1Poise.onUseCount = 2;
        skill1Poise.onHitPotency = 2;
        skill1Poise.gainsCoinPowerFromPoise = true;
        playerSkills[SKILL_1].SetPoiseEffects(skill1Poise);

        PoiseEffects skill2Poise;
        skill2Poise.onUsePotency = 3;
        skill2Poise.onHitCount = 2;
        skill2Poise.gainsCoinPowerFromPoise = true;
        playerSkills[SKILL_2].SetPoiseEffects(skill2Poise);

        PoiseEffects skill3Poise;
        skill3Poise.onUseCount = 3;
        skill3Poise.onHitPotency = 3;
        skill3Poise.onCriticalCount = 1;
        skill3Poise.gainsCoinPowerFromPoise = true;
        playerSkills[SKILL_3].SetPoiseEffects(skill3Poise);

        PoiseEffects furiosoPoise;
        furiosoPoise.onUsePotency = 3;
        furiosoPoise.onUseCount = 3;
        furiosoPoise.onHitPotency = 1;
        furiosoPoise.gainsCoinPowerFromPoise = true;
        playerSkills[SKILL_FURIOSO].SetPoiseEffects(furiosoPoise);

        // Sinking que le pone cada skill de Yi Sang a Lei Heng (sacado de la wiki, uptie 4)
        SinkingEffects skill1Sinking;
        skill1Sinking.onHitWithoutCrackingPotency = 2;
        playerSkills[SKILL_1].SetSinkingEffects(skill1Sinking);

        SinkingEffects skill2Sinking;
        skill2Sinking.onHitPotency = 3;
        skill2Sinking.onHitWithoutCrackingCount = 1;
        playerSkills[SKILL_2].SetSinkingEffects(skill2Sinking);

        SinkingEffects skill3Sinking;
        skill3Sinking.onHitPotency = 3;
        skill3Sinking.onHitWithoutCrackingCount = 3;
        playerSkills[SKILL_3].SetSinkingEffects(skill3Sinking);

        SinkingEffects furiosoSinking;
        furiosoSinking.onHitPotency = 1;
        furiosoSinking.onHitWithoutCrackingCount = 3;
        playerSkills[SKILL_FURIOSO].SetSinkingEffects(furiosoSinking);

        // Un boton por skill, uno abajo del otro
        for (size_t skillIndex = 0; skillIndex < playerSkills.size(); skillIndex++)
        {
            const Skill& skill = playerSkills[skillIndex];
            const float buttonY = SKILL_BUTTONS_TOP + static_cast<float>(skillIndex) * (SKILL_BUTTON_HEIGHT + SKILL_BUTTON_GAP);
            const std::string label = std::string(PLAYER_SKILL_TAGS[skillIndex]) + " · " + skill.GetName();
            std::string subLabel = skill.GetSummary();

            if (skill.IsLocked())
            {
                subLabel += "  -  Bloqueada";
            }

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
        helpButton.Draw(window, mousePosition);

        // Si el mouse esta sobre una skill, se muestra su detalle encima del registro
        if (state == GameState::ChoosingSkills && !isShowingHelp)
        {
            DrawSkillTooltip(window);
        }

        if (state != GameState::ChoosingSkills)
        {
            DrawGameOver(window);
        }

        // La ayuda va arriba de todo
        if (isShowingHelp)
        {
            DrawHelp(window);
        }
    }

    void Gameplay::StartNewGame()
    {
        player.Reset();
        enemy.Reset();
        turn = 1;
        state = GameState::ChoosingSkills;

        combatLog.Clear();
        combatLog.AddLine("Elegí una skill para cada slot de Yi Sang y apretá ¡Combatir!");

        PrepareTurn();
    }

    void Gameplay::PrepareTurn()
    {
        // Una unidad staggereada no actua este turno
        if (enemy.IsStaggered())
        {
            enemyActions.clear();
        }
        else
        {
            enemyActions = enemyPattern.GetActionsForTurn(turn, enemy.GetHpRatio());
        }

        // Para no tener que elegir los 6 slots cada turno, arrancan todos con S1 (o vacios si Yi Sang esta staggereado)
        playerActions.fill(player.IsStaggered() ? nullptr : &playerSkills[SKILL_1]);
        selectedSlot = 0;

        UpdateWinChances();
    }

    void Gameplay::UpdateWinChances()
    {
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            const Skill* playerSkill = playerActions[static_cast<size_t>(slotIndex)];
            const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;

            // Solo hay clash si estan las dos skills y al menos una de las dos ataca
            const bool hasClash = playerSkill != nullptr && enemySkill != nullptr && !(playerSkill->IsDefensive() && enemySkill->IsDefensive());

            winChances[static_cast<size_t>(slotIndex)] = hasClash ? EstimateClashWinChance(player, *playerSkill, enemy, *enemySkill) : -1.0f;
        }

        // Si cambiaron las skills o el slot seleccionado, tambien cambian los numeros de los botones
        UpdateSkillButtonChances();
    }

    void Gameplay::UpdateSkillButtonChances()
    {
        const Skill* enemySkill = (selectedSlot < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(selectedSlot)] : nullptr;

        for (size_t skillIndex = 0; skillIndex < playerSkills.size(); skillIndex++)
        {
            const Skill& playerSkill = playerSkills[skillIndex];
            Button& button = skillButtons[skillIndex];

            if (playerSkill.IsLocked())
            {
                button.SetRightText("", TEXT_COLOR);
            }
            else if (enemySkill == nullptr)
            {
                // En ese slot Lei Heng no hace nada: no hay clash
                button.SetRightText("Libre", DIM_TEXT_COLOR);
            }
            else if (playerSkill.IsDefensive() && enemySkill->IsDefensive())
            {
                button.SetRightText("-", DIM_TEXT_COLOR);
            }
            else
            {
                const float winChance = EstimateClashWinChance(player, playerSkill, enemy, *enemySkill);

                button.SetRightText(FormatPercent(winChance), GetChanceColor(winChance));
            }
        }
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

            // Si alguno quedo staggereado en una columna anterior, pierde las acciones que le quedaban
            const Skill* playerSkill = player.IsStaggered() ? nullptr : playerActions[static_cast<size_t>(slotIndex)];
            const Skill* enemySkill = nullptr;

            if (!enemy.IsStaggered() && slotIndex < static_cast<int>(enemyActions.size()))
            {
                enemySkill = enemyActions[static_cast<size_t>(slotIndex)];
            }

            ResolveColumn(player, playerSkill, enemy, enemySkill, combatLog);

            if (player.ConsumeStaggerEvent())
            {
                combatLog.AddLine("¡Yi Sang queda staggereado! Pierde lo que le quedaba del turno y no actúa el próximo.");
            }

            if (enemy.ConsumeStaggerEvent())
            {
                combatLog.AddLine("¡Lei Heng queda staggereado! Pierde lo que le quedaba del turno y no actúa el próximo.");
            }
        }

        // Al final del turno, la cantidad de Poise baja 1 y los staggereados avanzan su recuperacion
        player.GetPoise().OnTurnEnd();
        enemy.GetPoise().OnTurnEnd();

        if (player.UpdateStaggerOnTurnEnd())
        {
            combatLog.AddLine("Yi Sang se recupera del Stagger.");
        }

        if (enemy.UpdateStaggerOnTurnEnd())
        {
            combatLog.AddLine("Lei Heng se recupera del Stagger.");
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
        // Con la ayuda abierta, cualquier clic la cierra
        if (isShowingHelp)
        {
            isShowingHelp = false;
            return;
        }

        // Con la partida terminada, lo unico que responde es el boton de jugar de nuevo
        if (state != GameState::ChoosingSkills)
        {
            if (restartButton.Contains(clickPosition))
            {
                StartNewGame();
            }

            return;
        }

        // Si Yi Sang esta staggereado no puede elegir skills: solo puede apretar Combatir (o la ayuda)
        if (player.IsStaggered())
        {
            if (fightButton.Contains(clickPosition))
            {
                ResolveTurn();
            }
            else if (helpButton.Contains(clickPosition))
            {
                isShowingHelp = true;
            }

            return;
        }

        // Clic en un slot de Yi Sang: lo selecciona para asignarle una skill
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            if (GetSlotBounds(slotIndex, true).contains(clickPosition))
            {
                selectedSlot = slotIndex;
                UpdateSkillButtonChances();
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
                UpdateWinChances();
                return;
            }
        }

        if (fightButton.Contains(clickPosition))
        {
            ResolveTurn();
        }
        else if (helpButton.Contains(clickPosition))
        {
            isShowingHelp = true;
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

            DrawSlotCard(window, enemyBounds, enemySkill, ENEMY_SLOT_COLOR, false, enemy.IsStaggered() ? "Staggereado" : "Sin acción");

            const sf::FloatRect previewArea({ enemyBounds.position.x, VS_ROW_Y }, { SLOT_WIDTH, VS_ROW_HEIGHT });
            DrawClashPreview(window, slotIndex, previewArea);

            // Fila de abajo: lo que eligio el jugador para Yi Sang
            const sf::FloatRect playerBounds = GetSlotBounds(slotIndex, true);
            const bool isHighlighted = (state == GameState::ChoosingSkills) && !player.IsStaggered() && (slotIndex == selectedSlot || playerBounds.contains(mousePosition));

            DrawSlotCard(window, playerBounds, playerActions[static_cast<size_t>(slotIndex)], PLAYER_SLOT_COLOR, isHighlighted, player.IsStaggered() ? "Staggereado" : "Sin acción");
        }

        if (player.IsStaggered())
        {
            DrawTextAt(window, font, "Yi Sang está staggereado: este turno no puede actuar. Apretá ¡Combatir! para seguir.", HINT_SIZE, { SLOTS_LEFT, HINT_Y }, STAGGER_COLOR);
        }
        else
        {
            DrawTextAt(window, font, "Clic en un slot de Yi Sang y después en una skill. El % entre las filas es tu chance de ganar ese clash.", HINT_SIZE, { SLOTS_LEFT, HINT_Y }, DIM_TEXT_COLOR);
        }
    }

    void Gameplay::DrawSlotCard(sf::RenderWindow& window, sf::FloatRect bounds, const Skill* skill, sf::Color fillColor, bool isHighlighted, const std::string& emptyText) const
    {
        sf::RectangleShape card(bounds.size);
        card.setPosition(bounds.position);
        card.setFillColor(skill != nullptr ? fillColor : EMPTY_SLOT_COLOR);
        card.setOutlineColor(isHighlighted ? GOLD_COLOR : SLOT_BORDER_COLOR);
        card.setOutlineThickness(isHighlighted ? SELECTED_BORDER_THICKNESS : SLOT_BORDER_THICKNESS);
        window.draw(card);

        if (skill == nullptr)
        {
            DrawTextCentered(window, font, emptyText, SLOT_NAME_SIZE, bounds, DIM_TEXT_COLOR);
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

    void Gameplay::DrawClashPreview(sf::RenderWindow& window, int slotIndex, sf::FloatRect area) const
    {
        const Skill* playerSkill = playerActions[static_cast<size_t>(slotIndex)];
        const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;
        const float winChance = winChances[static_cast<size_t>(slotIndex)];

        // Casos sin clash: no hay rival, o son dos defensas enfrentadas
        if (winChance < 0.0f)
        {
            std::string label = "No chocan";

            if (playerSkill == nullptr && enemySkill != nullptr)
            {
                label = "Te pega directo";
            }
            else if (playerSkill == nullptr)
            {
                label = "-";
            }
            else if (enemySkill == nullptr)
            {
                label = playerSkill->IsDefensive() ? "Sin rival" : "Sin rival: pegás directo";
            }

            DrawTextCentered(window, font, label, VS_SIZE, area, DIM_TEXT_COLOR);
            return;
        }

        DrawTextCentered(window, font, FormatPercent(winChance) + " · " + GetChanceVerdict(winChance), VS_SIZE, area, GetChanceColor(winChance));
    }

    void Gameplay::DrawSkillButtons(sf::RenderWindow& window) const
    {
        for (const Button& button : skillButtons)
        {
            button.Draw(window, mousePosition);
        }
    }

    void Gameplay::DrawSkillTooltip(sf::RenderWindow& window) const
    {
        for (size_t skillIndex = 0; skillIndex < skillButtons.size(); skillIndex++)
        {
            if (!skillButtons[skillIndex].Contains(mousePosition))
            {
                continue;
            }

            const Skill& skill = playerSkills[skillIndex];

            // El cartel tapa el registro de combate mientras el mouse este sobre la skill
            sf::RectangleShape panel(COMBAT_LOG_AREA.size);
            panel.setPosition(COMBAT_LOG_AREA.position);
            panel.setFillColor(GAME_OVER_BOX_COLOR);
            panel.setOutlineColor(GOLD_COLOR);
            panel.setOutlineThickness(SLOT_BORDER_THICKNESS);
            window.draw(panel);

            const float textX = COMBAT_LOG_AREA.position.x + TOOLTIP_PADDING;
            const float textWidth = COMBAT_LOG_AREA.size.x - TOOLTIP_PADDING * 2.0f;
            float lineY = COMBAT_LOG_AREA.position.y + TOOLTIP_PADDING / 2.0f;

            const std::string title = std::string(PLAYER_SKILL_TAGS[skillIndex]) + " · " + skill.GetName();

            for (const std::string& line : WrapText(font, title, TOOLTIP_TITLE_SIZE, textWidth))
            {
                DrawTextAt(window, font, line, TOOLTIP_TITLE_SIZE, { textX, lineY }, GOLD_COLOR);
                lineY += TOOLTIP_TITLE_LINE_HEIGHT;
            }

            DrawTextAt(window, font, skill.GetSummary(), TOOLTIP_TEXT_SIZE, { textX, lineY }, DIM_TEXT_COLOR);
            lineY += TOOLTIP_LINE_HEIGHT + TOOLTIP_SECTION_GAP;

            std::vector<std::string> effectLines = skill.GetEffectLines();

            if (effectLines.empty())
            {
                effectLines.push_back("Sin efectos especiales.");
            }

            for (const std::string& effect : effectLines)
            {
                for (const std::string& line : WrapText(font, effect, TOOLTIP_TEXT_SIZE, textWidth))
                {
                    DrawTextAt(window, font, line, TOOLTIP_TEXT_SIZE, { textX, lineY }, TEXT_COLOR);
                    lineY += TOOLTIP_LINE_HEIGHT;
                }
            }

            return;
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

    void Gameplay::DrawHelp(sf::RenderWindow& window) const
    {
        sf::RectangleShape overlay({ SCREEN_WIDTH, SCREEN_HEIGHT });
        overlay.setFillColor(OVERLAY_COLOR);
        window.draw(overlay);

        const sf::Vector2f boxPosition = { (SCREEN_WIDTH - HELP_BOX_SIZE.x) / 2.0f, (SCREEN_HEIGHT - HELP_BOX_SIZE.y) / 2.0f };

        sf::RectangleShape box(HELP_BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(GAME_OVER_BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(box);

        DrawTextCentered(window, font, "Cómo se juega", HELP_TITLE_SIZE, sf::FloatRect(boxPosition, { HELP_BOX_SIZE.x, HELP_TITLE_HEIGHT }), GOLD_COLOR);

        // Cada regla se corta en varias lineas para que entre en el ancho del cartel
        const float textWidth = HELP_BOX_SIZE.x - HELP_PADDING * 2.0f;
        float lineY = boxPosition.y + HELP_TITLE_HEIGHT + HELP_RULE_GAP;

        for (const char* rule : HELP_RULES)
        {
            for (const std::string& line : WrapText(font, rule, HELP_RULE_SIZE, textWidth))
            {
                DrawTextAt(window, font, line, HELP_RULE_SIZE, { boxPosition.x + HELP_PADDING, lineY }, TEXT_COLOR);
                lineY += HELP_LINE_HEIGHT;
            }

            lineY += HELP_RULE_GAP;
        }

        closeHelpButton.Draw(window, mousePosition);
    }

    static sf::Color GetChanceColor(float winChance)
    {
        if (winChance >= FAVORABLE_CHANCE)
        {
            return FAVORABLE_COLOR;
        }

        if (winChance < UNFAVORABLE_CHANCE)
        {
            return UNFAVORABLE_COLOR;
        }

        return EVEN_COLOR;
    }

    static std::string GetChanceVerdict(float winChance)
    {
        if (winChance >= FAVORABLE_CHANCE)
        {
            return "Favorable";
        }

        if (winChance < UNFAVORABLE_CHANCE)
        {
            return "Desfavorable";
        }

        return "Parejo";
    }

    static std::string FormatPercent(float winChance)
    {
        // Se suma 0.5 antes de pasar a entero para redondear en vez de cortar
        const int percent = static_cast<int>(winChance * PERCENT_MULTIPLIER + 0.5f);

        return std::to_string(percent) + "%";
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

        // Marcas de los umbrales de Stagger: doradas si todavia no se rompieron, grises si ya se rompieron
        const std::vector<float>& thresholds = unit.GetStaggerThresholds();

        for (size_t thresholdIndex = 0; thresholdIndex < thresholds.size(); thresholdIndex++)
        {
            sf::RectangleShape mark({ THRESHOLD_MARK_WIDTH, HP_BAR_HEIGHT + THRESHOLD_MARK_OVERHANG * 2.0f });
            mark.setPosition({ barX + HP_BAR_WIDTH * thresholds[thresholdIndex] - THRESHOLD_MARK_WIDTH / 2.0f, barY - THRESHOLD_MARK_OVERHANG });
            mark.setFillColor(unit.IsThresholdBroken(thresholdIndex) ? BROKEN_THRESHOLD_COLOR : GOLD_COLOR);
            window.draw(mark);
        }

        const std::string hpText = std::to_string(unit.GetHp()) + " / " + std::to_string(unit.GetMaxHp());
        DrawTextCentered(window, font, hpText, HP_TEXT_SIZE, sf::FloatRect({ barX, barY }, { HP_BAR_WIDTH, HP_BAR_HEIGHT }), TEXT_COLOR);

        // Abajo de la barra se muestra la cordura y los estados que tenga (Poise y Sinking), en una sola linea
        const Poise& poise = unit.GetPoise();
        const Sinking& sinking = unit.GetSinking();
        const std::string sanitySign = (unit.GetSanity() > 0) ? "+" : "";
        std::string statusText = unit.IsStaggered() ? "STAGGER     SP " + sanitySign : "SP " + sanitySign + std::to_string(unit.GetSanity()) + " (" + FormatPercent(unit.GetHeadsChance()) + " cara)";

        if (poise.IsActive())
        {
            statusText += "     Poise " + std::to_string(poise.GetPotency()) + " pot. · " + std::to_string(poise.GetCount()) + " cant. (" + FormatPercent(poise.GetCriticalChance()) + " crítico)";
        }

        if (sinking.IsActive())
        {
            statusText += "     Sinking " + std::to_string(sinking.GetPotency()) + " pot. · " + std::to_string(sinking.GetCount()) + " cant.";
        }

        const float statusY = barY + HP_BAR_HEIGHT + POISE_TEXT_OFFSET_Y;
        const sf::Color statusColor = unit.IsStaggered() ? STAGGER_COLOR : GOLD_COLOR;

        if (isRightSide)
        {
            DrawTextRightAligned(window, font, statusText, POISE_TEXT_SIZE, { barX + HP_BAR_WIDTH, statusY }, statusColor);
        }
        else
        {
            DrawTextAt(window, font, statusText, POISE_TEXT_SIZE, { barX, statusY }, statusColor);
        }
    }
}
