#include "scenes/main_menu.h"

#include <string>

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // ---------- Layout (1280 x 720) ----------

    static const float SCREEN_WIDTH = 1280.0f;
    static const float SCREEN_HEIGHT = 720.0f;

    // Moneda decorativa y titulo
    static const float COIN_CENTER_Y = 120.0f;
    static const float COIN_RADIUS = 54.0f;
    static const float COIN_INNER_RADIUS = 42.0f;
    static const float COIN_OUTLINE_THICKNESS = 4.0f;
    static const float TITLE_Y = 190.0f;
    static const float TITLE_HEIGHT = 80.0f;
    static const float SUBTITLE_Y = 268.0f;
    static const float SUBTITLE_HEIGHT = 32.0f;

    // Cuadrados de color de cada personaje, a los costados del subtitulo
    static const float PORTRAIT_SIZE = 26.0f;
    static const float PORTRAIT_OFFSET_X = 170.0f;

    // Botones, uno abajo del otro
    static const sf::Vector2f BUTTON_SIZE = { 320.0f, 56.0f };
    static const float FIRST_BUTTON_Y = 330.0f;
    static const float BUTTON_GAP = 16.0f;

    // Texto de abajo de todo
    static const float FOOTER_Y = 680.0f;
    static const float FOOTER_HEIGHT = 24.0f;

    // Cartel de creditos
    static const sf::Vector2f CREDITS_BOX_SIZE = { 760.0f, 440.0f };
    static const float CREDITS_PADDING = 30.0f;
    static const float CREDITS_TITLE_HEIGHT = 60.0f;
    static const float CREDITS_LINE_HEIGHT = 24.0f;
    static const float CREDITS_PARAGRAPH_GAP = 10.0f;
    static const float CREDITS_HINT_OFFSET_Y = 40.0f;
    static const float BORDER_THICKNESS = 2.0f;

    // ---------- Tamanos de texto ----------

    static const unsigned int COIN_LETTER_SIZE = 56;
    static const unsigned int TITLE_SIZE = 72;
    static const unsigned int SUBTITLE_SIZE = 26;
    static const unsigned int FOOTER_SIZE = 16;
    static const unsigned int CREDITS_TITLE_SIZE = 36;
    static const unsigned int CREDITS_TEXT_SIZE = 19;
    static const unsigned int CREDITS_HINT_SIZE = 16;

    // ---------- Colores ----------

    static const sf::Color GOLD_COLOR = sf::Color(220, 190, 120);
    static const sf::Color COIN_FILL_COLOR = sf::Color(200, 160, 70);
    static const sf::Color COIN_INNER_COLOR = sf::Color(170, 130, 50);
    static const sf::Color COIN_OUTLINE_COLOR = sf::Color(110, 85, 40);
    static const sf::Color COIN_LETTER_COLOR = sf::Color(80, 60, 25);
    static const sf::Color TEXT_COLOR = sf::Color(235, 230, 220);
    static const sf::Color DIM_TEXT_COLOR = sf::Color(150, 140, 130);
    static const sf::Color PLAYER_COLOR = sf::Color(70, 140, 150);
    static const sf::Color ENEMY_COLOR = sf::Color(170, 60, 50);
    static const sf::Color OVERLAY_COLOR = sf::Color(0, 0, 0, 170);
    static const sf::Color BOX_COLOR = sf::Color(30, 27, 33);

    // Parrafos del cartel de creditos
    static const char* CREDITS_LINES[] =
    {
        "Programación y diseño: Valentin Reyes",
        "Sprites: Valentin Reyes, basados en los diseños originales de Project Moon",
        "Desarrollo de Videojuegos I - Image Campus, 2026",
        "Juego de fans inspirado en el combate de Limbus Company. Yi Sang, Lei Heng y sus skills pertenecen a Project Moon.",
        "Hecho en C++ con SFML 3.1.",
        "Fuente: Crimson Text (SIL Open Font License)."
    };

    // Orden de los botones, de arriba hacia abajo
    enum MenuButtonIndex
    {
        PLAY_BUTTON = 0,
        HELP_BUTTON,
        CREDITS_BUTTON,
        EXIT_BUTTON
    };

    // Posicion del boton numero buttonIndex (0 = el de arriba)
    static sf::FloatRect GetButtonBounds(int buttonIndex);

    MainMenu::MainMenu(const sf::Font& menuFont)
        : font(menuFont)
        , playButton(menuFont, GetButtonBounds(PLAY_BUTTON), "Jugar", "")
        , helpButton(menuFont, GetButtonBounds(HELP_BUTTON), "Cómo se juega", "")
        , creditsButton(menuFont, GetButtonBounds(CREDITS_BUTTON), "Créditos", "")
        , exitButton(menuFont, GetButtonBounds(EXIT_BUTTON), "Salir", "")
        , helpPanel(menuFont, { SCREEN_WIDTH, SCREEN_HEIGHT })
        , isShowingCredits(false)
        , mousePosition({ 0.0f, 0.0f })
    {
    }

    MenuAction MainMenu::HandleEvent(const sf::Event& event)
    {
        if (const sf::Event::MouseMoved* moved = event.getIf<sf::Event::MouseMoved>())
        {
            mousePosition = sf::Vector2f(moved->position);
        }
        else if (event.is<sf::Event::MouseLeft>())
        {
            // Fuera de la ventana no hay que resaltar ningun boton
            mousePosition = { -SCREEN_WIDTH, -SCREEN_HEIGHT };
        }
        else if (const sf::Event::MouseButtonPressed* pressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (pressed->button == sf::Mouse::Button::Left)
            {
                return HandleClick(sf::Vector2f(pressed->position));
            }
        }
        else if (const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>())
        {
            // Esc cierra el cartel que este abierto
            if (key->code == sf::Keyboard::Key::Escape)
            {
                helpPanel.Close();
                isShowingCredits = false;
            }
        }

        return MenuAction::None;
    }

    MenuAction MainMenu::HandleClick(sf::Vector2f clickPosition)
    {
        // Con un cartel abierto, cualquier clic lo cierra
        if (helpPanel.IsOpen())
        {
            helpPanel.Close();
            return MenuAction::None;
        }

        if (isShowingCredits)
        {
            isShowingCredits = false;
            return MenuAction::None;
        }

        if (playButton.Contains(clickPosition))
        {
            return MenuAction::Play;
        }

        if (helpButton.Contains(clickPosition))
        {
            helpPanel.Open();
        }
        else if (creditsButton.Contains(clickPosition))
        {
            isShowingCredits = true;
        }
        else if (exitButton.Contains(clickPosition))
        {
            return MenuAction::Exit;
        }

        return MenuAction::None;
    }

    void MainMenu::Draw(sf::RenderWindow& window) const
    {
        DrawCoinEmblem(window);

        DrawTextCentered(window, font, "Cara o Ceca", TITLE_SIZE, sf::FloatRect({ 0.0f, TITLE_Y }, { SCREEN_WIDTH, TITLE_HEIGHT }), GOLD_COLOR);

        // Subtitulo con un cuadrado del color de cada personaje a los costados
        const sf::FloatRect subtitleArea({ 0.0f, SUBTITLE_Y }, { SCREEN_WIDTH, SUBTITLE_HEIGHT });
        DrawTextCentered(window, font, "Yi Sang  vs  Lei Heng", SUBTITLE_SIZE, subtitleArea, TEXT_COLOR);

        const float portraitY = SUBTITLE_Y + (SUBTITLE_HEIGHT - PORTRAIT_SIZE) / 2.0f;

        sf::RectangleShape playerPortrait({ PORTRAIT_SIZE, PORTRAIT_SIZE });
        playerPortrait.setPosition({ SCREEN_WIDTH / 2.0f - PORTRAIT_OFFSET_X - PORTRAIT_SIZE, portraitY });
        playerPortrait.setFillColor(PLAYER_COLOR);
        window.draw(playerPortrait);

        sf::RectangleShape enemyPortrait({ PORTRAIT_SIZE, PORTRAIT_SIZE });
        enemyPortrait.setPosition({ SCREEN_WIDTH / 2.0f + PORTRAIT_OFFSET_X, portraitY });
        enemyPortrait.setFillColor(ENEMY_COLOR);
        window.draw(enemyPortrait);

        playButton.Draw(window, mousePosition);
        helpButton.Draw(window, mousePosition);
        creditsButton.Draw(window, mousePosition);
        exitButton.Draw(window, mousePosition);

        DrawTextCentered(window, font, "Valentin Reyes  ·  Desarrollo de Videojuegos I  ·  Image Campus 2026", FOOTER_SIZE, sf::FloatRect({ 0.0f, FOOTER_Y }, { SCREEN_WIDTH, FOOTER_HEIGHT }), DIM_TEXT_COLOR);

        if (isShowingCredits)
        {
            DrawCredits(window);
        }

        helpPanel.Draw(window, mousePosition);
    }

    void MainMenu::DrawCoinEmblem(sf::RenderWindow& window) const
    {
        sf::CircleShape coin(COIN_RADIUS);
        coin.setOrigin({ COIN_RADIUS, COIN_RADIUS });
        coin.setPosition({ SCREEN_WIDTH / 2.0f, COIN_CENTER_Y });
        coin.setFillColor(COIN_FILL_COLOR);
        coin.setOutlineColor(COIN_OUTLINE_COLOR);
        coin.setOutlineThickness(COIN_OUTLINE_THICKNESS);
        window.draw(coin);

        sf::CircleShape innerRing(COIN_INNER_RADIUS);
        innerRing.setOrigin({ COIN_INNER_RADIUS, COIN_INNER_RADIUS });
        innerRing.setPosition({ SCREEN_WIDTH / 2.0f, COIN_CENTER_Y });
        innerRing.setFillColor(COIN_INNER_COLOR);
        window.draw(innerRing);

        const sf::FloatRect letterArea({ SCREEN_WIDTH / 2.0f - COIN_RADIUS, COIN_CENTER_Y - COIN_RADIUS }, { COIN_RADIUS * 2.0f, COIN_RADIUS * 2.0f });
        DrawTextCentered(window, font, "C", COIN_LETTER_SIZE, letterArea, COIN_LETTER_COLOR);
    }

    void MainMenu::DrawCredits(sf::RenderWindow& window) const
    {
        sf::RectangleShape overlay({ SCREEN_WIDTH, SCREEN_HEIGHT });
        overlay.setFillColor(OVERLAY_COLOR);
        window.draw(overlay);

        const sf::Vector2f boxPosition = { (SCREEN_WIDTH - CREDITS_BOX_SIZE.x) / 2.0f, (SCREEN_HEIGHT - CREDITS_BOX_SIZE.y) / 2.0f };

        sf::RectangleShape box(CREDITS_BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(BORDER_THICKNESS);
        window.draw(box);

        DrawTextCentered(window, font, "Créditos", CREDITS_TITLE_SIZE, sf::FloatRect(boxPosition, { CREDITS_BOX_SIZE.x, CREDITS_TITLE_HEIGHT }), GOLD_COLOR);

        const float textWidth = CREDITS_BOX_SIZE.x - CREDITS_PADDING * 2.0f;
        float lineY = boxPosition.y + CREDITS_TITLE_HEIGHT + CREDITS_PARAGRAPH_GAP;

        for (const char* paragraph : CREDITS_LINES)
        {
            for (const std::string& line : WrapText(font, paragraph, CREDITS_TEXT_SIZE, textWidth))
            {
                DrawTextAt(window, font, line, CREDITS_TEXT_SIZE, { boxPosition.x + CREDITS_PADDING, lineY }, TEXT_COLOR);
                lineY += CREDITS_LINE_HEIGHT;
            }

            lineY += CREDITS_PARAGRAPH_GAP;
        }

        const sf::FloatRect hintArea({ boxPosition.x, boxPosition.y + CREDITS_BOX_SIZE.y - CREDITS_HINT_OFFSET_Y }, { CREDITS_BOX_SIZE.x, static_cast<float>(CREDITS_HINT_SIZE) });
        DrawTextCentered(window, font, "Clic en cualquier lado para volver", CREDITS_HINT_SIZE, hintArea, DIM_TEXT_COLOR);
    }

    static sf::FloatRect GetButtonBounds(int buttonIndex)
    {
        const float buttonY = FIRST_BUTTON_Y + static_cast<float>(buttonIndex) * (BUTTON_SIZE.y + BUTTON_GAP);

        return sf::FloatRect({ (SCREEN_WIDTH - BUTTON_SIZE.x) / 2.0f, buttonY }, BUTTON_SIZE);
    }
}
