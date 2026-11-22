#include "ui/text_utils.h"

#include <sstream>

namespace ReyesCuadra
{
    // Mide el ancho en pixeles que ocupa un texto con esa fuente y ese tamano
    static float MeasureTextWidth(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize);

    sf::String ToSfString(const std::string& utf8Text)
    {
        return sf::String::fromUtf8(utf8Text.begin(), utf8Text.end());
    }

    std::vector<std::string> WrapText(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, float maxWidth)
    {
        std::vector<std::string> lines;
        std::istringstream wordStream(utf8Text);
        std::string word;
        std::string currentLine;

        // Se van agregando palabras a la linea actual hasta que la siguiente ya no entra
        while (wordStream >> word)
        {
            const std::string candidate = currentLine.empty() ? word : currentLine + " " + word;

            if (!currentLine.empty() && MeasureTextWidth(font, candidate, characterSize) > maxWidth)
            {
                lines.push_back(currentLine);
                currentLine = word;
            }
            else
            {
                currentLine = candidate;
            }
        }

        if (!currentLine.empty())
        {
            lines.push_back(currentLine);
        }

        return lines;
    }

    void DrawTextAt(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::Vector2f position, sf::Color color)
    {
        sf::Text text(font, ToSfString(utf8Text), characterSize);
        text.setFillColor(color);
        text.setPosition(position);

        window.draw(text);
    }

    void DrawTextCentered(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::FloatRect area, sf::Color color)
    {
        sf::Text text(font, ToSfString(utf8Text), characterSize);
        text.setFillColor(color);

        // Los bounds de un texto no arrancan en (0, 0): se resta su posicion para que quede bien centrado
        const sf::FloatRect bounds = text.getLocalBounds();
        const sf::Vector2f areaCenter = area.position + area.size / 2.0f;
        const sf::Vector2f textCenter = bounds.position + bounds.size / 2.0f;

        text.setPosition(areaCenter - textCenter);

        window.draw(text);
    }

    void DrawTextRightAligned(sf::RenderWindow& window, const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, sf::Vector2f rightTopPosition, sf::Color color)
    {
        sf::Text text(font, ToSfString(utf8Text), characterSize);
        text.setFillColor(color);

        const sf::FloatRect bounds = text.getLocalBounds();

        text.setPosition({ rightTopPosition.x - bounds.position.x - bounds.size.x, rightTopPosition.y });

        window.draw(text);
    }

    static float MeasureTextWidth(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize)
    {
        const sf::Text text(font, ToSfString(utf8Text), characterSize);

        return text.getLocalBounds().size.x;
    }
}
