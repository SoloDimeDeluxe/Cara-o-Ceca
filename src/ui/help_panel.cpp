#include "ui/help_panel.h"

#include <string>
#include <vector>

#include "ui/text_utils.h"

namespace ReyesCuadra
{
    // ---------- Tamano y margenes del cartel ----------

    static const sf::Vector2f BOX_SIZE = { 1000.0f, 660.0f };
    static const float PADDING = 28.0f;
    static const float TITLE_HEIGHT = 56.0f;
    static const float BORDER_THICKNESS = 2.0f;

    // Fila de pestanas (una por pagina), justo abajo del titulo
    static const float TAB_HEIGHT = 36.0f;
    static const float TAB_GAP = 8.0f;
    static const float CONTENT_TOP_GAP = 20.0f;

    // Cada seccion: barrita de color a la izquierda, titulo y puntos con sangria
    static const float ACCENT_BAR_WIDTH = 4.0f;
    static const float ACCENT_BAR_GAP = 14.0f;
    static const float SECTION_TITLE_HEIGHT = 30.0f;
    static const float SECTION_GAP = 16.0f;
    static const float BULLET_INDENT = 20.0f;
    static const float BULLET_LINE_HEIGHT = 23.0f;
    static const float BULLET_GAP = 4.0f;

    // Botones de abajo: Anterior a la izquierda, Entendido en el medio y Siguiente a la derecha
    static const sf::Vector2f CLOSE_BUTTON_SIZE = { 220.0f, 44.0f };
    static const sf::Vector2f ARROW_BUTTON_SIZE = { 170.0f, 44.0f };

    // ---------- Tamanos de texto ----------

    static const unsigned int TITLE_SIZE = 34;
    static const unsigned int TAB_TEXT_SIZE = 17;
    static const unsigned int SECTION_TITLE_SIZE = 21;
    static const unsigned int BULLET_SIZE = 17;

    // ---------- Colores ----------

    static const sf::Color OVERLAY_COLOR = sf::Color(0, 0, 0, 170);
    static const sf::Color BOX_COLOR = sf::Color(30, 27, 33);
    static const sf::Color GOLD_COLOR = sf::Color(220, 190, 120);
    static const sf::Color TEXT_COLOR = sf::Color(235, 230, 220);
    static const sf::Color TAB_COLOR = sf::Color(45, 41, 49);
    static const sf::Color TAB_HOVER_COLOR = sf::Color(62, 57, 67);
    static const sf::Color TAB_TEXT_COLOR = sf::Color(170, 160, 150);
    static const sf::Color SELECTED_TAB_TEXT_COLOR = sf::Color(30, 27, 33);

    // Color de cada tema (parecido al que tiene en la pelea)
    static const sf::Color TURN_COLOR = sf::Color(220, 190, 120);
    static const sf::Color PLAYER_COLOR = sf::Color(90, 175, 185);
    static const sf::Color EVADE_COLOR = sf::Color(170, 205, 235);
    static const sf::Color WIN_COLOR = sf::Color(120, 200, 120);
    static const sf::Color SANITY_COLOR = sf::Color(140, 160, 230);
    static const sf::Color ENEMY_COLOR = sf::Color(215, 95, 85);
    static const sf::Color POISE_COLOR = sf::Color(235, 205, 95);
    static const sf::Color SINKING_COLOR = sf::Color(110, 150, 220);
    static const sf::Color STAGGER_COLOR = sf::Color(235, 110, 60);
    static const sf::Color TREMOR_COLOR = sf::Color(200, 170, 120);
    static const sf::Color SCORCH_COLOR = sf::Color(240, 140, 50);
    static const sf::Color BURN_COLOR = sf::Color(230, 90, 70);
    static const sf::Color PRESCRIPT_COLOR = sf::Color(220, 190, 120);
    static const sf::Color HARD_MODE_COLOR = sf::Color(215, 95, 85);
    static const sf::Color CONTROLS_COLOR = sf::Color(170, 160, 150);

    // Una seccion de una pagina: titulo, color y los puntos que explica
    struct HelpSection
    {
        const char* title;
        sf::Color color;
        std::vector<const char*> bullets;
    };

    // Una pagina del cartel: el nombre de su pestana y sus secciones
    struct HelpPage
    {
        const char* tabName;
        std::vector<HelpSection> sections;
    };

    // Todas las reglas, repartidas en paginas
    static const std::vector<HelpPage> HELP_PAGES =
    {
        {
            "Tu turno",
            {
                {
                    "La mesa", TURN_COLOR,
                    {
                        "Arriba están los ataques de Lei Heng: hasta 6 por turno, uno por columna.",
                        "Abajo está tu mano: 3 acciones, cada una con 2 skills sorteadas. S1 sale seguido, S2 a veces y S3 poco.",
                        "Si Lei Heng tiene un solo ataque ese turno, tenés 1 sola acción."
                    }
                },
                {
                    "Armar el turno", PLAYER_COLOR,
                    {
                        "Arrastrá una skill hasta la columna del ataque que querés frenar. Clic derecho la saca.",
                        "Una columna vacía es un ataque que te pega directo.",
                        "La skill que usás se gasta y el turno siguiente sale otra.",
                        "Pasá el mouse por una skill para ver qué hace."
                    }
                },
                {
                    "Esquivar", EVADE_COLOR,
                    {
                        "Siempre está disponible y gasta la 1ra opción de esa acción.",
                        "No es un clash: tirás una sola vez y cada golpe que no supere tu número no te hace daño.",
                        "Si esquivás todo, la esquiva sigue activa y enfrenta los ataques que te entren directo, hasta que un golpe la supere o termine el turno."
                    }
                }
            }
        },
        {
            "El clash",
            {
                {
                    "Cómo funciona", WIN_COLOR,
                    {
                        "Los dos tiran sus monedas y cada cara suma poder.",
                        "El que suma más le rompe una moneda al otro. Se repite hasta que uno se queda sin monedas.",
                        "El ganador pega con las monedas que le quedan. Si gana Yi Sang, se cura 5 HP.",
                        "El porcentaje entre las dos filas es tu chance de ganar ese clash: buscá los verdes."
                    }
                },
                {
                    "Cordura (SP)", SANITY_COLOR,
                    {
                        "Va de -45 a +45 y cambia tu chance de sacar cara: 50% + SP.",
                        "Ganar un clash da +10 SP (más si duró varias rondas). Perderlo saca 3 o 4.",
                        "Cada 3 turnos, el SP negativo de Lei Heng vuelve a 0."
                    }
                },
                {
                    "Monedas irrompibles", ENEMY_COLOR,
                    {
                        "Las de Lei Heng pegan aunque pierda el clash...",
                        "...pero cada cara suma solo +1 y hacen la mitad de daño. No siempre vas a poder esquivarlas."
                    }
                }
            }
        },
        {
            "Estados",
            {
                {
                    "Poise", POISE_COLOR,
                    {
                        "Las skills de Yi Sang lo van cargando.",
                        "Cada punto de potencia da 5% de crítico (+20% de daño). Cada 6 de potencia, +1 al poder de cada moneda.",
                        "La cantidad baja 1 por crítico y 1 al final del turno. Si llega a 0, se pierde."
                    }
                },
                {
                    "Sinking", SINKING_COLOR,
                    {
                        "Yi Sang se lo pone a Lei Heng.",
                        "Cada golpe le saca cordura igual a la potencia (si ya está en -45, le saca vida) y la cantidad baja 1."
                    }
                },
                {
                    "Stagger", STAGGER_COLOR,
                    {
                        "Las marcas en las barras de vida son umbrales.",
                        "Si la vida baja de uno, esa unidad queda staggereada: pierde lo que le quedaba del turno, no actúa el siguiente y recibe el doble de daño."
                    }
                }
            }
        },
        {
            "Fuego y temblor",
            {
                {
                    "Tremor", TREMOR_COLOR,
                    {
                        "Lei Heng te lo pone y la cantidad baja 1 al final de cada turno.",
                        "Tremor Burst: tu próximo umbral de Stagger sube tantos puntos como la potencia, así te staggerea antes."
                    }
                },
                {
                    "Tremor - Scorch", SCORCH_COLOR,
                    {
                        "Si Lei Heng no pierde el clash, el Tajo y el Cortador vuelven tu Tremor en Scorch.",
                        "Cada Burst además te hace (Tremor + Burn) / 2 de daño y baja 1 la cantidad de los dos."
                    }
                },
                {
                    "Burn", BURN_COLOR,
                    {
                        "Al final de cada turno recibís daño igual a la potencia y la cantidad baja 1.",
                        "Las skills de Lei Heng pegan más fuerte cuanto más Burn + Tremor tengas.",
                        "Abajo de las barras, los estados se ven como potencia·cantidad."
                    }
                }
            }
        },
        {
            "Extras",
            {
                {
                    "Prescripto", PRESCRIPT_COLOR,
                    {
                        "Cada turno se marcan hasta 2 skills de tu mano con un rombo dorado.",
                        "Usar una marcada: +15% de daño y 1 carga. Con 9 cargas aparece Furioso-Replica: 9 monedas irrompibles, una sola vez."
                    }
                },
                {
                    "Lei Heng se hace fuerte", ENEMY_COLOR,
                    {
                        "Arranca al 30% de su fuerza (daño, Burn y Tremor) y sube 10% por turno hasta llegar al 100%. Se ve arriba."
                    }
                },
                {
                    "Modo Difícil", HARD_MODE_COLOR,
                    {
                        "Lei Heng pega con el 100% de su fuerza desde el primer turno y su SP nunca baja de 0."
                    }
                },
                {
                    "Controles", CONTROLS_COLOR,
                    {
                        "Espacio o Enter: combatir (o saltear la pelea).   Esc: pausa (o cierra este cartel).   M: silenciar la música.   ← →: cambiar de página."
                    }
                }
            }
        }
    };

    static int GetPageCount()
    {
        return static_cast<int>(HELP_PAGES.size());
    }

    HelpPanel::HelpPanel(const sf::Font& panelFont, sf::Vector2f panelScreenSize)
        : font(panelFont)
        , screenSize(panelScreenSize)
        , boxPosition({ (panelScreenSize.x - BOX_SIZE.x) / 2.0f, (panelScreenSize.y - BOX_SIZE.y) / 2.0f })
        , closeButton(panelFont, sf::FloatRect({ (panelScreenSize.x - CLOSE_BUTTON_SIZE.x) / 2.0f, boxPosition.y + BOX_SIZE.y - PADDING - CLOSE_BUTTON_SIZE.y }, CLOSE_BUTTON_SIZE), "¡Entendido!", "")
        , previousButton(panelFont, sf::FloatRect({ boxPosition.x + PADDING, boxPosition.y + BOX_SIZE.y - PADDING - ARROW_BUTTON_SIZE.y }, ARROW_BUTTON_SIZE), "‹ Anterior", "")
        , nextButton(panelFont, sf::FloatRect({ boxPosition.x + BOX_SIZE.x - PADDING - ARROW_BUTTON_SIZE.x, boxPosition.y + BOX_SIZE.y - PADDING - ARROW_BUTTON_SIZE.y }, ARROW_BUTTON_SIZE), "Siguiente ›", "")
        , currentPage(0)
        , isOpen(false)
    {
        GoToPage(0);
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

    void HelpPanel::HandleClick(sf::Vector2f clickPosition)
    {
        if (!isOpen)
        {
            return;
        }

        const sf::FloatRect boxBounds(boxPosition, BOX_SIZE);

        // Un clic afuera del cartel lo cierra, igual que el boton
        if (!boxBounds.contains(clickPosition) || closeButton.Contains(clickPosition))
        {
            Close();
            return;
        }

        if (previousButton.IsEnabled() && previousButton.Contains(clickPosition))
        {
            GoToPage(currentPage - 1);
            return;
        }

        if (nextButton.IsEnabled() && nextButton.Contains(clickPosition))
        {
            GoToPage(currentPage + 1);
            return;
        }

        for (int tabIndex = 0; tabIndex < GetPageCount(); tabIndex++)
        {
            if (GetTabBounds(tabIndex).contains(clickPosition))
            {
                GoToPage(tabIndex);
                return;
            }
        }
    }

    void HelpPanel::HandleKey(sf::Keyboard::Key key)
    {
        if (!isOpen)
        {
            return;
        }

        if (key == sf::Keyboard::Key::Left)
        {
            GoToPage(currentPage - 1);
        }
        else if (key == sf::Keyboard::Key::Right)
        {
            GoToPage(currentPage + 1);
        }
    }

    void HelpPanel::GoToPage(int pageIndex)
    {
        if (pageIndex < 0 || pageIndex >= GetPageCount())
        {
            return;
        }

        currentPage = pageIndex;

        // En la primera pagina no hay Anterior y en la ultima no hay Siguiente
        previousButton.SetEnabled(currentPage > 0);
        nextButton.SetEnabled(currentPage < GetPageCount() - 1);
    }

    sf::FloatRect HelpPanel::GetTabBounds(int tabIndex) const
    {
        // Las pestanas se reparten todo el ancho del cartel, con un espacio entre cada una
        const float rowWidth = BOX_SIZE.x - PADDING * 2.0f;
        const float tabWidth = (rowWidth - TAB_GAP * static_cast<float>(GetPageCount() - 1)) / static_cast<float>(GetPageCount());
        const float tabX = boxPosition.x + PADDING + (tabWidth + TAB_GAP) * static_cast<float>(tabIndex);

        return sf::FloatRect({ tabX, boxPosition.y + TITLE_HEIGHT }, { tabWidth, TAB_HEIGHT });
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

        sf::RectangleShape box(BOX_SIZE);
        box.setPosition(boxPosition);
        box.setFillColor(BOX_COLOR);
        box.setOutlineColor(GOLD_COLOR);
        box.setOutlineThickness(BORDER_THICKNESS);
        window.draw(box);

        DrawTextCentered(window, font, "Cómo se juega", TITLE_SIZE, sf::FloatRect(boxPosition, { BOX_SIZE.x, TITLE_HEIGHT }), GOLD_COLOR);

        DrawTabs(window, mousePosition);
        DrawPageContent(window);

        previousButton.Draw(window, mousePosition);
        closeButton.Draw(window, mousePosition);
        nextButton.Draw(window, mousePosition);
    }

    void HelpPanel::DrawTabs(sf::RenderWindow& window, sf::Vector2f mousePosition) const
    {
        for (int tabIndex = 0; tabIndex < GetPageCount(); tabIndex++)
        {
            const sf::FloatRect tabBounds = GetTabBounds(tabIndex);
            const bool isSelected = tabIndex == currentPage;

            // La pestana de la pagina actual va dorada; las otras se aclaran un poco con el mouse encima
            sf::Color fillColor = TAB_COLOR;

            if (isSelected)
            {
                fillColor = GOLD_COLOR;
            }
            else if (tabBounds.contains(mousePosition))
            {
                fillColor = TAB_HOVER_COLOR;
            }

            sf::RectangleShape tab(tabBounds.size);
            tab.setPosition(tabBounds.position);
            tab.setFillColor(fillColor);
            window.draw(tab);

            const std::string tabText = std::to_string(tabIndex + 1) + ". " + HELP_PAGES[static_cast<size_t>(tabIndex)].tabName;
            DrawTextCentered(window, font, tabText, TAB_TEXT_SIZE, tabBounds, isSelected ? SELECTED_TAB_TEXT_COLOR : TAB_TEXT_COLOR);
        }
    }

    void HelpPanel::DrawPageContent(sf::RenderWindow& window) const
    {
        const float barX = boxPosition.x + PADDING;
        const float textX = barX + ACCENT_BAR_WIDTH + ACCENT_BAR_GAP;
        const float bulletTextX = textX + BULLET_INDENT;
        const float bulletTextWidth = boxPosition.x + BOX_SIZE.x - PADDING - bulletTextX;

        float sectionY = boxPosition.y + TITLE_HEIGHT + TAB_HEIGHT + CONTENT_TOP_GAP;

        for (const HelpSection& section : HELP_PAGES[static_cast<size_t>(currentPage)].sections)
        {
            const float sectionTop = sectionY;

            DrawTextAt(window, font, section.title, SECTION_TITLE_SIZE, { textX, sectionY }, section.color);
            sectionY += SECTION_TITLE_HEIGHT;

            // Cada punto lleva un "•" del color de la seccion y su texto con sangria (las lineas cortadas quedan alineadas)
            for (const char* bullet : section.bullets)
            {
                DrawTextAt(window, font, "•", BULLET_SIZE, { textX, sectionY }, section.color);

                for (const std::string& line : WrapText(font, bullet, BULLET_SIZE, bulletTextWidth))
                {
                    DrawTextAt(window, font, line, BULLET_SIZE, { bulletTextX, sectionY }, TEXT_COLOR);
                    sectionY += BULLET_LINE_HEIGHT;
                }

                sectionY += BULLET_GAP;
            }

            // Barrita de color a la izquierda, del alto de toda la seccion
            sf::RectangleShape accentBar({ ACCENT_BAR_WIDTH, sectionY - sectionTop - BULLET_GAP });
            accentBar.setPosition({ barX, sectionTop });
            accentBar.setFillColor(section.color);
            window.draw(accentBar);

            sectionY += SECTION_GAP;
        }
    }
}
