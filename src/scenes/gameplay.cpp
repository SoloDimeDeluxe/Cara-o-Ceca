#include "scenes/gameplay.h"

#include <cmath>
#include <string>

#include "combat/clash.h"
#include "ui/help_panel.h"
#include "ui/text_utils.h"
#include "utils/random.h"

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

    // Lei Heng arranca debil y cada turno pega mas fuerte (multiplica el dano de sus golpes), hasta llegar al 100%.
    // Asi al principio hay tiempo de juntar SP y Poise antes de que pegue con todo.
    static const float ENEMY_STARTING_STRENGTH = 0.3f;
    static const float ENEMY_STRENGTH_PER_TURN = 0.1f;
    static const float ENEMY_MAX_STRENGTH = 1.0f;

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
    static const float STRENGTH_LABEL_Y = 62.0f;
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

    // Mano de Yi Sang: una fila por accion, con un titulo arriba y los botones (2 opciones + Esquivar) abajo
    static const float HAND_ROW_HEIGHT = 86.0f;
    static const float HAND_HEADER_HEIGHT = 22.0f;
    static const float HAND_BUTTON_HEIGHT = 52.0f;
    static const float OPTION_BUTTON_WIDTH = 226.0f;
    static const float EVADE_BUTTON_WIDTH = 152.0f;
    static const float HAND_BUTTON_GAP = 8.0f;

    // Probabilidad de que salga cada skill en la mano (S3 es lo que queda: 10%)
    static const float SKILL_1_DRAW_CHANCE = 0.6f;
    static const float SKILL_2_DRAW_CHANCE = 0.3f;
    static const float FIGHT_BUTTON_Y = 636.0f;
    static const float FIGHT_BUTTON_HEIGHT = 54.0f;
    static const sf::FloatRect COMBAT_LOG_AREA = sf::FloatRect({ 670.0f, 364.0f }, { 580.0f, 326.0f });

    // Cartel de fin de partida: dos botones uno al lado del otro (Jugar de nuevo y Menu principal)
    static const sf::Vector2f GAME_OVER_BOX_SIZE = { 520.0f, 240.0f };
    static const sf::Vector2f GAME_OVER_BUTTON_SIZE = { 220.0f, 54.0f };
    static const float GAME_OVER_BUTTON_GAP = 20.0f;
    static const float GAME_OVER_BUTTON_OFFSET_Y = 60.0f;

    // Cartel de pausa (Esc): dos botones uno abajo del otro
    static const sf::Vector2f PAUSE_BOX_SIZE = { 420.0f, 280.0f };
    static const sf::Vector2f PAUSE_BUTTON_SIZE = { 260.0f, 54.0f };
    static const float PAUSE_TITLE_HEIGHT = 90.0f;
    static const float PAUSE_BUTTON_GAP = 16.0f;

    // Boton que abre el cartel de "Como se juega"
    static const sf::FloatRect HELP_BUTTON_BOUNDS = sf::FloatRect({ 1100.0f, 330.0f }, { 150.0f, 28.0f });

    // Cartel con el detalle de una skill (se muestra al pasar el mouse por su boton)
    static const float TOOLTIP_PADDING = 16.0f;
    static const float TOOLTIP_TITLE_LINE_HEIGHT = 26.0f;
    static const float TOOLTIP_LINE_HEIGHT = 23.0f;
    static const float TOOLTIP_SECTION_GAP = 8.0f;

    // Despues de cerrar un cartel (ayuda o fin de partida) se ignoran los clics un ratito,
    // asi un doble clic no "atraviesa" el cartel y aprieta lo que hay abajo (por ejemplo, Combatir)
    static const float CLICK_COOLDOWN_SECONDS = 0.35f;

    // Cuanto dura en pantalla cada tipo de paso de la pelea (en segundos)
    static const float COLUMN_START_DURATION = 0.45f;
    static const float HIT_DURATION = 0.32f;
    static const float BANNER_DURATION = 0.9f;
    static const float LOG_LINE_DURATION = 0.2f;
    static const float CLASH_ROUND_DURATION = 0.65f;
    static const float EVADE_ROLL_DURATION = 0.6f;

    // Monedas del clash: cuanto tiempo giran, que tan rapido y como se dibujan
    static const float COIN_SPIN_DURATION = 0.3f;
    static const float COIN_SPIN_SPEED = 40.0f;
    static const float COIN_RADIUS = 7.0f;
    static const float COIN_GAP = 4.0f;
    static const float COIN_STRIP_HEIGHT = 22.0f;
    static const float COIN_OUTLINE_THICKNESS = 2.0f;
    static const sf::Color COIN_STRIP_COLOR = sf::Color(0, 0, 0, 170);
    static const sf::Color COIN_HEADS_COLOR = sf::Color(230, 195, 90);
    static const sf::Color COIN_TAILS_COLOR = sf::Color(95, 90, 100);
    static const sf::Color COIN_BROKEN_COLOR = sf::Color(30, 28, 32);
    static const sf::Color COIN_OUTLINE_COLOR = sf::Color(60, 50, 40);
    static const sf::Color COIN_BREAKING_COLOR = sf::Color(230, 70, 60);

    // Animaciones
    static const float FLOATING_TEXT_DURATION = 0.9f;
    static const float FLOATING_TEXT_RISE_SPEED = 45.0f;
    static const float FLOATING_TEXT_OFFSET_Y = 18.0f;
    static const unsigned int FLOATING_TEXT_SIZE = 26;
    static const unsigned int CRITICAL_TEXT_SIZE = 32;
    static const float HP_BAR_ANIMATION_SPEED = 8.0f;
    static const float SHAKE_DURATION = 0.18f;
    static const float SHAKE_DISTANCE = 5.0f;
    static const float SHAKE_FREQUENCY = 90.0f;
    static const float ACTIVE_COLUMN_PADDING = 6.0f;
    static const unsigned int BANNER_TEXT_SIZE = 54;
    static const float BANNER_HEIGHT = 84.0f;
    static const float BANNER_CENTER_Y = 218.0f;
    static const sf::Color BANNER_BACKGROUND = sf::Color(0, 0, 0, 190);
    static const sf::Color ACTIVE_COLUMN_COLOR = sf::Color(220, 190, 120, 40);
    static const sf::Color DAMAGE_COLOR = sf::Color(255, 120, 100);
    static const sf::Color CRITICAL_DAMAGE_COLOR = sf::Color(255, 215, 90);

    // Valor de columna que significa "ninguna"
    static const int NO_COLUMN = -1;

    // Posicion que se usa cuando el mouse esta fuera de la ventana (no toca ningun boton)
    static const sf::Vector2f OFFSCREEN_MOUSE_POSITION = { -1000.0f, -1000.0f };

    // A partir de que porcentaje un clash se considera favorable o desfavorable
    static const float FAVORABLE_CHANCE = 0.6f;
    static const float UNFAVORABLE_CHANCE = 0.4f;
    static const float PERCENT_MULTIPLIER = 100.0f;

    // ---------- Tamanos de texto ----------

    static const unsigned int NAME_SIZE = 26;
    static const unsigned int HP_TEXT_SIZE = 16;
    static const unsigned int POISE_TEXT_SIZE = 15;
    static const unsigned int TURN_SIZE = 30;
    static const unsigned int STRENGTH_LABEL_SIZE = 16;
    static const unsigned int SLOT_NAME_SIZE = 16;
    static const unsigned int SLOT_SUMMARY_SIZE = 13;
    static const unsigned int VS_SIZE = 18;
    static const unsigned int HINT_SIZE = 15;
    static const unsigned int HAND_HEADER_SIZE = 15;
    static const unsigned int GAME_OVER_TITLE_SIZE = 48;
    static const unsigned int PAUSE_TITLE_SIZE = 40;
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

    // Color segun que tan buena es la chance: verde si es favorable, amarillo si es pareja y rojo si es desfavorable
    static sf::Color GetChanceColor(float winChance);

    // Palabra que acompana al porcentaje: "Favorable", "Parejo" o "Desfavorable"
    static std::string GetChanceVerdict(float winChance);

    // Pasa la chance (0 a 1) a un texto con porcentaje redondeado, por ejemplo "62%"
    static std::string FormatPercent(float winChance);

    // Dibuja el cuadrado de color, el nombre y la barra de vida de una unidad. Si isRightSide es true, se dibuja espejado a la derecha.
    // shownHp es la vida que se ve en la barra (baja de a poco durante la pelea).
    static void DrawUnitPanel(sf::RenderWindow& window, const sf::Font& font, const Unit& unit, bool isRightSide, float shownHp);

    Gameplay::Gameplay(const sf::Font& gameFont)
        : font(gameFont)
        , player(PLAYER_NAME, PLAYER_MAX_HP, PLAYER_COLOR)
        , enemy(ENEMY_NAME, ENEMY_MAX_HP, ENEMY_COLOR)
        , doubleSlash("Doble Corte - Explosión", "Doble Corte - Explosión", SkillType::Attack, 4, 2, 3, false)
        , tripleSlash("Triple Corte - Explosión", "Triple Corte - Explosión", SkillType::Attack, 3, 3, 3, false)
        , explosiveSlash("Tajo Destrozador Explosivo", "Tajo Destrozador Explosivo", SkillType::Attack, 4, 3, 3, true)
        , tangleCutter("Cortador de enredos", "Cortador de enredos", SkillType::Attack, 8, 1, 12, true)
        , tiantuiReload("Recargando espada tiantui", "Recargando espada tiantui", SkillType::Counter, 4, 2, 4, false)
        , enemyPattern(doubleSlash, tripleSlash, explosiveSlash, tangleCutter)
        , actionOptions()
        , actionChoices()
        , actionColumns()
        , playerActions()
        , winChances()
        , columnCoveredByEvade()
        , fightButton(gameFont, sf::FloatRect({ MARGIN, FIGHT_BUTTON_Y }, { SKILL_BUTTON_WIDTH, FIGHT_BUTTON_HEIGHT }), "¡Combatir!", "")
        , restartButton(gameFont, sf::FloatRect({ SCREEN_WIDTH / 2.0f - GAME_OVER_BUTTON_GAP / 2.0f - GAME_OVER_BUTTON_SIZE.x, SCREEN_HEIGHT / 2.0f + GAME_OVER_BUTTON_OFFSET_Y - GAME_OVER_BUTTON_SIZE.y / 2.0f }, GAME_OVER_BUTTON_SIZE), "Jugar de nuevo", "")
        , gameOverMenuButton(gameFont, sf::FloatRect({ SCREEN_WIDTH / 2.0f + GAME_OVER_BUTTON_GAP / 2.0f, SCREEN_HEIGHT / 2.0f + GAME_OVER_BUTTON_OFFSET_Y - GAME_OVER_BUTTON_SIZE.y / 2.0f }, GAME_OVER_BUTTON_SIZE), "Menú principal", "")
        , helpButton(gameFont, HELP_BUTTON_BOUNDS, "¿Cómo se juega?", "")
        , resumeButton(gameFont, sf::FloatRect({ (SCREEN_WIDTH - PAUSE_BUTTON_SIZE.x) / 2.0f, (SCREEN_HEIGHT - PAUSE_BOX_SIZE.y) / 2.0f + PAUSE_TITLE_HEIGHT }, PAUSE_BUTTON_SIZE), "Seguir", "")
        , pauseMenuButton(gameFont, sf::FloatRect({ (SCREEN_WIDTH - PAUSE_BUTTON_SIZE.x) / 2.0f, (SCREEN_HEIGHT - PAUSE_BOX_SIZE.y) / 2.0f + PAUSE_TITLE_HEIGHT + PAUSE_BUTTON_SIZE.y + PAUSE_BUTTON_GAP }, PAUSE_BUTTON_SIZE), "Volver al menú", "")
        , helpPanel(gameFont, { SCREEN_WIDTH, SCREEN_HEIGHT })
        , combatLog(gameFont, COMBAT_LOG_AREA)
        , state(GameState::ChoosingSkills)
        , isPaused(false)
        , returnToMenuRequested(false)
        , turn(1)
        , selectedSlot(0)
        , mousePosition({ 0.0f, 0.0f })
        , timeline(player, enemy)
        , nextEventIndex(0)
        , eventTimer(0.0f)
        , stateAfterPlayback(GameState::ChoosingSkills)
        , activeColumn(NO_COLUMN)
        , shownPlayerHp(static_cast<float>(PLAYER_MAX_HP))
        , shownEnemyHp(static_cast<float>(ENEMY_MAX_HP))
        , targetPlayerHp(static_cast<float>(PLAYER_MAX_HP))
        , targetEnemyHp(static_cast<float>(ENEMY_MAX_HP))
        , bannerText("")
        , bannerTimeLeft(0.0f)
        , shownClash()
        , isShowingClash(false)
        , coinSpinTimeLeft(0.0f)
        , shakeColumn(NO_COLUMN)
        , shakeIsPlayerRow(false)
        , shakeTimeLeft(0.0f)
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

        // Botones de la mano: por cada accion, las 2 opciones sorteadas y Esquivar a la derecha.
        // Los textos de las opciones se ponen en RefreshHandButtons, porque cambian cada turno.
        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            const float buttonY = SKILL_BUTTONS_TOP + static_cast<float>(actionIndex) * HAND_ROW_HEIGHT + HAND_HEADER_HEIGHT;

            for (int optionIndex = 0; optionIndex < OPTIONS_PER_ACTION; optionIndex++)
            {
                const float buttonX = MARGIN + static_cast<float>(optionIndex) * (OPTION_BUTTON_WIDTH + HAND_BUTTON_GAP);

                skillButtons.push_back(Button(gameFont, sf::FloatRect({ buttonX, buttonY }, { OPTION_BUTTON_WIDTH, HAND_BUTTON_HEIGHT }), "", " "));
            }

            const float evadeX = MARGIN + static_cast<float>(OPTIONS_PER_ACTION) * (OPTION_BUTTON_WIDTH + HAND_BUTTON_GAP);

            skillButtons.push_back(Button(gameFont, sf::FloatRect({ evadeX, buttonY }, { EVADE_BUTTON_WIDTH, HAND_BUTTON_HEIGHT }), "Esquivar", "Gasta la 1ra"));
        }

        // Tremor y Burn que inflige cada skill de Lei Heng, moneda por moneda (sacado de los datos del juego).
        // Lo de "sin Cracking" se aplica siempre porque todavia no existe el Cracking.
        // Sin Rondas de Marca de Tigre (las pasivas de Lei Heng).

        // Doble Corte - Explosion: poder final +1 cada 4 de (Burn + Tremor) del objetivo, maximo +4
        TargetStatusEffects doubleSlashEffects;
        doubleSlashEffects.statusPerFinalPower = 4;
        doubleSlashEffects.maxFinalPowerBonus = 4;
        doubleSlashEffects.perCoin.resize(2);
        doubleSlashEffects.perCoin[0].tremorPotency = 3;
        doubleSlashEffects.perCoin[1].burnPotency = 3;
        doubleSlashEffects.perCoin[1].burnCount = 2;
        doubleSlashEffects.perCoin[1].tremorPotency = 3;
        doubleSlashEffects.perCoin[1].tremorCount = 2;
        doubleSlash.SetTargetStatusEffects(doubleSlashEffects);

        // Triple Corte - Explosion: igual bonus; la 3ra moneda hace Tremor Burst 2 veces y saca 2 de cantidad
        TargetStatusEffects tripleSlashEffects;
        tripleSlashEffects.statusPerFinalPower = 4;
        tripleSlashEffects.maxFinalPowerBonus = 4;
        tripleSlashEffects.perCoin.resize(3);
        tripleSlashEffects.perCoin[0].tremorCount = 2;
        tripleSlashEffects.perCoin[1].burnPotency = 3;
        tripleSlashEffects.perCoin[1].burnCount = 2;
        tripleSlashEffects.perCoin[1].tremorPotency = 2;
        tripleSlashEffects.perCoin[1].tremorCount = 2;
        tripleSlashEffects.perCoin[2].burnPotency = 3;
        tripleSlashEffects.perCoin[2].burnCount = 2;
        tripleSlashEffects.perCoin[2].burstTimes = 2;
        tripleSlashEffects.perCoin[2].burstOnlyWithoutCracking = true;
        tripleSlashEffects.perCoin[2].countLossAfterBurst = 2;
        tripleSlash.SetTargetStatusEffects(tripleSlashEffects);

        // Tajo Destrozador Explosivo: poder de moneda +1 cada 6 de (Burn + Tremor) del objetivo, maximo +3
        TargetStatusEffects explosiveSlashEffects;
        explosiveSlashEffects.statusPerCoinPower = 6;
        explosiveSlashEffects.maxCoinPowerBonus = 3;
        explosiveSlashEffects.perCoin.resize(3);
        explosiveSlashEffects.perCoin[0].burnPotency = 3;
        explosiveSlashEffects.perCoin[0].tremorPotency = 3;
        explosiveSlashEffects.perCoin[0].tremorWithoutCrackingPotency = 4;
        explosiveSlashEffects.perCoin[0].tremorWithoutCrackingCount = 4;
        explosiveSlashEffects.perCoin[1].burnCount = 3;
        explosiveSlashEffects.perCoin[1].tremorCount = 3;
        explosiveSlashEffects.perCoin[1].convertsTremorToScorch = true;
        explosiveSlashEffects.perCoin[2].burnPotency = 3;
        explosiveSlashEffects.perCoin[2].burnCount = 3;
        explosiveSlashEffects.perCoin[2].burstTimes = 1;
        explosiveSlashEffects.perCoin[2].burstOnlyWithoutCracking = true;
        explosiveSlashEffects.perCoin[2].countLossAfterBurst = 1;
        explosiveSlash.SetTargetStatusEffects(explosiveSlashEffects);

        // Cortador de enredos: su unica moneda hace Tremor Burst
        TargetStatusEffects tangleCutterEffects;
        tangleCutterEffects.perCoin.resize(1);
        tangleCutterEffects.perCoin[0].convertsTremorToScorch = true;
        tangleCutterEffects.perCoin[0].burstTimes = 1;
        tangleCutterEffects.perCoin[0].burstOnlyWithoutCracking = true;
        tangleCutterEffects.perCoin[0].countLossAfterBurst = 1;
        tangleCutter.SetTargetStatusEffects(tangleCutterEffects);


        StartNewGame();
    }

    void Gameplay::HandleEvent(const sf::Event& event)
    {
        // Se guarda la posicion del mouse para resaltar el boton o el slot que esta debajo
        if (const sf::Event::MouseMoved* moved = event.getIf<sf::Event::MouseMoved>())
        {
            mousePosition = sf::Vector2f(moved->position);
        }
        else if (event.is<sf::Event::MouseLeft>())
        {
            // Si el mouse sale de la ventana, se lo manda lejos para que no quede nada resaltado
            mousePosition = OFFSCREEN_MOUSE_POSITION;
        }
        else if (const sf::Event::MouseButtonPressed* pressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (pressed->button == sf::Mouse::Button::Left)
            {
                HandleClick(sf::Vector2f(pressed->position));
            }
            else if (pressed->button == sf::Mouse::Button::Right)
            {
                HandleRightClick(sf::Vector2f(pressed->position));
            }
        }
        else if (const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                HandleEscape();
            }
        }
    }

    void Gameplay::Update(float deltaSeconds)
    {
        // En pausa no avanza nada (ni la pelea ni las animaciones)
        if (isPaused)
        {
            return;
        }

        // Pelea paso a paso: cuando se termina el tiempo de un paso, se muestra el siguiente
        if (state == GameState::PlayingTurn)
        {
            eventTimer -= deltaSeconds;

            while (state == GameState::PlayingTurn && eventTimer <= 0.0f)
            {
                const std::vector<CombatEvent>& events = timeline.GetEvents();

                if (nextEventIndex >= events.size())
                {
                    FinishPlayback();
                    break;
                }

                PlayEvent(events[nextEventIndex]);
                nextEventIndex++;
            }
        }

        // Las barras de vida bajan de a poco hasta la vida de ese momento de la pelea
        const float hpStep = deltaSeconds * HP_BAR_ANIMATION_SPEED;
        const float hpLerp = (hpStep > 1.0f) ? 1.0f : hpStep;

        shownPlayerHp += (targetPlayerHp - shownPlayerHp) * hpLerp;
        shownEnemyHp += (targetEnemyHp - shownEnemyHp) * hpLerp;

        // Los numeros flotantes suben y desaparecen
        for (FloatingText& floatingText : floatingTexts)
        {
            floatingText.timeLeft -= deltaSeconds;
            floatingText.position.y -= FLOATING_TEXT_RISE_SPEED * deltaSeconds;
        }

        for (size_t textIndex = floatingTexts.size(); textIndex > 0; textIndex--)
        {
            if (floatingTexts[textIndex - 1].timeLeft <= 0.0f)
            {
                floatingTexts.erase(floatingTexts.begin() + static_cast<std::ptrdiff_t>(textIndex - 1));
            }
        }

        if (bannerTimeLeft > 0.0f)
        {
            bannerTimeLeft -= deltaSeconds;
        }

        if (shakeTimeLeft > 0.0f)
        {
            shakeTimeLeft -= deltaSeconds;
        }

        if (coinSpinTimeLeft > 0.0f)
        {
            coinSpinTimeLeft -= deltaSeconds;
        }
    }

    void Gameplay::Draw(sf::RenderWindow& window) const
    {
        DrawUnitPanels(window);
        DrawActiveColumn(window);
        DrawSlots(window);
        DrawClashCoins(window);
        DrawFloatingTexts(window);
        DrawSkillButtons(window);
        fightButton.Draw(window, mousePosition);
        combatLog.Draw(window);
        helpButton.Draw(window, mousePosition);

        // Si el mouse esta sobre una skill, se muestra su detalle encima del registro
        if (state == GameState::ChoosingSkills && !helpPanel.IsOpen() && !isPaused)
        {
            DrawSkillTooltip(window);
        }

        DrawBanner(window);

        if (state == GameState::Victory || state == GameState::Defeat)
        {
            DrawGameOver(window);
        }

        if (isPaused)
        {
            DrawPause(window);
        }

        // La ayuda va arriba de todo
        helpPanel.Draw(window, mousePosition);
    }

    void Gameplay::StartNewGame()
    {
        isPaused = false;
        returnToMenuRequested = false;
        helpPanel.Close();

        player.Reset();
        enemy.Reset();
        turn = 1;
        state = GameState::ChoosingSkills;

        // Se limpian las animaciones y las barras vuelven a estar llenas
        timeline.Clear();
        nextEventIndex = 0;
        activeColumn = NO_COLUMN;
        floatingTexts.clear();
        bannerTimeLeft = 0.0f;
        shakeTimeLeft = 0.0f;
        shownPlayerHp = static_cast<float>(player.GetHp());
        shownEnemyHp = static_cast<float>(enemy.GetHp());
        targetPlayerHp = shownPlayerHp;
        targetEnemyHp = shownEnemyHp;
        fightButton.SetLabel("¡Combatir!");

        combatLog.Clear();
        combatLog.AddLine("Elegí una skill de cada acción de Yi Sang, ponela en la columna que quieras frenar y apretá ¡Combatir!");

        DealNewHand();
        PrepareTurn();
    }

    void Gameplay::PrepareTurn()
    {
        // Fuerza de Lei Heng este turno: 30% en el turno 1 y +10% por turno, hasta 100%
        float enemyStrength = ENEMY_STARTING_STRENGTH + ENEMY_STRENGTH_PER_TURN * static_cast<float>(turn - 1);

        if (enemyStrength > ENEMY_MAX_STRENGTH)
        {
            enemyStrength = ENEMY_MAX_STRENGTH;
        }

        enemy.SetDamageMultiplier(enemyStrength);

        // Una unidad staggereada no actua este turno
        if (enemy.IsStaggered())
        {
            enemyActions.clear();
        }
        else
        {
            enemyActions = enemyPattern.GetActionsForTurn(turn);
        }

        // Cada accion de Yi Sang arranca en su columna (accion 1 en la columna 1...) con su primera opcion.
        // Si esta staggereado, no pone ninguna.
        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            actionChoices[static_cast<size_t>(actionIndex)] = 0;
            actionColumns[static_cast<size_t>(actionIndex)] = player.IsStaggered() ? NO_COLUMN : actionIndex;
        }

        selectedSlot = 0;

        // Cambio el estado de las unidades (vida, Poise, SP...): las chances viejas ya no sirven
        winChanceCache.clear();

        RebuildPlayerActions();
        RefreshHandButtons();
        UpdateWinChances();
    }

    int Gameplay::DrawSkillOption() const
    {
        if (RollChance(SKILL_1_DRAW_CHANCE))
        {
            return SKILL_1;
        }

        // Si no salio S1, queda repartir el 40% restante entre S2 (30%) y S3 (10%)
        if (RollChance(SKILL_2_DRAW_CHANCE / (1.0f - SKILL_1_DRAW_CHANCE)))
        {
            return SKILL_2;
        }

        return SKILL_3;
    }

    void Gameplay::DealNewHand()
    {
        for (std::array<int, OPTIONS_PER_ACTION>& options : actionOptions)
        {
            for (int& option : options)
            {
                option = DrawSkillOption();
            }
        }
    }

    void Gameplay::SpendUsedOptions()
    {
        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            // Una accion que no se puso en ninguna columna no gasta nada
            if (actionColumns[static_cast<size_t>(actionIndex)] == NO_COLUMN)
            {
                continue;
            }

            const int choice = actionChoices[static_cast<size_t>(actionIndex)];

            // Esquivar gasta la skill que estaba primera en esa accion
            const int spentOption = (choice == OPTIONS_PER_ACTION) ? 0 : choice;

            actionOptions[static_cast<size_t>(actionIndex)][static_cast<size_t>(spentOption)] = DrawSkillOption();
        }
    }

    const Skill* Gameplay::GetActionSkill(int actionIndex) const
    {
        const int choice = actionChoices[static_cast<size_t>(actionIndex)];

        if (choice == OPTIONS_PER_ACTION)
        {
            return &playerSkills[SKILL_DEFENSE];
        }

        const int skillIndex = actionOptions[static_cast<size_t>(actionIndex)][static_cast<size_t>(choice)];

        return &playerSkills[static_cast<size_t>(skillIndex)];
    }

    void Gameplay::RebuildPlayerActions()
    {
        playerActions.fill(nullptr);

        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            const int column = actionColumns[static_cast<size_t>(actionIndex)];

            if (column != NO_COLUMN)
            {
                playerActions[static_cast<size_t>(column)] = GetActionSkill(actionIndex);
            }
        }
    }

    void Gameplay::AssignAction(int actionIndex, int choiceIndex)
    {
        // Si en esa columna habia otra accion, esa otra queda sin usar (se puede poner en otra columna)
        for (int otherAction = 0; otherAction < PLAYER_MAX_ACTIONS; otherAction++)
        {
            if (otherAction != actionIndex && actionColumns[static_cast<size_t>(otherAction)] == selectedSlot)
            {
                actionColumns[static_cast<size_t>(otherAction)] = NO_COLUMN;
            }
        }

        // Si esta accion estaba en otra columna, se mueve a la seleccionada
        actionColumns[static_cast<size_t>(actionIndex)] = selectedSlot;
        actionChoices[static_cast<size_t>(actionIndex)] = choiceIndex;

        RebuildPlayerActions();
        RefreshHandButtons();
        UpdateWinChances();
    }

    void Gameplay::RefreshHandButtons()
    {
        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            const bool isPlaced = actionColumns[static_cast<size_t>(actionIndex)] != NO_COLUMN;

            for (int choiceIndex = 0; choiceIndex < BUTTONS_PER_ACTION; choiceIndex++)
            {
                Button& button = skillButtons[static_cast<size_t>(actionIndex * BUTTONS_PER_ACTION + choiceIndex)];

                // Las opciones muestran la skill que salio sorteada; Esquivar siempre dice lo mismo
                if (choiceIndex < OPTIONS_PER_ACTION)
                {
                    const int skillIndex = actionOptions[static_cast<size_t>(actionIndex)][static_cast<size_t>(choiceIndex)];
                    const Skill& skill = playerSkills[static_cast<size_t>(skillIndex)];

                    button.SetLabel(skill.GetShortName());
                    button.SetSubLabel(skill.GetSummary());
                }

                button.SetSelected(isPlaced && actionChoices[static_cast<size_t>(actionIndex)] == choiceIndex);
                button.SetEnabled(!player.IsStaggered());
            }
        }
    }

    void Gameplay::UpdateWinChances()
    {
        // Se recorre en orden: si hay una esquiva antes, se asume que sigue activa y cubre los ataques que entran directo
        const Skill* pendingEvade = nullptr;

        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            const Skill* playerSkill = playerActions[static_cast<size_t>(slotIndex)];
            const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;

            if (playerSkill != nullptr && playerSkill->GetType() == SkillType::Evade)
            {
                pendingEvade = playerSkill;
            }

            const bool coveredByEvade = playerSkill == nullptr && pendingEvade != nullptr && enemySkill != nullptr && enemySkill->DealsDamageOnWin();
            columnCoveredByEvade[static_cast<size_t>(slotIndex)] = coveredByEvade;

            if (coveredByEvade)
            {
                playerSkill = pendingEvade;
            }

            // Solo hay clash si estan las dos skills y al menos una de las dos ataca
            const bool hasClash = playerSkill != nullptr && enemySkill != nullptr && !(playerSkill->IsDefensive() && enemySkill->IsDefensive());

            winChances[static_cast<size_t>(slotIndex)] = hasClash ? GetWinChance(*playerSkill, *enemySkill) : -1.0f;
        }

        // Si cambiaron las skills o el slot seleccionado, tambien cambian los numeros de los botones
        UpdateSkillButtonChances();
    }

    float Gameplay::GetWinChance(const Skill& playerSkill, const Skill& enemySkill)
    {
        for (const WinChanceCacheEntry& entry : winChanceCache)
        {
            if (entry.playerSkill == &playerSkill && entry.enemySkill == &enemySkill)
            {
                return entry.winChance;
            }
        }

        const float winChance = EstimateClashWinChance(player, playerSkill, enemy, enemySkill);
        winChanceCache.push_back({ &playerSkill, &enemySkill, winChance });

        return winChance;
    }

    void Gameplay::UpdateSkillButtonChances()
    {
        const Skill* enemySkill = (selectedSlot < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(selectedSlot)] : nullptr;

        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            for (int choiceIndex = 0; choiceIndex < BUTTONS_PER_ACTION; choiceIndex++)
            {
                Button& button = skillButtons[static_cast<size_t>(actionIndex * BUTTONS_PER_ACTION + choiceIndex)];

                // Skill que pondria este boton: la opcion sorteada o Esquivar
                const int skillIndex = (choiceIndex < OPTIONS_PER_ACTION) ? actionOptions[static_cast<size_t>(actionIndex)][static_cast<size_t>(choiceIndex)] : static_cast<int>(SKILL_DEFENSE);
                const Skill& playerSkill = playerSkills[static_cast<size_t>(skillIndex)];

                if (enemySkill == nullptr)
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
                    const float winChance = GetWinChance(playerSkill, *enemySkill);

                    button.SetRightText(FormatPercent(winChance), GetChanceColor(winChance));
                }
            }
        }
    }

    void Gameplay::ResolveTurn()
    {
        // La logica resuelve todo el turno de una y anota cada paso en la linea de tiempo;
        // despues la pantalla lo va mostrando de a poco (PlayingTurn)
        timeline.Clear();
        timeline.AddLogLine("-- Turno " + std::to_string(turn) + " --");

        // Esquiva de Yi Sang que sigue activa: cubre los ataques que le entran directo hasta que pierde o termina el turno
        const Skill* activeEvade = nullptr;

        // Cada accion de Yi Sang se enfrenta con la accion de Lei Heng de esa misma columna
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

            // Columna sin acciones de nadie: no hay nada que mostrar
            if (playerSkill == nullptr && enemySkill == nullptr)
            {
                continue;
            }

            timeline.StartColumn(slotIndex);

            // Si Yi Sang no tiene accion aca pero tiene una esquiva activa, la esquiva enfrenta este ataque
            const bool usesActiveEvade = playerSkill == nullptr && activeEvade != nullptr && !player.IsStaggered() && enemySkill->DealsDamageOnWin();

            if (usesActiveEvade)
            {
                timeline.AddLogLine("La esquiva de Yi Sang sigue activa y enfrenta este ataque.");
                playerSkill = activeEvade;
            }

            const bool evadeHeld = ResolveColumn(player, playerSkill, enemy, enemySkill, timeline);
            const bool usedEvade = playerSkill != nullptr && playerSkill->GetType() == SkillType::Evade;

            // La esquiva queda activa si aguanto; si la superaron, se pierde
            if (usedEvade)
            {
                activeEvade = evadeHeld ? playerSkill : nullptr;

                if (!evadeHeld && !player.IsDead())
                {
                    timeline.AddLogLine("La esquiva de Yi Sang se rompió.");
                }
            }

            timeline.EndColumn();
        }

        // Al final del turno quema el Burn (dano fijo); despues la cantidad de Poise y de Tremor baja 1
        // y los staggereados avanzan su recuperacion
        ApplyBurnAtTurnEnd(player);
        ApplyBurnAtTurnEnd(enemy);

        player.GetPoise().OnTurnEnd();
        enemy.GetPoise().OnTurnEnd();
        player.GetTremor().OnTurnEnd();
        enemy.GetTremor().OnTurnEnd();

        if (player.UpdateStaggerOnTurnEnd())
        {
            timeline.AddLogLine("Yi Sang se recupera del Stagger.");
        }

        if (enemy.UpdateStaggerOnTurnEnd())
        {
            timeline.AddLogLine("Lei Heng se recupera del Stagger.");
        }

        // Como queda la partida cuando termine de mostrarse el turno
        if (enemy.IsDead())
        {
            stateAfterPlayback = GameState::Victory;
            timeline.AddBanner("¡LEI HENG CAYÓ!");
            timeline.AddLogLine("¡Lei Heng cayó!");
        }
        else if (player.IsDead())
        {
            stateAfterPlayback = GameState::Defeat;
            timeline.AddBanner("YI SANG CAYÓ");
            timeline.AddLogLine("Yi Sang cayó...");
        }
        else
        {
            stateAfterPlayback = GameState::ChoosingSkills;
        }

        // Arranca a mostrarse la pelea
        state = GameState::PlayingTurn;
        nextEventIndex = 0;
        eventTimer = 0.0f;
        fightButton.SetLabel("Saltear");
    }

    int Gameplay::CountPlayerActions() const
    {
        int actionCount = 0;

        for (const Skill* action : playerActions)
        {
            if (action != nullptr)
            {
                actionCount++;
            }
        }

        return actionCount;
    }

    void Gameplay::PlayEvent(const CombatEvent& event)
    {
        // Todos los pasos traen la vida de los dos en ese momento: las barras van hacia ahi
        targetPlayerHp = static_cast<float>(event.playerHpAfter);
        targetEnemyHp = static_cast<float>(event.enemyHpAfter);

        switch (event.type)
        {
            case CombatEventType::ColumnStart:
            {
                activeColumn = event.column;
                isShowingClash = false;
                eventTimer += COLUMN_START_DURATION;
                break;
            }
            case CombatEventType::Hit:
            {
                // Numero de dano flotando sobre la tarjeta del que recibe el golpe, y la tarjeta se sacude
                FloatingText floatingText;
                floatingText.text = event.isCritical ? "-" + std::to_string(event.damage) + "!" : "-" + std::to_string(event.damage);
                floatingText.position = GetHitPosition(event);
                floatingText.color = event.isCritical ? CRITICAL_DAMAGE_COLOR : DAMAGE_COLOR;
                floatingText.timeLeft = FLOATING_TEXT_DURATION;
                floatingTexts.push_back(floatingText);

                shakeColumn = event.column;
                shakeIsPlayerRow = event.targetIsPlayer;
                shakeTimeLeft = SHAKE_DURATION;

                eventTimer += HIT_DURATION;
                break;
            }
            case CombatEventType::ClashRound:
            {
                // Las monedas giran un ratito y despues se ve como cayeron y quien gano la ronda
                shownClash = event;
                isShowingClash = true;
                coinSpinTimeLeft = COIN_SPIN_DURATION;
                eventTimer += CLASH_ROUND_DURATION;
                break;
            }
            case CombatEventType::EvadeRoll:
            {
                shownClash = event;
                isShowingClash = true;
                coinSpinTimeLeft = COIN_SPIN_DURATION;
                eventTimer += EVADE_ROLL_DURATION;
                break;
            }
            case CombatEventType::Banner:
            {
                bannerText = event.text;
                bannerTimeLeft = BANNER_DURATION;
                eventTimer += BANNER_DURATION;
                break;
            }
            case CombatEventType::LogLine:
            {
                combatLog.AddLine(event.text);
                eventTimer += LOG_LINE_DURATION;
                break;
            }
        }
    }

    void Gameplay::FinishPlayback()
    {
        // Si se apreto Saltear, se pasan al registro todas las lineas que faltaban
        const std::vector<CombatEvent>& events = timeline.GetEvents();

        for (size_t eventIndex = nextEventIndex; eventIndex < events.size(); eventIndex++)
        {
            if (events[eventIndex].type == CombatEventType::LogLine)
            {
                combatLog.AddLine(events[eventIndex].text);
            }
        }

        nextEventIndex = events.size();
        activeColumn = NO_COLUMN;
        isShowingClash = false;
        targetPlayerHp = static_cast<float>(player.GetHp());
        targetEnemyHp = static_cast<float>(enemy.GetHp());
        fightButton.SetLabel("¡Combatir!");

        state = stateAfterPlayback;

        if (state == GameState::ChoosingSkills)
        {
            // Las skills que se usaron este turno se gastan: en su lugar salen otras nuevas
            SpendUsedOptions();

            turn++;
            PrepareTurn();
        }
    }

    sf::Vector2f Gameplay::GetHitPosition(const CombatEvent& event) const
    {
        // Golpe fuera de las columnas (la quemada del final del turno): sale sobre la barra de vida
        if (event.column == NO_COLUMN)
        {
            const float barCenterX = event.targetIsPlayer ? MARGIN + NAME_OFFSET_X + HP_BAR_WIDTH / 2.0f : SCREEN_WIDTH - MARGIN - NAME_OFFSET_X - HP_BAR_WIDTH / 2.0f;

            return { barCenterX, PANEL_TOP + HP_BAR_OFFSET_Y + HP_BAR_HEIGHT + FLOATING_TEXT_OFFSET_Y };
        }

        const sf::FloatRect bounds = GetSlotBounds(event.column, event.targetIsPlayer);

        return { bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + FLOATING_TEXT_OFFSET_Y };
    }

    void Gameplay::ApplyBurnAtTurnEnd(Unit& unit)
    {
        if (unit.IsDead())
        {
            return;
        }

        const int burnDamage = unit.GetBurn().OnTurnEnd();

        if (burnDamage <= 0)
        {
            return;
        }

        unit.TakeDamage(burnDamage);
        timeline.AddHit(unit, burnDamage, false);
        timeline.AddLogLine(unit.GetName() + " se quema: " + std::to_string(burnDamage) + " de daño por Burn.");

        // La quemada tambien puede romper un umbral de Stagger
        if (unit.ConsumeStaggerEvent())
        {
            timeline.AddBanner("¡STAGGER!");
            timeline.AddLogLine("¡" + unit.GetName() + " queda staggereado por la quemadura! No actúa el próximo turno.");
        }
    }

    void Gameplay::HandleClick(sf::Vector2f clickPosition)
    {
        // Con la ayuda abierta, cualquier clic la cierra (y arranca la espera para el proximo clic)
        if (helpPanel.IsOpen())
        {
            helpPanel.Close();
            clickCooldownClock.restart();
            return;
        }

        if (isPaused)
        {
            HandlePauseClick(clickPosition);
            return;
        }

        // Si se acaba de cerrar un cartel, este clic es el segundo de un doble clic: se ignora
        if (clickCooldownClock.getElapsedTime() < sf::seconds(CLICK_COOLDOWN_SECONDS))
        {
            return;
        }

        // Mientras se muestra la pelea, el boton de Combatir funciona como "Saltear"
        if (state == GameState::PlayingTurn)
        {
            if (fightButton.Contains(clickPosition))
            {
                FinishPlayback();
            }

            return;
        }

        // Con la partida terminada, solo responden Jugar de nuevo y Menu principal
        if (state != GameState::ChoosingSkills)
        {
            if (restartButton.Contains(clickPosition))
            {
                StartNewGame();
                clickCooldownClock.restart();
            }
            else if (gameOverMenuButton.Contains(clickPosition))
            {
                returnToMenuRequested = true;
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
                helpPanel.Open();
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

        // Clic en una skill de la mano: esa accion pasa a la columna seleccionada con esa skill
        for (size_t buttonIndex = 0; buttonIndex < skillButtons.size(); buttonIndex++)
        {
            if (skillButtons[buttonIndex].IsEnabled() && skillButtons[buttonIndex].Contains(clickPosition))
            {
                const int actionIndex = static_cast<int>(buttonIndex) / BUTTONS_PER_ACTION;
                const int choiceIndex = static_cast<int>(buttonIndex) % BUTTONS_PER_ACTION;

                AssignAction(actionIndex, choiceIndex);
                return;
            }
        }

        if (fightButton.Contains(clickPosition))
        {
            ResolveTurn();
        }
        else if (helpButton.Contains(clickPosition))
        {
            helpPanel.Open();
        }
    }

    void Gameplay::HandleEscape()
    {
        if (helpPanel.IsOpen())
        {
            helpPanel.Close();
            return;
        }

        // Con la partida terminada ya hay un cartel con el boton para ir al menu
        if (state == GameState::Victory || state == GameState::Defeat)
        {
            return;
        }

        isPaused = !isPaused;
    }

    void Gameplay::HandlePauseClick(sf::Vector2f clickPosition)
    {
        if (resumeButton.Contains(clickPosition))
        {
            isPaused = false;
            clickCooldownClock.restart();
        }
        else if (pauseMenuButton.Contains(clickPosition))
        {
            isPaused = false;
            returnToMenuRequested = true;
        }
    }

    bool Gameplay::ConsumeReturnToMenuRequest()
    {
        const bool wasRequested = returnToMenuRequested;
        returnToMenuRequested = false;

        return wasRequested;
    }

    void Gameplay::HandleRightClick(sf::Vector2f clickPosition)
    {
        if (state != GameState::ChoosingSkills || helpPanel.IsOpen() || isPaused || player.IsStaggered())
        {
            return;
        }

        // Clic derecho en una columna de Yi Sang: se saca la accion y queda libre para ponerla en otra
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            if (GetSlotBounds(slotIndex, true).contains(clickPosition))
            {
                // La accion que estaba en esa columna queda sin usar (no gasta su skill)
                for (int& column : actionColumns)
                {
                    if (column == slotIndex)
                    {
                        column = NO_COLUMN;
                    }
                }

                selectedSlot = slotIndex;
                RebuildPlayerActions();
                RefreshHandButtons();
                UpdateWinChances();
                return;
            }
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
        DrawUnitPanel(window, font, player, false, shownPlayerHp);
        DrawUnitPanel(window, font, enemy, true, shownEnemyHp);

        // Numero de turno en el medio de la pantalla
        const sf::FloatRect turnArea({ 0.0f, TURN_LABEL_Y }, { SCREEN_WIDTH, static_cast<float>(TURN_SIZE) });
        DrawTextCentered(window, font, "Turno " + std::to_string(turn), TURN_SIZE, turnArea, GOLD_COLOR);

        // Abajo del turno, que tan fuerte esta pegando Lei Heng (en rojo cuando ya pega con todo)
        const float enemyStrength = enemy.GetDamageMultiplier();
        const std::string strengthText = "Fuerza de Lei Heng: " + FormatPercent(enemyStrength);
        const sf::FloatRect strengthArea({ 0.0f, STRENGTH_LABEL_Y }, { SCREEN_WIDTH, static_cast<float>(STRENGTH_LABEL_SIZE) });

        DrawTextCentered(window, font, strengthText, STRENGTH_LABEL_SIZE, strengthArea, (enemyStrength >= ENEMY_MAX_STRENGTH) ? UNFAVORABLE_COLOR : DIM_TEXT_COLOR);
    }

    void Gameplay::DrawSlots(sf::RenderWindow& window) const
    {
        for (int slotIndex = 0; slotIndex < SLOT_COUNT; slotIndex++)
        {
            // Fila de arriba: lo que hace Lei Heng (puede tener menos acciones que slots, por ejemplo en el turno del ataque fuerte)
            const Skill* enemySkill = (slotIndex < static_cast<int>(enemyActions.size())) ? enemyActions[static_cast<size_t>(slotIndex)] : nullptr;
            sf::FloatRect enemyBounds = GetSlotBounds(slotIndex, false);

            // La tarjeta que recibio un golpe se sacude un ratito
            const float shakeOffset = (shakeTimeLeft > 0.0f) ? std::sin(shakeTimeLeft * SHAKE_FREQUENCY) * SHAKE_DISTANCE : 0.0f;

            if (slotIndex == shakeColumn && !shakeIsPlayerRow)
            {
                enemyBounds.position.x += shakeOffset;
            }

            DrawSlotCard(window, enemyBounds, enemySkill, ENEMY_SLOT_COLOR, false, enemy.IsStaggered() ? "Staggereado" : "Sin acción");

            const sf::FloatRect previewArea({ enemyBounds.position.x, VS_ROW_Y }, { SLOT_WIDTH, VS_ROW_HEIGHT });

            // En la columna que se esta peleando se ve el poder de cada uno; en las demas, la chance
            if (isShowingClash && slotIndex == activeColumn)
            {
                DrawClashNumbers(window, previewArea);
            }
            else
            {
                DrawClashPreview(window, slotIndex, previewArea);
            }

            // Fila de abajo: lo que eligio el jugador para Yi Sang
            sf::FloatRect playerBounds = GetSlotBounds(slotIndex, true);

            if (slotIndex == shakeColumn && shakeIsPlayerRow)
            {
                playerBounds.position.x += shakeOffset;
            }

            const bool isHighlighted = (state == GameState::ChoosingSkills) && !player.IsStaggered() && (slotIndex == selectedSlot || playerBounds.contains(mousePosition));

            DrawSlotCard(window, playerBounds, playerActions[static_cast<size_t>(slotIndex)], PLAYER_SLOT_COLOR, isHighlighted, player.IsStaggered() ? "Staggereado" : "Libre");
        }

        if (state == GameState::PlayingTurn)
        {
            DrawTextAt(window, font, "Mirá cómo se resuelve el turno... (Saltear lo termina de una)", HINT_SIZE, { SLOTS_LEFT, HINT_Y }, DIM_TEXT_COLOR);
        }
        else if (player.IsStaggered())
        {
            DrawTextAt(window, font, "Yi Sang está staggereado: este turno no puede actuar. Apretá ¡Combatir! para seguir.", HINT_SIZE, { SLOTS_LEFT, HINT_Y }, STAGGER_COLOR);
        }
        else
        {
            const std::string hint = "Acciones: " + std::to_string(CountPlayerActions()) + "/" + std::to_string(PLAYER_MAX_ACTIONS) + "  ·  Clic en una columna y después en una skill de abajo  ·  Clic derecho la saca  ·  Sin acción = te pegan directo";

            DrawTextAt(window, font, hint, HINT_SIZE, { SLOTS_LEFT, HINT_Y }, DIM_TEXT_COLOR);
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

        // Columna que cubre la esquiva activa: se muestra la chance de esquivar todos los golpes
        if (columnCoveredByEvade[static_cast<size_t>(slotIndex)])
        {
            DrawTextCentered(window, font, "Esquiva: " + FormatPercent(winChance), VS_SIZE, area, GetChanceColor(winChance));
            return;
        }

        DrawTextCentered(window, font, FormatPercent(winChance) + " · " + GetChanceVerdict(winChance), VS_SIZE, area, GetChanceColor(winChance));
    }

    void Gameplay::DrawSkillButtons(sf::RenderWindow& window) const
    {
        // Arriba de cada fila: que accion es y en que columna esta puesta
        for (int actionIndex = 0; actionIndex < PLAYER_MAX_ACTIONS; actionIndex++)
        {
            const int column = actionColumns[static_cast<size_t>(actionIndex)];
            const float headerY = SKILL_BUTTONS_TOP + static_cast<float>(actionIndex) * HAND_ROW_HEIGHT;
            std::string header = "Acción " + std::to_string(actionIndex + 1);

            if (column != NO_COLUMN)
            {
                header += "  ·  en la columna " + std::to_string(column + 1);
            }
            else if (!player.IsStaggered())
            {
                header += "  ·  sin usar (guarda sus skills)";
            }

            DrawTextAt(window, font, header, HAND_HEADER_SIZE, { MARGIN, headerY }, column != NO_COLUMN ? GOLD_COLOR : DIM_TEXT_COLOR);
        }

        for (const Button& button : skillButtons)
        {
            button.Draw(window, mousePosition);
        }
    }

    void Gameplay::DrawSkillTooltip(sf::RenderWindow& window) const
    {
        // Skills de Yi Sang (botones de la lista)
        for (size_t buttonIndex = 0; buttonIndex < skillButtons.size(); buttonIndex++)
        {
            if (skillButtons[buttonIndex].Contains(mousePosition))
            {
                const size_t actionIndex = buttonIndex / static_cast<size_t>(BUTTONS_PER_ACTION);
                const size_t choiceIndex = buttonIndex % static_cast<size_t>(BUTTONS_PER_ACTION);
                const size_t skillIndex = (choiceIndex < static_cast<size_t>(OPTIONS_PER_ACTION)) ? static_cast<size_t>(actionOptions[actionIndex][choiceIndex]) : static_cast<size_t>(SKILL_DEFENSE);
                const std::string title = std::string(PLAYER_SKILL_TAGS[skillIndex]) + " · " + playerSkills[skillIndex].GetName();

                DrawSkillDetails(window, playerSkills[skillIndex], title);
                return;
            }
        }

        // Skills de Lei Heng (tarjetas de la fila de arriba), asi se sabe que te va a hacer
        for (int slotIndex = 0; slotIndex < static_cast<int>(enemyActions.size()) && slotIndex < SLOT_COUNT; slotIndex++)
        {
            const Skill* enemySkill = enemyActions[static_cast<size_t>(slotIndex)];

            if (enemySkill != nullptr && GetSlotBounds(slotIndex, false).contains(mousePosition))
            {
                DrawSkillDetails(window, *enemySkill, std::string(ENEMY_NAME) + " · " + enemySkill->GetName());
                return;
            }
        }
    }

    void Gameplay::DrawSkillDetails(sf::RenderWindow& window, const Skill& skill, const std::string& title) const
    {
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
    }

    void Gameplay::DrawActiveColumn(sf::RenderWindow& window) const
    {
        if (state != GameState::PlayingTurn || activeColumn == NO_COLUMN)
        {
            return;
        }

        // Rectangulo dorado detras de las dos tarjetas de la columna que se esta resolviendo
        const sf::FloatRect enemyBounds = GetSlotBounds(activeColumn, false);
        const float top = enemyBounds.position.y - ACTIVE_COLUMN_PADDING;
        const float bottom = PLAYER_ROW_Y + SLOT_HEIGHT + ACTIVE_COLUMN_PADDING;

        sf::RectangleShape highlight({ SLOT_WIDTH + ACTIVE_COLUMN_PADDING * 2.0f, bottom - top });
        highlight.setPosition({ enemyBounds.position.x - ACTIVE_COLUMN_PADDING, top });
        highlight.setFillColor(ACTIVE_COLUMN_COLOR);
        highlight.setOutlineColor(GOLD_COLOR);
        highlight.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(highlight);
    }

    void Gameplay::DrawFloatingTexts(sf::RenderWindow& window) const
    {
        for (const FloatingText& floatingText : floatingTexts)
        {
            // Se va haciendo transparente a medida que se le acaba el tiempo
            sf::Color color = floatingText.color;
            const float alpha = floatingText.timeLeft / FLOATING_TEXT_DURATION;
            color.a = static_cast<std::uint8_t>(255.0f * ((alpha > 1.0f) ? 1.0f : alpha));

            const bool isCritical = !floatingText.text.empty() && floatingText.text.back() == '!';
            const unsigned int size = isCritical ? CRITICAL_TEXT_SIZE : FLOATING_TEXT_SIZE;
            const sf::FloatRect area({ floatingText.position.x - SLOT_WIDTH / 2.0f, floatingText.position.y }, { SLOT_WIDTH, static_cast<float>(size) });

            DrawTextCentered(window, font, floatingText.text, size, area, color);
        }
    }

    void Gameplay::DrawBanner(sf::RenderWindow& window) const
    {
        if (bannerTimeLeft <= 0.0f)
        {
            return;
        }

        // Franja oscura con el texto grande en el medio de las columnas
        sf::RectangleShape band({ SCREEN_WIDTH, BANNER_HEIGHT });
        band.setPosition({ 0.0f, BANNER_CENTER_Y - BANNER_HEIGHT / 2.0f });
        band.setFillColor(BANNER_BACKGROUND);
        window.draw(band);

        const sf::FloatRect area({ 0.0f, BANNER_CENTER_Y - BANNER_HEIGHT / 2.0f }, { SCREEN_WIDTH, BANNER_HEIGHT });
        const bool isStagger = bannerText.find("STAGGER") != std::string::npos;

        DrawTextCentered(window, font, bannerText, BANNER_TEXT_SIZE, area, isStagger ? STAGGER_COLOR : GOLD_COLOR);
    }

    void Gameplay::DrawClashCoins(sf::RenderWindow& window) const
    {
        if (!isShowingClash || activeColumn == NO_COLUMN)
        {
            return;
        }

        const bool isRevealed = coinSpinTimeLeft <= 0.0f;
        const bool isEvade = shownClash.type == CombatEventType::EvadeRoll;

        // Despues de girar se sabe quien perdio la ronda: a ese se le rompe la ultima moneda
        const bool playerLosesRound = isRevealed && !isEvade && shownClash.enemyPower > shownClash.playerPower;
        const bool enemyLosesRound = isRevealed && !isEvade && shownClash.playerPower > shownClash.enemyPower;

        DrawCoinRow(window, GetSlotBounds(activeColumn, true), shownClash.playerCoinFaces, shownClash.playerTotalCoins, playerLosesRound);

        if (!isEvade)
        {
            DrawCoinRow(window, GetSlotBounds(activeColumn, false), shownClash.enemyCoinFaces, shownClash.enemyTotalCoins, enemyLosesRound);
        }
    }

    void Gameplay::DrawCoinRow(sf::RenderWindow& window, sf::FloatRect cardBounds, const std::vector<bool>& coinFaces, int totalCoins, bool breaksLastCoin) const
    {
        // Franja oscura abajo de la tarjeta para que las monedas se vean bien encima del texto
        const float stripY = cardBounds.position.y + cardBounds.size.y - COIN_STRIP_HEIGHT;
        sf::RectangleShape strip({ cardBounds.size.x, COIN_STRIP_HEIGHT });
        strip.setPosition({ cardBounds.position.x, stripY });
        strip.setFillColor(COIN_STRIP_COLOR);
        window.draw(strip);

        const bool isSpinning = coinSpinTimeLeft > 0.0f;
        const float centerY = stripY + COIN_STRIP_HEIGHT / 2.0f;
        const int flippedCoins = static_cast<int>(coinFaces.size());

        for (int coinIndex = 0; coinIndex < totalCoins; coinIndex++)
        {
            const float centerX = cardBounds.position.x + SLOT_TEXT_PADDING + COIN_RADIUS + static_cast<float>(coinIndex) * (COIN_RADIUS * 2.0f + COIN_GAP);

            sf::CircleShape coin(COIN_RADIUS);
            coin.setOrigin({ COIN_RADIUS, COIN_RADIUS });
            coin.setPosition({ centerX, centerY });
            coin.setOutlineThickness(COIN_OUTLINE_THICKNESS);
            coin.setOutlineColor(COIN_OUTLINE_COLOR);

            if (coinIndex >= flippedCoins)
            {
                // Moneda que ya se rompio en una ronda anterior
                coin.setFillColor(COIN_BROKEN_COLOR);
            }
            else if (isSpinning)
            {
                // Mientras gira se achica de costado y cambia de cara (como una moneda en el aire)
                const float spin = std::cos(coinSpinTimeLeft * COIN_SPIN_SPEED + static_cast<float>(coinIndex));
                coin.setScale({ std::fabs(spin), 1.0f });
                coin.setFillColor(spin > 0.0f ? COIN_HEADS_COLOR : COIN_TAILS_COLOR);
            }
            else
            {
                const bool isHeads = coinFaces[static_cast<size_t>(coinIndex)];
                coin.setFillColor(isHeads ? COIN_HEADS_COLOR : COIN_TAILS_COLOR);

                // La ultima moneda del que perdio la ronda es la que se rompe
                if (breaksLastCoin && coinIndex == flippedCoins - 1)
                {
                    coin.setOutlineColor(COIN_BREAKING_COLOR);
                }
            }

            window.draw(coin);
        }
    }

    void Gameplay::DrawClashNumbers(sf::RenderWindow& window, sf::FloatRect area) const
    {
        // Mientras giran las monedas todavia no se sabe el resultado
        if (coinSpinTimeLeft > 0.0f)
        {
            DrawTextCentered(window, font, "...", VS_SIZE, area, DIM_TEXT_COLOR);
            return;
        }

        if (shownClash.type == CombatEventType::EvadeRoll)
        {
            DrawTextCentered(window, font, "Esquiva: " + std::to_string(shownClash.playerPower), VS_SIZE, area, GOLD_COLOR);
            return;
        }

        // Gana la ronda el que saco mas poder: en dorado; el que perdio, en rojo; si empatan, los dos apagados
        sf::Color playerColor = DIM_TEXT_COLOR;
        sf::Color enemyColor = DIM_TEXT_COLOR;

        if (shownClash.playerPower > shownClash.enemyPower)
        {
            playerColor = GOLD_COLOR;
            enemyColor = UNFAVORABLE_COLOR;
        }
        else if (shownClash.enemyPower > shownClash.playerPower)
        {
            playerColor = UNFAVORABLE_COLOR;
            enemyColor = GOLD_COLOR;
        }

        const float thirdWidth = area.size.x / 3.0f;
        const sf::FloatRect playerArea(area.position, { thirdWidth, area.size.y });
        const sf::FloatRect middleArea({ area.position.x + thirdWidth, area.position.y }, { thirdWidth, area.size.y });
        const sf::FloatRect enemyArea({ area.position.x + thirdWidth * 2.0f, area.position.y }, { thirdWidth, area.size.y });

        DrawTextCentered(window, font, "YS " + std::to_string(shownClash.playerPower), VS_SIZE, playerArea, playerColor);
        DrawTextCentered(window, font, (shownClash.playerPower == shownClash.enemyPower) ? "empate" : "vs", VS_SIZE, middleArea, DIM_TEXT_COLOR);
        DrawTextCentered(window, font, std::to_string(shownClash.enemyPower) + " LH", VS_SIZE, enemyArea, enemyColor);
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
        gameOverMenuButton.Draw(window, mousePosition);
    }

    void Gameplay::DrawPause(sf::RenderWindow& window) const
    {
        sf::RectangleShape overlay({ SCREEN_WIDTH, SCREEN_HEIGHT });
        overlay.setFillColor(OVERLAY_COLOR);
        window.draw(overlay);

        const sf::Vector2f boxPosition = { (SCREEN_WIDTH - PAUSE_BOX_SIZE.x) / 2.0f, (SCREEN_HEIGHT - PAUSE_BOX_SIZE.y) / 2.0f };

        sf::RectangleShape box(PAUSE_BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(GAME_OVER_BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(SLOT_BORDER_THICKNESS);
        window.draw(box);

        DrawTextCentered(window, font, "Pausa", PAUSE_TITLE_SIZE, sf::FloatRect(boxPosition, { PAUSE_BOX_SIZE.x, PAUSE_TITLE_HEIGHT }), GOLD_COLOR);
        resumeButton.Draw(window, mousePosition);
        pauseMenuButton.Draw(window, mousePosition);
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

    static void DrawUnitPanel(sf::RenderWindow& window, const sf::Font& font, const Unit& unit, bool isRightSide, float shownHp)
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

        const float shownRatio = shownHp / static_cast<float>(unit.GetMaxHp());

        sf::RectangleShape barFill({ HP_BAR_WIDTH * shownRatio, HP_BAR_HEIGHT });
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

        const std::string hpText = std::to_string(static_cast<int>(std::round(shownHp))) + " / " + std::to_string(unit.GetMaxHp());
        DrawTextCentered(window, font, hpText, HP_TEXT_SIZE, sf::FloatRect({ barX, barY }, { HP_BAR_WIDTH, HP_BAR_HEIGHT }), TEXT_COLOR);

        // Abajo de la barra se muestra la cordura y los estados que tenga (Poise y Sinking), en una sola linea
        const Poise& poise = unit.GetPoise();
        const Sinking& sinking = unit.GetSinking();
        const std::string sanitySign = (unit.GetSanity() > 0) ? "+" : "";
        std::string statusText = unit.IsStaggered() ? "STAGGER   SP " + sanitySign : "SP " + sanitySign + std::to_string(unit.GetSanity()) + " (" + FormatPercent(unit.GetHeadsChance()) + " cara)";

        if (poise.IsActive())
        {
            statusText += "   Poise " + std::to_string(poise.GetPotency()) + "·" + std::to_string(poise.GetCount()) + " (" + FormatPercent(poise.GetCriticalChance()) + " crít.)";
        }

        if (sinking.IsActive())
        {
            statusText += "   Sinking " + std::to_string(sinking.GetPotency()) + "·" + std::to_string(sinking.GetCount());
        }

        const Burn& burn = unit.GetBurn();

        if (burn.IsActive())
        {
            statusText += "   Burn " + std::to_string(burn.GetPotency()) + "·" + std::to_string(burn.GetCount());
        }

        const Tremor& tremor = unit.GetTremor();

        if (tremor.IsActive())
        {
            statusText += std::string(tremor.IsScorch() ? "   Tremor-Scorch " : "   Tremor ") + std::to_string(tremor.GetPotency()) + "·" + std::to_string(tremor.GetCount());
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
