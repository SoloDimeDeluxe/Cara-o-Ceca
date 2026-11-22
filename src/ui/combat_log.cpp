#include "ui/combat_log.h"

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // Tamanos del texto del panel
    static const unsigned int TITLE_SIZE = 20;
    static const unsigned int LINE_SIZE = 16;
    static const float LINE_HEIGHT = 20.0f;
    static const float PADDING = 12.0f;
    static const float TITLE_HEIGHT = 34.0f;

    // Colores del panel
    static const sf::Color PANEL_COLOR = sf::Color(30, 27, 33);
    static const sf::Color BORDER_COLOR = sf::Color(90, 80, 70);
    static const sf::Color TITLE_COLOR = sf::Color(220, 190, 120);
    static const sf::Color LINE_COLOR = sf::Color(225, 220, 210);

    // Las lineas que arrancan con "--" son separadores de turno y se pintan de otro color
    static const std::string TURN_HEADER_PREFIX = "--";
    static const sf::Color TURN_HEADER_COLOR = sf::Color(160, 150, 140);

    CombatLog::CombatLog(const sf::Font& logFont, sf::FloatRect logArea)
        : font(logFont)
        , area(logArea)
    {
    }

    void CombatLog::AddLine(const std::string& utf8Text)
    {
        const float maxWidth = area.size.x - PADDING * 2.0f;
        const std::vector<std::string> wrappedLines = WrapText(font, utf8Text, LINE_SIZE, maxWidth);

        for (const std::string& wrappedLine : wrappedLines)
        {
            lines.push_back(wrappedLine);
        }
    }

    void CombatLog::Clear()
    {
        lines.clear();
    }

    void CombatLog::Draw(sf::RenderWindow& window) const
    {
        sf::RectangleShape panel(area.size);
        panel.setPosition(area.position);
        panel.setFillColor(PANEL_COLOR);
        panel.setOutlineColor(BORDER_COLOR);
        panel.setOutlineThickness(2.0f);
        window.draw(panel);

        DrawTextAt(window, font, "Registro de combate", TITLE_SIZE, { area.position.x + PADDING, area.position.y + PADDING / 2.0f }, TITLE_COLOR);

        // Se muestran solo las ultimas lineas que entran en el panel, como un chat
        const float availableHeight = area.size.y - TITLE_HEIGHT - PADDING;
        const size_t visibleLines = static_cast<size_t>(availableHeight / LINE_HEIGHT);
        const size_t firstLine = (lines.size() > visibleLines) ? lines.size() - visibleLines : 0;

        float lineY = area.position.y + TITLE_HEIGHT;

        for (size_t lineIndex = firstLine; lineIndex < lines.size(); lineIndex++)
        {
            const bool isTurnHeader = lines[lineIndex].rfind(TURN_HEADER_PREFIX, 0) == 0;
            const sf::Color color = isTurnHeader ? TURN_HEADER_COLOR : LINE_COLOR;

            DrawTextAt(window, font, lines[lineIndex], LINE_SIZE, { area.position.x + PADDING, lineY }, color);

            lineY += LINE_HEIGHT;
        }
    }
}
