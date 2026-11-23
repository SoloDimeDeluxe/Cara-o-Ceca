#ifndef BURN_H
#define BURN_H

#include "combat/status_effect.h"

namespace ReyesCuadra
{
    // Estado "Burn" (Quemadura) de Limbus: quema a quien lo tiene al final de cada turno.
    //   Potencia: dano fijo que recibe al final del turno (no lo cambian el Stagger ni nada).
    //   Cantidad: cuantos turnos mas quema. Baja 1 despues de cada quemada.
    class Burn : public StatusEffect
    {
    public:
        // Se llama al final del turno: devuelve el dano que tiene que recibir la unidad (0 si no tiene Burn)
        // y despues baja 1 la cantidad.
        int OnTurnEnd();
    };
}

#endif // BURN_H
