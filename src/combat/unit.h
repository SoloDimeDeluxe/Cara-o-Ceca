#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>

#include <SFML/Graphics/Color.hpp>

#include "combat/poise.h"
#include "combat/sinking.h"
#include "combat/burn.h"
#include "combat/tremor.h"

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

        // Le resta vida a la unidad, sin bajar de 0. Si la vida baja de un umbral de Stagger, la unidad queda staggereada.
        void TakeDamage(int damage);

        // Umbrales de Stagger, como proporcion de la vida maxima (por ejemplo 0.7 = al 70% de vida). Cada uno se rompe una vez.
        void SetStaggerThresholds(const std::vector<float>& thresholds);
        const std::vector<float>& GetStaggerThresholds() const;

        // Devuelve true si el umbral con ese indice ya se rompio
        bool IsThresholdBroken(size_t thresholdIndex) const;

        // Una unidad staggereada no actua (hasta el final del turno siguiente) y recibe el doble de dano de los golpes
        bool IsStaggered() const;

        // Devuelve true una sola vez despues de quedar staggereada (para avisarlo en el registro)
        bool ConsumeStaggerEvent();

        // Se llama al final de cada turno: si estaba staggereada, avanza la recuperacion.
        // Devuelve true si justo se recupero en este turno.
        bool UpdateStaggerOnTurnEnd();

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

        // Estado de Tremor de la unidad (le acerca el proximo umbral de Stagger cuando le hacen Burst)
        Tremor& GetTremor();
        const Tremor& GetTremor() const;

        // "Tremor Burst": sube el proximo umbral de Stagger tantos puntos de porcentaje como la potencia de Tremor
        // (por ejemplo, con 10 de potencia el umbral pasa de 50% a 60%). No gasta cantidad: eso lo decide cada skill.
        // Si la vida queda por debajo del umbral nuevo, la unidad queda staggereada.
        // Devuelve cuantos puntos subio el umbral (0 si no tenia Tremor o ya no le quedaban umbrales).
        int TriggerTremorBurst();

        // Estado de Burn de la unidad (dano fijo al final de cada turno)
        Burn& GetBurn();
        const Burn& GetBurn() const;

        // Deja la unidad como al principio: vida llena y sin estados (para empezar una partida nueva)
        void Reset();

    private:
        std::string name;
        int maxHp;
        int hp;
        int sanity;
        sf::Color color;
        std::vector<float> staggerThresholds;
        size_t brokenThresholds;
        int staggerTurnEndsLeft;
        bool hasPendingStaggerEvent;
        Poise poise;
        Sinking sinking;
        Tremor tremor;
        Burn burn;

        // Umbrales originales (sin los cambios del Tremor), para volver a ponerlos al empezar otra partida
        std::vector<float> originalStaggerThresholds;

        // Rompe los umbrales que la vida ya haya pasado y, si rompio alguno, staggerea a la unidad
        void CheckStaggerThresholds();
    };
}

#endif // UNIT_H
