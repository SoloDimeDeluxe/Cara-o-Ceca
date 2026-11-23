#ifndef UNIT_H
#define UNIT_H

#include <string>

#include <SFML/Graphics/Color.hpp>

#include "combat/poise.h"
#include "combat/sinking.h"

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

        // Cordura (SP): va de -45 a +45 y arranca en 0
        int GetSanity() const;

        // Devuelve true si la cordura ya esta en el minimo (-45)
        bool IsAtMinSanity() const;

        // Suma o resta cordura, sin pasarse de los limites
        void ChangeSanity(int amount);

        // Chance de sacar cara en cada moneda segun la cordura: 50% + SP% (con 45 de SP es 95%, con -45 es 5%)
        float GetHeadsChance() const;

        // Estado de Poise de la unidad (chance de critico)
        Poise& GetPoise();
        const Poise& GetPoise() const;

        // Estado de Sinking de la unidad (dano extra al recibir golpes)
        Sinking& GetSinking();
        const Sinking& GetSinking() const;

        // Deja la unidad como al principio: vida llena y sin estados (para empezar una partida nueva)
        void Reset();

    private:
        std::string name;
        int maxHp;
        int hp;
        int sanity;
        sf::Color color;
        Poise poise;
        Sinking sinking;
    };
}

#endif // UNIT_H
