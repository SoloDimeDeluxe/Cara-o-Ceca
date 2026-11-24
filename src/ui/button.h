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

        // Cambia el texto principal del boton (por ejemplo "¡Combatir!" -> "Saltear")
        void SetLabel(const std::string& text);

        // Cambia el texto secundario (el chiquito de abajo)
        void SetSubLabel(const std::string& text);

        // Texto corto que se muestra pegado al borde derecho del boton (por ejemplo, la chance de ganar el clash).
        // Con un texto vacio no se muestra nada.
        void SetRightText(const std::string& text, sf::Color color);

        // Un boton seleccionado se dibuja con el borde dorado (por ejemplo, la skill elegida para una accion)
        void SetSelected(bool selected);

        // Un boton marcado lleva un rombo dorado en la esquina de arriba a la derecha (la Marca del Prescripto)
        void SetMarked(bool marked);

        // Dibuja el boton, resaltado si el mouse esta encima
        void Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const;

    private:
        const sf::Font& font;
        sf::FloatRect bounds;
        std::string label;
        std::string subLabel;
        std::string rightText;
        sf::Color rightTextColor;
        bool isEnabled;
        bool isSelected;
        bool isMarked;
    };
}

#endif // BUTTON_H
