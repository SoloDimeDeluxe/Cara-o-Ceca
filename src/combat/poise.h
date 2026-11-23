#ifndef POISE_H
#define POISE_H

#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Estado "Poise" de Limbus: da chance de critico a los golpes de quien lo tiene.
    //   Potencia: cada punto da 5% de chance de critico (con 20 o mas, el critico es seguro).
    //   Cantidad: baja 1 por cada critico y 1 al final de cada turno.
    class Poise : public StatusEffect
    {
    public:
        // Chance de critico del proximo golpe (0 a 1)
        float GetCriticalChance() const;

        // Tira si el golpe es critico. Si lo es, gasta 1 de cantidad y devuelve true.
        bool RollCritical();

        // Al final del turno la cantidad baja 1
        void OnTurnEnd();
    };
}

#endif // POISE_H
