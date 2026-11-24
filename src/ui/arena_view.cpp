#include "ui/arena_view.h"

#include <cmath>
#include <cstdint>
#include <initializer_list>

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // ---------- Lugar de cada figura ----------

    // Distancia del borde de la arena a donde se para cada figura, y altura del piso desde abajo
    static const float HOME_MARGIN_X = 150.0f;
    static const float FLOOR_MARGIN_BOTTOM = 46.0f;

    // Al chocar, cada uno queda a esta distancia del centro
    static const float CLASH_GAP = 34.0f;

    // Cuanto retrocede el que pierde una ronda de clash (y cada uno si empatan)
    static const float CLASH_RECOIL = 46.0f;
    static const float TIE_RECOIL = 18.0f;

    // Golpe: el que pega se pasa un poco del centro y el que recibe sale despedido para atras
    static const float LUNGE_EXTRA = 18.0f;
    static const float HIT_KNOCKBACK = 58.0f;

    // Esquiva: salto hacia atras
    static const float EVADE_BACKSTEP = 70.0f;
    static const float JUMP_DURATION = 0.45f;
    static const float JUMP_HEIGHT = 38.0f;

    // Que tan rapido llega cada figura a donde va (mas alto = mas rapido)
    static const float MOVE_SPEED = 14.0f;

    // Despues del ultimo golpe, cuanto tarda en volver a su lugar
    static const float RETURN_DELAY = 0.45f;

    // ---------- Efectos ----------

    static const float HIT_FLASH_DURATION = 0.15f;
    static const float VIBRATE_DURATION = 0.6f;
    static const float VIBRATE_FREQUENCY = 70.0f;
    static const float VIBRATE_AMPLITUDE = 4.0f;
    static const float STAGGER_TILT_DEGREES = 24.0f;
    static const float DEFEATED_TILT_DEGREES = 80.0f;
    static const float SPARK_DURATION = 0.28f;
    static const float SPARK_MIN_RADIUS = 8.0f;
    static const float SPARK_MAX_RADIUS = 38.0f;
    static const float SPARK_HEIGHT = 62.0f;
    static const float PI = 3.14159265f;
    static const float FULL_ALPHA = 255.0f;
    static const std::uint8_t DEFEATED_ALPHA = 90;

    // Tamano de Lei Heng comparado con Yi Sang
    static const float ENEMY_SCALE = 1.3f;

    // ---------- Forma de la figura (con los pies en 0,0; para arriba es negativo) ----------

    static const sf::Vector2f LEG_SIZE = { 8.0f, 30.0f };
    static const float LEG_SEPARATION = 10.0f;
    static const sf::Vector2f TORSO_SIZE = { 26.0f, 44.0f };
    static const float HEAD_RADIUS = 13.0f;
    static const float HEAD_CENTER_Y = -88.0f;
    static const sf::Vector2f WEAPON_SIZE = { 5.0f, 62.0f };
    static const sf::Vector2f WEAPON_HAND = { 14.0f, -58.0f };
    static const float WEAPON_ANGLE_DEGREES = 55.0f;
    static const float FIGURE_HEIGHT = 104.0f;

    // ---------- Textos ----------

    static const unsigned int NAME_SIZE = 16;
    static const unsigned int SKILL_SIZE = 15;
    static const float LABEL_WIDTH = 280.0f;
    static const float NAME_OFFSET_Y = 8.0f;
    static const float SKILL_OFFSET_Y = 26.0f;
    static const float HEAD_TEXT_OFFSET_Y = 14.0f;

    // ---------- Colores ----------

    // Un poco mas claro que el resto de la pantalla, para que se vea el traje negro de Yi Sang
    static const sf::Color BACKGROUND_COLOR = sf::Color(58, 52, 62);
    static const sf::Color BORDER_COLOR = sf::Color(90, 75, 60);
    static const sf::Color FLOOR_COLOR = sf::Color(70, 60, 50);
    static const sf::Color WEAPON_COLOR = sf::Color(205, 205, 215);
    static const sf::Color FLASH_COLOR = sf::Color(255, 255, 255);
    static const sf::Color SCORCH_COLOR = sf::Color(240, 140, 50);
    static const sf::Color SPARK_COLOR = sf::Color(240, 210, 120);
    static const sf::Color NAME_COLOR = sf::Color(235, 230, 220);
    static const sf::Color SKILL_COLOR = sf::Color(220, 190, 120);
    static const float BORDER_THICKNESS = 2.0f;
    static const float FLOOR_THICKNESS = 2.0f;
    static const float HEAD_LIGHTEN = 40.0f;

    // ---------- Sprites ----------

    // Carpeta y final del nombre de cada pose (por ejemplo res/sprites/yi_sang_attack.png)
    static const char* SPRITES_FOLDER = "res/sprites/";
    static const char* POSE_SUFFIXES[FIGHTER_POSE_COUNT] = { "_idle.png", "_attack.png", "_hurt.png" };

    // Cuanto dura cada pose despues del momento que la dispara
    static const float ATTACK_POSE_DURATION = 0.5f;
    static const float HURT_POSE_DURATION = 0.4f;

    // Tintes del sprite: rojizo al recibir un golpe, naranja con Scorch
    static const sf::Color SPRITE_NORMAL_COLOR = sf::Color(255, 255, 255);
    static const sf::Color SPRITE_HIT_COLOR = sf::Color(255, 130, 130);

    // Mezcla dos colores: amount 0 = el primero, 1 = el segundo
    static sf::Color MixColors(sf::Color from, sf::Color to, float amount);

    // Mueve value hacia target de a poco (mas rapido cuanto mas lejos)
    static float Approach(float value, float target, float deltaSeconds);

    ArenaView::ArenaView(const sf::Font& arenaFont, sf::FloatRect arenaBounds, sf::Color playerColor, sf::Color enemyColor)
        : font(arenaFont)
        , bounds(arenaBounds)
        , playerFighter()
        , enemyFighter()
        , sparkTimeLeft(0.0f)
        , elapsedSeconds(0.0f)
    {
        playerFighter.name = "Yi Sang";
        playerFighter.color = playerColor;
        playerFighter.homeX = bounds.position.x + HOME_MARGIN_X;
        playerFighter.facing = 1.0f;

        enemyFighter.name = "Lei Heng";
        enemyFighter.color = enemyColor;
        enemyFighter.homeX = bounds.position.x + bounds.size.x - HOME_MARGIN_X;
        enemyFighter.facing = -1.0f;
        enemyFighter.scale = ENEMY_SCALE;

        LoadSprites(playerSprites, "yi_sang");
        LoadSprites(enemySprites, "lei_heng");
    }

    void ArenaView::LoadSprites(FighterSprites& sprites, const std::string& fileName)
    {
        for (int poseIndex = 0; poseIndex < FIGHTER_POSE_COUNT; poseIndex++)
        {
            const std::string path = std::string(SPRITES_FOLDER) + fileName + POSE_SUFFIXES[poseIndex];
            sprites.isLoaded[static_cast<size_t>(poseIndex)] = sprites.textures[static_cast<size_t>(poseIndex)].loadFromFile(path);
        }
    }

    FighterPose ArenaView::GetCurrentPose(const ArenaFighter& fighter)
    {
        if (fighter.isDefeated || fighter.isStaggered || fighter.hurtPoseTimeLeft > 0.0f)
        {
            return FighterPose::Hurt;
        }

        if (fighter.attackPoseTimeLeft > 0.0f)
        {
            return FighterPose::Attack;
        }

        return FighterPose::Idle;
    }

    void ArenaView::Reset(bool playerIsStaggered, bool enemyIsStaggered)
    {
        for (ArenaFighter* fighter : { &playerFighter, &enemyFighter })
        {
            fighter->offsetX = 0.0f;
            fighter->targetOffsetX = 0.0f;
            fighter->jumpTimeLeft = 0.0f;
            fighter->hitFlashTimeLeft = 0.0f;
            fighter->vibrateTimeLeft = 0.0f;
            fighter->returnTimeLeft = 0.0f;
            fighter->attackPoseTimeLeft = 0.0f;
            fighter->hurtPoseTimeLeft = 0.0f;
            fighter->isDefeated = false;
            fighter->skillName.clear();
        }

        playerFighter.isStaggered = playerIsStaggered;
        enemyFighter.isStaggered = enemyIsStaggered;
        sparkTimeLeft = 0.0f;
    }

    void ArenaView::OnColumnStart(const std::string& playerSkillName, const std::string& enemySkillName)
    {
        playerFighter.targetOffsetX = 0.0f;
        enemyFighter.targetOffsetX = 0.0f;
        playerFighter.skillName = playerSkillName;
        enemyFighter.skillName = enemySkillName;
    }

    void ArenaView::OnClashRound()
    {
        // Los dos se tiran hacia el medio
        playerFighter.targetOffsetX = GetClashOffset(playerFighter);
        enemyFighter.targetOffsetX = GetClashOffset(enemyFighter);
        playerFighter.returnTimeLeft = 0.0f;
        enemyFighter.returnTimeLeft = 0.0f;
        playerFighter.attackPoseTimeLeft = ATTACK_POSE_DURATION;
        enemyFighter.attackPoseTimeLeft = ATTACK_POSE_DURATION;
    }

    void ArenaView::OnClashRoundResult(bool playerLostRound, bool isTie)
    {
        sparkTimeLeft = SPARK_DURATION;

        if (isTie)
        {
            playerFighter.targetOffsetX = GetClashOffset(playerFighter) - playerFighter.facing * TIE_RECOIL;
            enemyFighter.targetOffsetX = GetClashOffset(enemyFighter) - enemyFighter.facing * TIE_RECOIL;
            return;
        }

        // El que perdio la ronda retrocede (y pone cara de golpeado); el que gano se queda en el medio atacando
        ArenaFighter& loser = playerLostRound ? playerFighter : enemyFighter;
        ArenaFighter& winner = playerLostRound ? enemyFighter : playerFighter;
        loser.targetOffsetX = GetClashOffset(loser) - loser.facing * CLASH_RECOIL;
        loser.attackPoseTimeLeft = 0.0f;
        loser.hurtPoseTimeLeft = HURT_POSE_DURATION;
        winner.attackPoseTimeLeft = ATTACK_POSE_DURATION;
    }

    void ArenaView::OnHit(bool targetIsPlayer, bool targetIsDefeated)
    {
        ArenaFighter& attacker = targetIsPlayer ? enemyFighter : playerFighter;
        ArenaFighter& target = targetIsPlayer ? playerFighter : enemyFighter;

        // Golpe fuera de las columnas (la quemada del final del turno): solo destella
        target.hitFlashTimeLeft = HIT_FLASH_DURATION;
        target.hurtPoseTimeLeft = HURT_POSE_DURATION;
        target.attackPoseTimeLeft = 0.0f;
        target.isDefeated = targetIsDefeated;

        if (attacker.skillName.empty())
        {
            return;
        }

        attacker.attackPoseTimeLeft = ATTACK_POSE_DURATION;
        attacker.targetOffsetX = GetClashOffset(attacker) + attacker.facing * LUNGE_EXTRA;
        target.targetOffsetX = GetClashOffset(target) - target.facing * HIT_KNOCKBACK;
        attacker.returnTimeLeft = RETURN_DELAY;
        target.returnTimeLeft = RETURN_DELAY;
    }

    void ArenaView::OnEvade()
    {
        playerFighter.jumpTimeLeft = JUMP_DURATION;
        playerFighter.targetOffsetX = -playerFighter.facing * EVADE_BACKSTEP;
        playerFighter.returnTimeLeft = RETURN_DELAY + JUMP_DURATION;
    }

    void ArenaView::OnMiss(bool targetIsPlayer)
    {
        ArenaFighter& attacker = targetIsPlayer ? enemyFighter : playerFighter;
        ArenaFighter& target = targetIsPlayer ? playerFighter : enemyFighter;

        attacker.attackPoseTimeLeft = ATTACK_POSE_DURATION;
        attacker.targetOffsetX = GetClashOffset(attacker) + attacker.facing * LUNGE_EXTRA;
        attacker.returnTimeLeft = RETURN_DELAY;

        // El que esquiva salta para atras y no cambia de pose
        target.jumpTimeLeft = JUMP_DURATION;
        target.targetOffsetX = -target.facing * EVADE_BACKSTEP;
        target.returnTimeLeft = RETURN_DELAY + JUMP_DURATION;
    }

    void ArenaView::OnTremorBurst(bool targetIsPlayer, bool isScorch)
    {
        ArenaFighter& target = targetIsPlayer ? playerFighter : enemyFighter;
        target.vibrateTimeLeft = VIBRATE_DURATION;
        target.isScorchVibration = isScorch;
    }

    void ArenaView::OnStagger(bool targetIsPlayer)
    {
        ArenaFighter& target = targetIsPlayer ? playerFighter : enemyFighter;
        target.isStaggered = true;
    }

    void ArenaView::Update(float deltaSeconds)
    {
        elapsedSeconds += deltaSeconds;

        if (sparkTimeLeft > 0.0f)
        {
            sparkTimeLeft -= deltaSeconds;
        }

        for (ArenaFighter* fighter : { &playerFighter, &enemyFighter })
        {
            // Pasado un rato del ultimo golpe, vuelve a su lugar
            if (fighter->returnTimeLeft > 0.0f)
            {
                fighter->returnTimeLeft -= deltaSeconds;

                if (fighter->returnTimeLeft <= 0.0f)
                {
                    fighter->targetOffsetX = 0.0f;
                }
            }

            fighter->offsetX = Approach(fighter->offsetX, fighter->targetOffsetX, deltaSeconds);

            if (fighter->jumpTimeLeft > 0.0f)
            {
                fighter->jumpTimeLeft -= deltaSeconds;
            }

            if (fighter->hitFlashTimeLeft > 0.0f)
            {
                fighter->hitFlashTimeLeft -= deltaSeconds;
            }

            if (fighter->vibrateTimeLeft > 0.0f)
            {
                fighter->vibrateTimeLeft -= deltaSeconds;
            }

            if (fighter->attackPoseTimeLeft > 0.0f)
            {
                fighter->attackPoseTimeLeft -= deltaSeconds;
            }

            if (fighter->hurtPoseTimeLeft > 0.0f)
            {
                fighter->hurtPoseTimeLeft -= deltaSeconds;
            }
        }
    }

    void ArenaView::Draw(sf::RenderWindow& window) const
    {
        // Fondo y piso
        sf::RectangleShape background(bounds.size);
        background.setPosition(bounds.position);
        background.setFillColor(BACKGROUND_COLOR);
        background.setOutlineColor(BORDER_COLOR);
        background.setOutlineThickness(BORDER_THICKNESS);
        window.draw(background);

        const float floorY = bounds.position.y + bounds.size.y - FLOOR_MARGIN_BOTTOM;

        sf::RectangleShape floor({ bounds.size.x, FLOOR_THICKNESS });
        floor.setPosition({ bounds.position.x, floorY });
        floor.setFillColor(FLOOR_COLOR);
        window.draw(floor);

        DrawFighter(window, playerFighter, playerSprites);
        DrawFighter(window, enemyFighter, enemySprites);

        // Chispa del choque: un circulo que crece y se desvanece en el medio
        if (sparkTimeLeft > 0.0f)
        {
            const float progress = 1.0f - sparkTimeLeft / SPARK_DURATION;
            const float radius = SPARK_MIN_RADIUS + (SPARK_MAX_RADIUS - SPARK_MIN_RADIUS) * progress;

            sf::Color sparkColor = SPARK_COLOR;
            sparkColor.a = static_cast<std::uint8_t>(FULL_ALPHA * (1.0f - progress));

            sf::CircleShape spark(radius);
            spark.setOrigin({ radius, radius });
            spark.setPosition({ bounds.position.x + bounds.size.x / 2.0f, floorY - SPARK_HEIGHT });
            spark.setFillColor(sparkColor);
            window.draw(spark);
        }
    }

    sf::Vector2f ArenaView::GetHeadPosition(bool isPlayer) const
    {
        const ArenaFighter& fighter = isPlayer ? playerFighter : enemyFighter;
        const sf::Vector2f feet = GetFeetPosition(fighter);

        return { feet.x, feet.y - GetFigureHeight(fighter, isPlayer ? playerSprites : enemySprites) - HEAD_TEXT_OFFSET_Y };
    }

    float ArenaView::GetFigureHeight(const ArenaFighter& fighter, const FighterSprites& sprites) const
    {
        const size_t idleIndex = static_cast<size_t>(FighterPose::Idle);

        if (sprites.isLoaded[idleIndex])
        {
            return static_cast<float>(sprites.textures[idleIndex].getSize().y);
        }

        return FIGURE_HEIGHT * fighter.scale;
    }

    void ArenaView::DrawFighter(sf::RenderWindow& window, const ArenaFighter& fighter, const FighterSprites& sprites) const
    {
        const sf::Vector2f feet = GetFeetPosition(fighter);

        // Inclinacion: hacia atras si esta staggereado, casi acostado si perdio
        float tiltDegrees = 0.0f;

        if (fighter.isDefeated)
        {
            tiltDegrees = -DEFEATED_TILT_DEGREES * fighter.facing;
        }
        else if (fighter.isStaggered)
        {
            tiltDegrees = -STAGGER_TILT_DEGREES * fighter.facing;
        }

        // Con sprites se usa la imagen de la pose; si no se pudieron cargar, las formas simples
        if (sprites.isLoaded[static_cast<size_t>(FighterPose::Idle)])
        {
            DrawFighterSprite(window, fighter, sprites, feet, tiltDegrees);
        }
        else
        {
            DrawFighterShapes(window, fighter, feet, tiltDegrees);
        }

        DrawFighterLabels(window, fighter, GetFigureHeight(fighter, sprites));
    }

    void ArenaView::DrawFighterSprite(sf::RenderWindow& window, const ArenaFighter& fighter, const FighterSprites& sprites, sf::Vector2f feet, float tiltDegrees) const
    {
        // Si falta la imagen de la pose, se usa la de quieto
        size_t poseIndex = static_cast<size_t>(GetCurrentPose(fighter));

        if (!sprites.isLoaded[poseIndex])
        {
            poseIndex = static_cast<size_t>(FighterPose::Idle);
        }

        const sf::Texture& texture = sprites.textures[poseIndex];
        const sf::Vector2f textureSize = sf::Vector2f(texture.getSize());

        // Los sprites miran a la derecha y tienen los pies en el centro de abajo:
        // se apoyan ahi, se dan vuelta si la figura mira a la izquierda y se inclinan
        sf::Sprite sprite(texture);
        sprite.setOrigin({ textureSize.x / 2.0f, textureSize.y });
        sprite.setPosition(feet);
        sprite.setRotation(sf::degrees(tiltDegrees));
        sprite.setScale({ fighter.facing, 1.0f });

        // Tinte: rojizo al recibir un golpe, naranja si vibra por Scorch, transparente si perdio
        sf::Color tint = SPRITE_NORMAL_COLOR;

        if (fighter.hitFlashTimeLeft > 0.0f)
        {
            tint = SPRITE_HIT_COLOR;
        }
        else if (fighter.vibrateTimeLeft > 0.0f && fighter.isScorchVibration)
        {
            tint = SCORCH_COLOR;
        }

        if (fighter.isDefeated)
        {
            tint.a = DEFEATED_ALPHA;
        }

        sprite.setColor(tint);
        window.draw(sprite);
    }

    void ArenaView::DrawFighterShapes(sf::RenderWindow& window, const ArenaFighter& fighter, sf::Vector2f feet, float tiltDegrees) const
    {
        // Todas las partes se arman con los pies en 0,0 mirando a la derecha; la transformacion las ubica,
        // las da vuelta si mira a la izquierda, las inclina y las agranda
        sf::Transform transform;
        transform.translate(feet);
        transform.rotate(sf::degrees(tiltDegrees));
        transform.scale({ fighter.facing * fighter.scale, fighter.scale });

        sf::RenderStates states;
        states.transform = transform;

        // Color del cuerpo: blanco un instante al recibir un golpe, naranja si vibra por Scorch, transparente si perdio
        sf::Color bodyColor = fighter.color;

        if (fighter.hitFlashTimeLeft > 0.0f)
        {
            bodyColor = MixColors(bodyColor, FLASH_COLOR, fighter.hitFlashTimeLeft / HIT_FLASH_DURATION);
        }
        else if (fighter.vibrateTimeLeft > 0.0f && fighter.isScorchVibration)
        {
            bodyColor = SCORCH_COLOR;
        }

        sf::Color headColor = MixColors(bodyColor, FLASH_COLOR, HEAD_LIGHTEN / FULL_ALPHA);
        sf::Color weaponColor = WEAPON_COLOR;

        if (fighter.isDefeated)
        {
            bodyColor.a = DEFEATED_ALPHA;
            headColor.a = DEFEATED_ALPHA;
            weaponColor.a = DEFEATED_ALPHA;
        }

        // Piernas
        for (const float legX : { -LEG_SEPARATION, 0.0f })
        {
            sf::RectangleShape leg(LEG_SIZE);
            leg.setPosition({ legX, -LEG_SIZE.y });
            leg.setFillColor(bodyColor);
            window.draw(leg, states);
        }

        // Torso
        sf::RectangleShape torso(TORSO_SIZE);
        torso.setPosition({ -TORSO_SIZE.x / 2.0f, -LEG_SIZE.y - TORSO_SIZE.y });
        torso.setFillColor(bodyColor);
        window.draw(torso, states);

        // Cabeza
        sf::CircleShape head(HEAD_RADIUS);
        head.setOrigin({ HEAD_RADIUS, HEAD_RADIUS });
        head.setPosition({ 0.0f, HEAD_CENTER_Y });
        head.setFillColor(headColor);
        window.draw(head, states);

        // Arma: una hoja larga que sale de la mano, apuntando hacia adelante
        sf::RectangleShape weapon(WEAPON_SIZE);
        weapon.setOrigin({ WEAPON_SIZE.x / 2.0f, WEAPON_SIZE.y });
        weapon.setPosition(WEAPON_HAND);
        weapon.setRotation(sf::degrees(WEAPON_ANGLE_DEGREES));
        weapon.setFillColor(weaponColor);
        window.draw(weapon, states);

    }

    void ArenaView::DrawFighterLabels(sf::RenderWindow& window, const ArenaFighter& fighter, float figureHeight) const
    {
        // Nombre abajo de los pies (sin inclinar) y skill que usa arriba de la cabeza
        const float homeFeetY = bounds.position.y + bounds.size.y - FLOOR_MARGIN_BOTTOM;
        const sf::FloatRect nameArea({ fighter.homeX - LABEL_WIDTH / 2.0f, homeFeetY + NAME_OFFSET_Y }, { LABEL_WIDTH, static_cast<float>(NAME_SIZE) });
        DrawTextCentered(window, font, fighter.name, NAME_SIZE, nameArea, NAME_COLOR);

        if (!fighter.skillName.empty())
        {
            const float skillY = homeFeetY - figureHeight - SKILL_OFFSET_Y;
            const sf::FloatRect skillArea({ fighter.homeX - LABEL_WIDTH / 2.0f, skillY }, { LABEL_WIDTH, static_cast<float>(SKILL_SIZE) });
            DrawTextCentered(window, font, fighter.skillName, SKILL_SIZE, skillArea, SKILL_COLOR);
        }
    }

    sf::Vector2f ArenaView::GetFeetPosition(const ArenaFighter& fighter) const
    {
        const float floorY = bounds.position.y + bounds.size.y - FLOOR_MARGIN_BOTTOM;
        float x = fighter.homeX + fighter.offsetX;
        float y = floorY;

        // Salto: sube y baja siguiendo medio seno
        if (fighter.jumpTimeLeft > 0.0f)
        {
            const float jumpProgress = 1.0f - fighter.jumpTimeLeft / JUMP_DURATION;
            y -= std::sin(PI * jumpProgress) * JUMP_HEIGHT;
        }

        // Temblor del Tremor Burst: va y viene de costado muy rapido
        if (fighter.vibrateTimeLeft > 0.0f)
        {
            x += std::sin(elapsedSeconds * VIBRATE_FREQUENCY) * VIBRATE_AMPLITUDE;
        }

        return { x, y };
    }

    float ArenaView::GetClashOffset(const ArenaFighter& fighter) const
    {
        const float centerX = bounds.position.x + bounds.size.x / 2.0f;

        return centerX - fighter.facing * CLASH_GAP - fighter.homeX;
    }

    static sf::Color MixColors(sf::Color from, sf::Color to, float amount)
    {
        const float clamped = (amount < 0.0f) ? 0.0f : ((amount > 1.0f) ? 1.0f : amount);

        return sf::Color(
            static_cast<std::uint8_t>(static_cast<float>(from.r) + (static_cast<float>(to.r) - static_cast<float>(from.r)) * clamped),
            static_cast<std::uint8_t>(static_cast<float>(from.g) + (static_cast<float>(to.g) - static_cast<float>(from.g)) * clamped),
            static_cast<std::uint8_t>(static_cast<float>(from.b) + (static_cast<float>(to.b) - static_cast<float>(from.b)) * clamped),
            from.a);
    }

    static float Approach(float value, float target, float deltaSeconds)
    {
        const float step = deltaSeconds * MOVE_SPEED;
        const float amount = (step > 1.0f) ? 1.0f : step;

        return value + (target - value) * amount;
    }
}
