#ifndef UNIT_H
#define UNIT_H

#include <string>

#include <SFML/Graphics/Color.hpp>

#include "combat/poise.h"

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

        // Estado de Poise de la unidad (chance de critico)
        Poise& GetPoise();
        const Poise& GetPoise() const;

        // Deja la unidad como al principio: vida llena y sin Poise (para empezar una partida nueva)
        void Reset();

    private:
        std::string name;
        int maxHp;
        int hp;
        sf::Color color;
        Poise poise;
    };
}

#endif // UNIT_H
