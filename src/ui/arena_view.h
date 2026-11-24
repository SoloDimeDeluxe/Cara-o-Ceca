#ifndef ARENA_VIEW_H
#define ARENA_VIEW_H

#include <array>
#include <string>

#include <SFML/Graphics.hpp>

namespace ReyesCuadra
{
    // Pose del sprite que se muestra: quieto, atacando o recibiendo un golpe
    enum class FighterPose
    {
        Idle,
        Attack,
        Hurt
    };

    // Cantidad de poses (para el arreglo de texturas)
    const int FIGHTER_POSE_COUNT = 3;

    // Texturas de las poses de un personaje (si no se pudo cargar alguna, se usa la de quieto;
    // si no esta ni la de quieto, la figura se dibuja con formas simples)
    struct FighterSprites
    {
        std::array<sf::Texture, FIGHTER_POSE_COUNT> textures;
        std::array<bool, FIGHTER_POSE_COUNT> isLoaded = { false, false, false };
    };

    // Una de las dos figuras de la arena (Yi Sang a la izquierda, Lei Heng a la derecha)
    struct ArenaFighter
    {
        std::string name;
        sf::Color color;

        // Donde se para cuando no hace nada (centro de los pies) y para que lado mira (1 = derecha, -1 = izquierda)
        float homeX = 0.0f;
        float facing = 1.0f;

        // Tamano de la figura (Lei Heng es mas grande)
        float scale = 1.0f;

        // Corrimiento actual respecto de su lugar, y hacia donde se esta moviendo
        float offsetX = 0.0f;
        float targetOffsetX = 0.0f;

        // Salto (para la esquiva): tiempo que le queda en el aire
        float jumpTimeLeft = 0.0f;

        // Tiempo que le queda al destello blanco de "me pegaron"
        float hitFlashTimeLeft = 0.0f;

        // Tiempo que le queda temblando por un Tremor Burst (y si fue Scorch, para pintarlo naranja)
        float vibrateTimeLeft = 0.0f;
        bool isScorchVibration = false;

        // Cuando termina este tiempo, la figura vuelve a su lugar
        float returnTimeLeft = 0.0f;

        // Tiempo que le queda mostrando la pose de ataque o la de golpeado
        float attackPoseTimeLeft = 0.0f;
        float hurtPoseTimeLeft = 0.0f;

        // Staggereado: queda inclinado. Caido: se murio
        bool isStaggered = false;
        bool isDefeated = false;

        // Skill que esta usando en la columna que se esta peleando (se muestra arriba de la figura)
        std::string skillName;
    };

    // Arena donde se ve la pelea con dos figuras simples: se tiran una contra otra en el clash, retroceden
    // al recibir golpes, Yi Sang salta al esquivar, vibran con el Tremor Burst y se inclinan con el Stagger.
    // No sabe nada de las reglas: el gameplay le avisa que paso con cada paso de la pelea.
    class ArenaView
    {
    public:
        ArenaView(const sf::Font& arenaFont, sf::FloatRect arenaBounds, sf::Color playerColor, sf::Color enemyColor);

        // Deja las dos figuras en su lugar, paradas y sin efectos (al empezar a mostrar un turno)
        void Reset(bool playerIsStaggered, bool enemyIsStaggered);

        // Empieza una columna: las figuras vuelven a su lugar y muestran que skill usa cada una
        void OnColumnStart(const std::string& playerSkillName, const std::string& enemySkillName);

        // Ronda de clash: los dos se tiran hacia el medio
        void OnClashRound();

        // Se vio como cayeron las monedas: el que perdio la ronda retrocede (con empate, los dos un poco)
        void OnClashRoundResult(bool playerLostRound, bool isTie);

        // Golpe: el que pega se tira encima del otro, y el otro retrocede y destella
        void OnHit(bool targetIsPlayer, bool targetIsDefeated);

        // Esquiva: Yi Sang salta hacia atras
        void OnEvade();

        // Tremor Burst sobre una figura: vibra (naranja si es Scorch)
        void OnTremorBurst(bool targetIsPlayer, bool isScorch);

        // Stagger sobre una figura: queda inclinada
        void OnStagger(bool targetIsPlayer);

        void Update(float deltaSeconds);
        void Draw(sf::RenderWindow& window) const;

        // Arriba de la cabeza de una figura (para que el gameplay saque numeros de dano ahi)
        sf::Vector2f GetHeadPosition(bool isPlayer) const;

    private:
        // Carga las 3 poses de un personaje desde res/sprites (fileName_idle.png, _attack.png, _hurt.png)
        static void LoadSprites(FighterSprites& sprites, const std::string& fileName);

        // Pose que corresponde mostrar ahora (golpeado, atacando o quieto)
        static FighterPose GetCurrentPose(const ArenaFighter& fighter);

        // Dibuja una figura con su transformacion (posicion, para donde mira, inclinacion, temblor)
        void DrawFighter(sf::RenderWindow& window, const ArenaFighter& fighter, const FighterSprites& sprites) const;

        // Dibuja la figura con el sprite de la pose actual
        void DrawFighterSprite(sf::RenderWindow& window, const ArenaFighter& fighter, const FighterSprites& sprites, sf::Vector2f feet, float tiltDegrees) const;

        // Dibuja la figura con formas simples (si no hay sprites)
        void DrawFighterShapes(sf::RenderWindow& window, const ArenaFighter& fighter, sf::Vector2f feet, float tiltDegrees) const;

        // Nombre abajo de los pies y skill que usa arriba de la cabeza
        void DrawFighterLabels(sf::RenderWindow& window, const ArenaFighter& fighter, float figureHeight) const;

        // Alto de la figura parada (el del sprite de quieto, o el de las formas simples)
        float GetFigureHeight(const ArenaFighter& fighter, const FighterSprites& sprites) const;

        // Pie de la figura en este momento (lugar + corrimiento + salto + temblor)
        sf::Vector2f GetFeetPosition(const ArenaFighter& fighter) const;

        // Corrimiento hacia el medio que usan los dos al chocar (cada uno queda a un costado del centro)
        float GetClashOffset(const ArenaFighter& fighter) const;

        const sf::Font& font;
        sf::FloatRect bounds;
        ArenaFighter playerFighter;
        ArenaFighter enemyFighter;
        FighterSprites playerSprites;
        FighterSprites enemySprites;

        // Chispa del choque en el medio de la arena
        float sparkTimeLeft;

        // Tiempo total (para el temblor)
        float elapsedSeconds;
    };
}

#endif // ARENA_VIEW_H
