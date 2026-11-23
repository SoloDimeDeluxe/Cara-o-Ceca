#ifndef TREMOR_H
#define TREMOR_H

#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Estado "Tremor" de Limbus: no hace dano, pero prepara el Stagger.
    //   Potencia: cuanto sube el umbral de Stagger de quien lo tiene cada vez que le hacen un "Tremor Burst".
    //   Cantidad: baja 1 al final de cada turno (y algunas skills la bajan al hacer Burst).
    // El Burst lo hace la Unit (porque es la que tiene los umbrales de Stagger); este estado solo guarda los numeros.
    //
    // Variante "Tremor - Scorch": algunas skills de Lei Heng convierten el Tremor en Scorch. Entonces cada Burst ademas
    // hace dano igual a (potencia de Tremor + potencia de Burn) / 2 y baja 1 la cantidad de Tremor y de Burn.
    // Cuando el Tremor se acaba, vuelve a ser Tremor comun.
    class Tremor : public StatusEffect
    {
    public:
        Tremor();

        // Al final del turno la cantidad baja 1
        void OnTurnEnd();

        // Saca 1 de cantidad (lo usan las skills que hacen Burst)
        void LoseOneCount();

        // Convierte el Tremor en Tremor - Scorch (solo si tiene Tremor)
        void ConvertToScorch();

        // true si tiene Tremor y es Tremor - Scorch
        bool IsScorch() const;

        // Saca todo el estado, incluido el Scorch
        void Clear() override;

    private:
        // Si el Tremor se acabo, deja de ser Scorch (asi un Tremor nuevo arranca siendo comun)
        void EndScorchIfGone();

        bool isScorch;
    };
}

#endif // TREMOR_H
