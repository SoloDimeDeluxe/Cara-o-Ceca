#include "ui/help_panel.h"

#include <string>

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // Tamano y margenes del cartel
    static const sf::Vector2f BOX_SIZE = { 940.0f, 700.0f };
    static const float PADDING = 30.0f;
    static const float TITLE_HEIGHT = 50.0f;
    static const float LINE_HEIGHT = 20.0f;
    static const float RULE_GAP = 6.0f;
    static const sf::Vector2f CLOSE_BUTTON_SIZE = { 220.0f, 48.0f };
    static const float BORDER_THICKNESS = 2.0f;

    // Tamanos de texto
    static const unsigned int TITLE_SIZE = 34;
    static const unsigned int RULE_SIZE = 16;

    // Colores
    static const sf::Color OVERLAY_COLOR = sf::Color(0, 0, 0, 170);
    static const sf::Color BOX_COLOR = sf::Color(30, 27, 33);
    static const sf::Color GOLD_COLOR = sf::Color(220, 190, 120);
    static const sf::Color TEXT_COLOR = sf::Color(235, 230, 220);

    // Reglas que se muestran en el cartel, una por parrafo
    static const char* HELP_RULES[] =
    {
        "1. Cada turno Lei Heng hace hasta 6 acciones, una por columna (fila de arriba). Yi Sang tiene 3 acciones (abajo a la izquierda) y cada una trae 2 skills sorteadas: S1 sale seguido, S2 a veces y S3 poco. Esquivar siempre está.",
        "2. Clic en una columna de Yi Sang y después en una skill de alguna acción para ponerla ahí; clic derecho la saca. La skill que usás se gasta y sale otra el turno siguiente; Esquivar gasta la 1ra opción de esa acción. Si no ponés nada en una columna, ese ataque te pega directo.",
        "3. En el clash los dos tiran sus monedas: cada cara suma poder. El que suma más le rompe una moneda al otro, y se repite hasta que uno se queda sin monedas.",
        "4. El que gana el clash pega con las monedas que le quedan. El porcentaje que aparece entre las dos filas es tu chance de ganar ese clash: buscá los verdes. Ojo: las monedas irrompibles de Lei Heng pegan aunque él pierda el clash.",
        "5. Esquivar no es un clash: tirás una sola vez y cada golpe que no supere tu número no te hace daño. Si esquivás todo, la esquiva sigue activa y enfrenta los próximos ataques que te entren directo, hasta que un golpe la supere o termine el turno. Ponela temprano para que cubra más.",
        "6. Poise: las skills de Yi Sang lo van cargando. Cada punto de potencia da 5% de chance de crítico (+20% de daño) y cada 6 de potencia suman +1 al poder de cada moneda. La cantidad baja 1 por crítico y 1 al final del turno; si llega a 0, se pierde.",
        "7. Sinking: las skills de Yi Sang se lo ponen a Lei Heng. Cada vez que lo golpean pierde cordura igual a la potencia (si ya está en -45, pierde vida) y la cantidad baja 1.",
        "8. Cordura (SP): va de -45 a +45 y cambia la chance de sacar cara: 50% + SP. Ganar un clash da +10 SP (más si duró varias rondas). Pasá el mouse por una skill para ver todos sus efectos (también las de Lei Heng).",
        "9. Stagger: las marcas en las barras de vida son umbrales. Si la vida baja de uno, esa unidad queda staggereada: pierde lo que le quedaba del turno, no actúa el turno siguiente y mientras tanto recibe el doble de daño.",
        "10. Tremor: las skills de Lei Heng se lo ponen a Yi Sang y la cantidad baja 1 al final de cada turno. Varias skills hacen Tremor Burst: tu próximo umbral de Stagger sube tantos puntos como la potencia de Tremor, así te staggerea antes. Si Lei Heng no pierde el clash, Tajo y Cortador lo vuelven Tremor - Scorch: cada Burst además te hace (Tremor + Burn) / 2 de daño y baja 1 la cantidad de los dos.",
        "11. Burn: Lei Heng también te quema. Al final de cada turno recibís daño fijo igual a la potencia y la cantidad baja 1. Sus skills pegan más fuerte cuanto más Burn + Tremor tengas. Abajo de las barras, los estados se muestran como potencia·cantidad."
    };

    HelpPanel::HelpPanel(const sf::Font& panelFont, sf::Vector2f panelScreenSize)
        : font(panelFont)
        , screenSize(panelScreenSize)
        , closeButton(panelFont, sf::FloatRect({ (panelScreenSize.x - CLOSE_BUTTON_SIZE.x) / 2.0f, (panelScreenSize.y + BOX_SIZE.y) / 2.0f - PADDING - CLOSE_BUTTON_SIZE.y }, CLOSE_BUTTON_SIZE), "¡Entendido!", "")
        , isOpen(false)
    {
    }

    bool HelpPanel::IsOpen() const
    {
        return isOpen;
    }

    void HelpPanel::Open()
    {
        isOpen = true;
    }

    void HelpPanel::Close()
    {
        isOpen = false;
    }

    void HelpPanel::Draw(sf::RenderWindow& window, sf::Vector2f mousePosition) const
    {
        if (!isOpen)
        {
            return;
        }

        // Se oscurece lo de atras y encima va el cartel centrado
        sf::RectangleShape overlay(screenSize);
        overlay.setFillColor(OVERLAY_COLOR);
        window.draw(overlay);

        const sf::Vector2f boxPosition = { (screenSize.x - BOX_SIZE.x) / 2.0f, (screenSize.y - BOX_SIZE.y) / 2.0f };

        sf::RectangleShape box(BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(BORDER_THICKNESS);
        window.draw(box);

        DrawTextCentered(window, font, "Cómo se juega", TITLE_SIZE, sf::FloatRect(boxPosition, { BOX_SIZE.x, TITLE_HEIGHT }), GOLD_COLOR);

        // Cada regla se corta en varias lineas para que entre en el ancho del cartel
        const float textWidth = BOX_SIZE.x - PADDING * 2.0f;
        float lineY = boxPosition.y + TITLE_HEIGHT + RULE_GAP;

        for (const char* rule : HELP_RULES)
        {
            for (const std::string& line : WrapText(font, rule, RULE_SIZE, textWidth))
            {
                DrawTextAt(window, font, line, RULE_SIZE, { boxPosition.x + PADDING, lineY }, TEXT_COLOR);
                lineY += LINE_HEIGHT;
            }

            lineY += RULE_GAP;
        }

        closeButton.Draw(window, mousePosition);
    }
}
