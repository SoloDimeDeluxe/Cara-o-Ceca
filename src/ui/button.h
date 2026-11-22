#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include <SFML/Graphics.hpp>

namespace ReyesCuadra
{
    // Boton rectangular que se aprieta con el mouse. Puede tener un texto principal y uno secundario mas chico abajo.
    class Button
    {
    public:
        Button(const sf::Font& buttonFont, sf::FloatRect buttonBounds, const std::string& buttonLabel, const std::string& buttonSubLabel);

        // Devuelve true si el punto (por ejemplo, la posicion del mouse) esta adentro del boton
        bool Contains(sf::Vector2f point) const;

        // Un boton deshabilitado se ve gris y no responde a los clics
        bool IsEnabled() const;
        void SetEnabled(bool enabled);

        // Dibuja el boton, resaltado si el mouse esta encima
        void Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const;

    private:
        const sf::Font& font;
        sf::FloatRect bounds;
        std::string label;
        std::string subLabel;
        bool isEnabled;
    };
}

#endif // BUTTON_H
