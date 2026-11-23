#include "combat/tremor.h"

namespace ReyesCuadra
{
    void Tremor::OnTurnEnd()
    {
        ConsumeCount();
    }

    void Tremor::LoseOneCount()
    {
        ConsumeCount();
    }
}
