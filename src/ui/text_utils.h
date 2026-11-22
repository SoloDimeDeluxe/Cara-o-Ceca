#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

namespace ReyesCuadra
{
    // Convierte un texto UTF-8 (con tildes y enies) al formato que usa SFML para dibujarlo bien
    sf::String ToSfString(const std::string& utf8Text);

    // Corta el texto en varias lineas para que ninguna sea mas ancha que maxWidth
    std::vector<std::string> WrapText(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, float maxWidth);

    // Dibuja un texto con su esquina superior izquierda en la posicion indicada
    void DrawTextAt(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::Vector2f position, sf::Color color);

    // Dibuja un texto centrado dentro del rectangulo indicado
    void DrawTextCentered(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::FloatRect area, sf::Color color);

    // Dibuja un texto con su borde derecho en la posicion x indicada (para lo que va pegado al lado derecho de la pantalla)
    void DrawTextRightAligned(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::Vector2f rightTopPosition, sf::Color color);
}

#endif // TEXT_UTILS_H
