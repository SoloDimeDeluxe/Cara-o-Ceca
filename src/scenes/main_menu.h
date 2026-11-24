#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Graphics.hpp>

#include "ui/button.h"
#include "ui/help_panel.h"

namespace ReyesCuadra
{
    // Lo que el menu le pide al loop del juego despues de un evento
    enum class MenuAction
    {
        None,
        Play,
        Exit
    };

    // Pantalla de titulo: Jugar, Como se juega, Creditos y Salir.
    // La ayuda y los creditos se muestran ahi mismo, encima del menu.
    class MainMenu
    {
    public:
        explicit MainMenu(const sf::Font& menuFont);

        // Recibe los eventos de la ventana y devuelve si hay que empezar a jugar o cerrar el juego
        MenuAction HandleEvent(const sf::Event& event);

        void Draw(sf::RenderWindow& window) const;

    private:
        // Reacciona a un clic izquierdo segun lo que haya debajo del mouse
        MenuAction HandleClick(sf::Vector2f clickPosition);

        // Moneda grande decorativa con una "C" (de Cara o Ceca) arriba del titulo
        void DrawCoinEmblem(sf::RenderWindow& window) const;

        // Cartel con los creditos (quien lo hizo, en que se basa, fuente y libreria)
        void DrawCredits(sf::RenderWindow& window) const;

        const sf::Font& font;

        Button playButton;
        Button helpButton;
        Button creditsButton;
        Button exitButton;
        HelpPanel helpPanel;

        bool isShowingCredits;
        sf::Vector2f mousePosition;
    };
}

#endif // MAIN_MENU_H
