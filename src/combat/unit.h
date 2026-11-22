#ifndef UNIT_H
#define UNIT_H

#include <string>

#include <SFML/Graphics/Color.hpp>

namespace ReyesCuadra
{
    // Personaje que pelea (Yi Sang o Lei Heng): tiene nombre, vida y un color para dibujarlo
    class Unit
    {
    public:
        Unit(const std::string& unitName, int unitMaxHp, sf::Color unitColor);

        const std::string& GetName() const;
        int GetHp() const;
        int GetMaxHp() const;

        // Vida actual dividida la vida maxima (1 = vida llena, 0 = muerto)
        float GetHpRatio() const;

        sf::Color GetColor() const;

        // Le resta vida a la unidad, sin bajar de 0
        void TakeDamage(int damage);

        bool IsDead() const;

        // Vuelve a dejar la vida llena (para empezar una partida nueva)
        void ResetHp();

    private:
        std::string name;
        int maxHp;
        int hp;
        sf::Color color;
    };
}

#endif // UNIT_H
