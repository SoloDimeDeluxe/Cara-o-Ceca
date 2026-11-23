#ifndef SINKING_H
#define SINKING_H

#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Estado "Sinking" de Limbus: castiga a quien lo tiene cada vez que lo golpean.
    //   Potencia: cuanta cordura (SP) pierde en cada golpe.
    //   Cantidad: cuantos golpes mas lo activan. Baja 1 cada vez que se activa.
    // Si la unidad ya esta en la cordura minima (-45), en vez de cordura pierde vida (dano Gloom).
    class Sinking : public StatusEffect
    {
    public:
        // Se llama cuando la unidad recibe un golpe: devuelve cuanto tiene que perder (0 si no tiene Sinking)
        // y gasta 1 de cantidad. Quien lo llama decide si eso se resta de la cordura o de la vida.
        int Trigger();
    };
}

#endif // SINKING_H
