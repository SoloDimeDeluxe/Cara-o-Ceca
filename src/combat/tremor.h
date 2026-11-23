#ifndef TREMOR_H
#define TREMOR_H

#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Estado "Tremor" de Limbus: no hace dano, pero prepara el Stagger.
    //   Potencia: cuanto sube el umbral de Stagger de quien lo tiene cada vez que le hacen un "Tremor Burst".
    //   Cantidad: baja 1 al final de cada turno (y algunas skills la bajan al hacer Burst).
    // El Burst lo hace la Unit (porque es la que tiene los umbrales de Stagger); este estado solo guarda los numeros.
    class Tremor : public StatusEffect
    {
    public:
        // Al final del turno la cantidad baja 1
        void OnTurnEnd();

        // Saca 1 de cantidad (lo usan las skills que hacen Burst)
        void LoseOneCount();
    };
}

#endif // TREMOR_H
