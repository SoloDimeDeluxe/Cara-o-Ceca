#include "ui/text_utils.h"

#include <map>
#include <sstream>

namespace ReyesCuadra
{
    // Mide el ancho en pixeles que ocupa un texto con esa fuente y ese tamano
    static float MeasureTextWidth(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize);

    sf::String ToSfString(const std::string& utf8Text)
    {
        return sf::String::fromUtf8(utf8Text.begin(), utf8Text.end());
    }

    // Tope de textos guardados: el registro de combate agrega lineas nuevas todo el tiempo,
    // asi que si se llena se vacia y se vuelve a armar con lo que se este dibujando
    static const size_t MAX_CACHED_TEXTS = 500;

    // Arma la clave con la que se guarda un texto ya cortado en renglones (texto, fuente, tamano y ancho)
    static std::string MakeWrapKey(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, float maxWidth);

    std::vector<std::string> WrapText(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, float maxWidth)
    {
        // Medir el texto palabra por palabra es lento, y muchos textos se dibujan en todos los frames
        // (tarjetas, detalle de la skill, ayuda). Cada texto se corta una sola vez y despues se usa lo guardado.
        static std::map<std::string, std::vector<std::string>> wrappedTextCache;

        const std::string cacheKey = MakeWrapKey(font, utf8Text, characterSize, maxWidth);
        const std::map<std::string, std::vector<std::string>>::const_iterator cached = wrappedTextCache.find(cacheKey);

        if (cached != wrappedTextCache.end())
        {
            return cached->second;
        }

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

        if (wrappedTextCache.size() >= MAX_CACHED_TEXTS)
        {
            wrappedTextCache.clear();
        }

        wrappedTextCache[cacheKey] = lines;

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

    static std::string MakeWrapKey(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize, float maxWidth)
    {
        std::ostringstream key;
        key << static_cast<const void*>(&font) << '|' << characterSize << '|' << maxWidth << '|' << utf8Text;

        return key.str();
    }

    static float MeasureTextWidth(const sf::Font& font, const std::string& utf8Text, unsigned int characterSize)
    {
        const sf::Text text(font, ToSfString(utf8Text), characterSize);

        return text.getLocalBounds().size.x;
    }
}
