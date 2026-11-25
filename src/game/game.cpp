#include "game/game.h"

#include <iostream>
#include <optional>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "scenes/gameplay.h"
#include "scenes/main_menu.h"
#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // Configuracion de la ventana
    static const unsigned int SCREEN_WIDTH = 1280;
    static const unsigned int SCREEN_HEIGHT = 720;
    static const unsigned int TARGET_FPS = 60;
    static const char* WINDOW_TITLE = "Cara o Ceca";

    // Color de fondo de la pantalla (gris muy oscuro)
    static const sf::Color BACKGROUND_COLOR = sf::Color(18, 16, 20);

    // Ruta de la fuente que usan todos los textos del juego
    static const char* FONT_PATH = "res/fonts/CrimsonText-SemiBold.ttf";

    // Musica de la batalla: se repite en loop mientras dura la pelea y se corta al volver al menu
    static const char* BATTLE_MUSIC_PATH = "res/music/battle_theme.ogg";
    static const float MUSIC_VOLUME = 35.0f;
    static const float MUTED_VOLUME = 0.0f;

    // Tecla para silenciar o volver a prender la musica (anda en cualquier pantalla)
    static const sf::Keyboard::Key MUTE_KEY = sf::Keyboard::Key::M;

    // Pantalla que se esta mostrando
    enum class Scene
    {
        Menu,
        Battle
    };

    // Musica de fondo de la pelea. Si el archivo no esta, el juego anda igual pero en silencio.
    struct BattleMusic
    {
        sf::Music music;
        bool isLoaded = false;
        bool isMuted = false;
    };

    // Procesa los eventos de la ventana: la cruz la cierra, la M silencia la musica y el resto se lo pasa a la escena actual
    // (el menu puede pedir empezar a jugar o salir)
    static void HandleWindowEvents(sf::RenderWindow& window, Scene& currentScene, MainMenu& menu, Gameplay& gameplay, BattleMusic& battleMusic);

    // Arranca la musica desde el principio (se llama al entrar a la pelea desde el menu)
    static void PlayBattleMusic(BattleMusic& battleMusic);

    // Corta la musica (al volver al menu)
    static void StopBattleMusic(BattleMusic& battleMusic);

    // Silencia la musica o la vuelve a prender
    static void ToggleMute(BattleMusic& battleMusic);

    // Limpia la pantalla, dibuja la escena actual y muestra el frame
    static void DrawFrame(sf::RenderWindow& window, Scene currentScene, const MainMenu& menu, const Gameplay& gameplay);

    void RunGame()
    {
        // SFML no trae una fuente por defecto, asi que si no se encuentra el archivo no se puede mostrar nada
        sf::Font font;

        if (!font.openFromFile(FONT_PATH))
        {
            std::cerr << "No se pudo cargar la fuente: " << FONT_PATH << std::endl;
            return;
        }

        // Titlebar | Close: la ventana no se puede agrandar, asi las coordenadas del mouse coinciden con las del juego
        sf::RenderWindow window(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), ToSfString(WINDOW_TITLE), sf::Style::Titlebar | sf::Style::Close);
        window.setFramerateLimit(TARGET_FPS);

        MainMenu menu(font);
        Gameplay gameplay(font);

        // La musica se lee de a pedazos desde el archivo (no se carga entera en memoria)
        BattleMusic battleMusic;
        battleMusic.isLoaded = battleMusic.music.openFromFile(BATTLE_MUSIC_PATH);

        if (battleMusic.isLoaded)
        {
            battleMusic.music.setLooping(true);
            battleMusic.music.setVolume(MUSIC_VOLUME);
        }
        else
        {
            std::cerr << "No se pudo cargar la musica: " << BATTLE_MUSIC_PATH << std::endl;
        }

        // El juego arranca en el menu principal
        Scene currentScene = Scene::Menu;

        // Mide cuanto tiempo paso entre un frame y el siguiente (para las animaciones)
        sf::Clock frameClock;

        while (window.isOpen())
        {
            const float deltaSeconds = frameClock.restart().asSeconds();

            HandleWindowEvents(window, currentScene, menu, gameplay, battleMusic);

            if (currentScene == Scene::Battle)
            {
                gameplay.Update(deltaSeconds);

                // Desde la pausa o el fin de partida se puede volver al menu
                if (gameplay.ConsumeReturnToMenuRequest())
                {
                    currentScene = Scene::Menu;
                    StopBattleMusic(battleMusic);
                }
            }

            DrawFrame(window, currentScene, menu, gameplay);
        }
    }

    static void HandleWindowEvents(sf::RenderWindow& window, Scene& currentScene, MainMenu& menu, Gameplay& gameplay, BattleMusic& battleMusic)
    {
        // En SFML 3 pollEvent devuelve un std::optional: queda vacio cuando ya no hay mas eventos
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>(); keyPressed != nullptr && keyPressed->code == MUTE_KEY)
            {
                ToggleMute(battleMusic);
            }
            else if (currentScene == Scene::Menu)
            {
                const MenuAction action = menu.HandleEvent(*event);

                if (action == MenuAction::Play || action == MenuAction::PlayHard)
                {
                    // Cada vez que se entra desde el menu arranca una partida nueva (normal o en Dificil)
                    gameplay.StartNewGame(action == MenuAction::PlayHard);
                    currentScene = Scene::Battle;
                    PlayBattleMusic(battleMusic);
                }
                else if (action == MenuAction::Exit)
                {
                    window.close();
                }
            }
            else
            {
                gameplay.HandleEvent(*event);
            }
        }
    }

    static void PlayBattleMusic(BattleMusic& battleMusic)
    {
        if (!battleMusic.isLoaded)
        {
            return;
        }

        // stop() la vuelve al principio, asi cada partida nueva arranca con la cancion desde cero
        battleMusic.music.stop();
        battleMusic.music.play();
    }

    static void StopBattleMusic(BattleMusic& battleMusic)
    {
        if (battleMusic.isLoaded)
        {
            battleMusic.music.stop();
        }
    }

    static void ToggleMute(BattleMusic& battleMusic)
    {
        battleMusic.isMuted = !battleMusic.isMuted;

        if (battleMusic.isLoaded)
        {
            battleMusic.music.setVolume(battleMusic.isMuted ? MUTED_VOLUME : MUSIC_VOLUME);
        }
    }

    static void DrawFrame(sf::RenderWindow& window, Scene currentScene, const MainMenu& menu, const Gameplay& gameplay)
    {
        window.clear(BACKGROUND_COLOR);

        if (currentScene == Scene::Menu)
        {
            menu.Draw(window);
        }
        else
        {
            gameplay.Draw(window);
        }

        window.display();
    }
}
