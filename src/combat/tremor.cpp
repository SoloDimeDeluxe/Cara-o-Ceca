#include "combat/tremor.h"

namespace ReyesCuadra
{
    Tremor::Tremor()
        : isScorch(false)
    {
    }

    void Tremor::OnTurnEnd()
    {
        ConsumeCount();
        EndScorchIfGone();
    }

    void Tremor::LoseOneCount()
    {
        ConsumeCount();
        EndScorchIfGone();
    }

    void Tremor::ConvertToScorch()
    {
        if (IsActive())
        {
            isScorch = true;
        }
    }

    bool Tremor::IsScorch() const
    {
        return isScorch && IsActive();
    }

    void Tremor::Clear()
    {
        StatusEffect::Clear();
        isScorch = false;
    }

    void Tremor::EndScorchIfGone()
    {
        if (!IsActive())
        {
            isScorch = false;
        }
    }
}
