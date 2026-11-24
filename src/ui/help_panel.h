#ifndef HELP_PANEL_H
#define HELP_PANEL_H

#include <SFML/Graphics.hpp>

#include "ui/button.h"

namespace ReyesCuadra
{
    // Cartel de "Como se juega" con las reglas del combate. Lo usan el menu principal y la pelea.
    // Mientras esta abierto, cualquier clic lo cierra.
    class HelpPanel
    {
    public:
        // screenSize es el tamano de la ventana: el cartel se centra y oscurece todo lo de atras
        HelpPanel(const sf::Font& panelFont, sf::Vector2f panelScreenSize);

        bool IsOpen() const;
        void Open();
        void Close();

        // Dibuja el cartel (si esta abierto), con el boton de cerrar resaltado si el mouse esta encima
        void Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const;

    private:
        const sf::Font& font;
        sf::Vector2f screenSize;
        Button closeButton;
        bool isOpen;
    };
}

#endif // HELP_PANEL_H
