#ifndef HELP_PANEL_H
#define HELP_PANEL_H

#include <SFML/Graphics.hpp>

#include "ui/button.h"

namespace ReyesCuadra
{
    // Cartel de "Como se juega" con las reglas del combate. Lo usan el menu principal y la pelea.
    // Las reglas estan repartidas en paginas con pestanas arriba; cada pagina tiene secciones con su titulo en color
    // y puntos cortos. Se cambia de pagina con las pestanas, con los botones Anterior / Siguiente o con las flechas.
    class HelpPanel
    {
    public:
        // screenSize es el tamano de la ventana: el cartel se centra y oscurece todo lo de atras
        HelpPanel(const sf::Font& panelFont, sf::Vector2f panelScreenSize);

        bool IsOpen() const;

        // Abre el cartel en la pagina que se vio por ultima vez
        void Open();
        void Close();

        // Clic con el cartel abierto: las pestanas y las flechas cambian de pagina,
        // "¡Entendido!" o un clic afuera del cartel lo cierran
        void HandleClick(sf::Vector2f clickPosition);

        // Flechas izquierda / derecha: pagina anterior / siguiente
        void HandleKey(sf::Keyboard::Key key);

        // Dibuja el cartel (si esta abierto), con los botones resaltados si el mouse esta encima
        void Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const;

    private:
        // Va a la pagina pedida (si existe) y habilita o deshabilita Anterior / Siguiente segun corresponda
        void GoToPage(int pageIndex);

        // Rectangulo de la pestana numero tabIndex
        sf::FloatRect GetTabBounds(int tabIndex) const;

        void DrawTabs(sf::RenderWindow& window, sf::Vector2f mousePosition) const;

        // Dibuja las secciones de la pagina actual una abajo de la otra
        void DrawPageContent(sf::RenderWindow& window) const;

        const sf::Font& font;
        sf::Vector2f screenSize;
        sf::Vector2f boxPosition;
        Button closeButton;
        Button previousButton;
        Button nextButton;
        int currentPage;
        bool isOpen;
    };
}

#endif // HELP_PANEL_H
