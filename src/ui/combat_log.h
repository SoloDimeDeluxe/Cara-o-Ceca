#ifndef COMBAT_LOG_H
#define COMBAT_LOG_H

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

namespace ReyesCuadra
{
    // Panel que muestra lo que fue pasando en el combate (los clashes, el dano, etc.)
    class CombatLog
    {
    public:
        CombatLog(const sf::Font& logFont, sf::FloatRect logArea);

        // Agrega una linea al final del registro (si es muy larga se corta en varias lineas)
        void AddLine(const std::string& utf8Text);

        // Borra todo el registro
        void Clear();

        // Dibuja el panel con las ultimas lineas que entran
        void Draw(sf::RenderWindow& window) const;

    private:
        const sf::Font& font;
        sf::FloatRect area;
        std::vector<std::string> lines;
    };
}

#endif // COMBAT_LOG_H
