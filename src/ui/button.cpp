#include "ui/button.h"

#include <cstddef>

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // Tamanos de los textos del boton
    static const unsigned int LABEL_SIZE = 17;
    static const unsigned int SUB_LABEL_SIZE = 14;
    static const unsigned int RIGHT_TEXT_SIZE = 20;
    static const float TEXT_PADDING_X = 12.0f;
    static const float LABEL_OFFSET_Y = 2.0f;
    static const float SUB_LABEL_OFFSET_Y = 22.0f;
    static const float BORDER_THICKNESS = 2.0f;
    static const float SELECTED_BORDER_THICKNESS = 4.0f;

    // Rombo de la marca: tamano y distancia a la esquina
    static const float MARK_RADIUS = 6.0f;
    static const float MARK_MARGIN = 4.0f;
    static const std::size_t MARK_POINT_COUNT = 4;

    // Colores segun el estado del boton
    static const sf::Color NORMAL_COLOR = sf::Color(45, 40, 48);
    static const sf::Color HOVER_COLOR = sf::Color(70, 60, 72);
    static const sf::Color DISABLED_COLOR = sf::Color(32, 30, 34);
    static const sf::Color BORDER_COLOR = sf::Color(120, 100, 80);
    static const sf::Color SELECTED_BORDER_COLOR = sf::Color(220, 190, 120);
    static const sf::Color MARK_COLOR = sf::Color(240, 200, 90);
    static const sf::Color LABEL_COLOR = sf::Color(235, 230, 220);
    static const sf::Color SUB_LABEL_COLOR = sf::Color(180, 170, 155);
    static const sf::Color DISABLED_TEXT_COLOR = sf::Color(100, 95, 90);

    Button::Button(const sf::Font& buttonFont, sf::FloatRect buttonBounds, const std::string& buttonLabel, const std::string& buttonSubLabel)
        : font(buttonFont)
        , bounds(buttonBounds)
        , label(buttonLabel)
        , subLabel(buttonSubLabel)
        , rightText("")
        , rightTextColor(LABEL_COLOR)
        , isEnabled(true)
        , isSelected(false)
        , isMarked(false)
    {
    }

    bool Button::Contains(sf::Vector2f point) const
    {
        return bounds.contains(point);
    }

    bool Button::IsEnabled() const
    {
        return isEnabled;
    }

    void Button::SetEnabled(bool enabled)
    {
        isEnabled = enabled;
    }

    void Button::SetSelected(bool selected)
    {
        isSelected = selected;
    }

    void Button::SetMarked(bool marked)
    {
        isMarked = marked;
    }

    void Button::SetLabel(const std::string& text)
    {
        label = text;
    }

    void Button::SetSubLabel(const std::string& text)
    {
        subLabel = text;
    }

    void Button::SetRightText(const std::string& text, sf::Color color)
    {
        rightText = text;
        rightTextColor = color;
    }

    void Button::Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const
    {
        sf::Color fillColor = NORMAL_COLOR;

        if (!isEnabled)
        {
            fillColor = DISABLED_COLOR;
        }
        else if (Contains(mousePosition))
        {
            fillColor = HOVER_COLOR;
        }

        sf::RectangleShape shape(bounds.size);
        shape.setPosition(bounds.position);
        shape.setFillColor(fillColor);
        shape.setOutlineColor(isSelected ? SELECTED_BORDER_COLOR : BORDER_COLOR);
        shape.setOutlineThickness(isSelected ? SELECTED_BORDER_THICKNESS : BORDER_THICKNESS);
        window.draw(shape);

        const sf::Color labelColor = isEnabled ? LABEL_COLOR : DISABLED_TEXT_COLOR;
        const sf::Color subLabelColor = isEnabled ? SUB_LABEL_COLOR : DISABLED_TEXT_COLOR;

        // Sin texto secundario, el principal va centrado; con texto secundario, van los dos a la izquierda uno abajo del otro
        if (subLabel.empty())
        {
            DrawTextCentered(window, font, label, LABEL_SIZE, bounds, labelColor);
        }
        else
        {
            DrawTextAt(window, font, label, LABEL_SIZE, { bounds.position.x + TEXT_PADDING_X, bounds.position.y + LABEL_OFFSET_Y }, labelColor);
            DrawTextAt(window, font, subLabel, SUB_LABEL_SIZE, { bounds.position.x + TEXT_PADDING_X, bounds.position.y + SUB_LABEL_OFFSET_Y }, subLabelColor);
        }

        // La marca es un circulo de 4 puntas (un rombo) en la esquina de arriba a la derecha
        if (isMarked)
        {
            sf::CircleShape mark(MARK_RADIUS, MARK_POINT_COUNT);
            mark.setOrigin({ MARK_RADIUS, MARK_RADIUS });
            mark.setPosition({ bounds.position.x + bounds.size.x - MARK_MARGIN - MARK_RADIUS, bounds.position.y + MARK_MARGIN + MARK_RADIUS });
            mark.setFillColor(MARK_COLOR);
            window.draw(mark);
        }

        // El texto de la derecha va centrado en altura, pegado al borde derecho
        if (isEnabled && !rightText.empty())
        {
            const sf::FloatRect rightArea({ bounds.position.x + bounds.size.x - TEXT_PADDING_X, bounds.position.y }, { 0.0f, bounds.size.y });
            sf::Text measure(font, ToSfString(rightText), RIGHT_TEXT_SIZE);
            const float textWidth = measure.getLocalBounds().size.x;
            const sf::FloatRect textArea({ rightArea.position.x - textWidth, bounds.position.y }, { textWidth, bounds.size.y });

            DrawTextCentered(window, font, rightText, RIGHT_TEXT_SIZE, textArea, rightTextColor);
        }
    }
}
