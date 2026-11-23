#include "utils/random.h"

#include <random>

namespace ReyesCuadra
{
    // Devuelve el generador de numeros aleatorios del juego
    static std::mt19937& GetGenerator();

    bool RollChance(float chance)
    {
        // bernoulli_distribution devuelve true con la probabilidad que se le pasa
        std::bernoulli_distribution distribution(static_cast<double>(chance));

        return distribution(GetGenerator());
    }

    static std::mt19937& GetGenerator()
    {
        static std::mt19937 generator(std::random_device{}());

        return generator;
    }
}
