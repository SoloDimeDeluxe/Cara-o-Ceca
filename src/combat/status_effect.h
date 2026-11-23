#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

namespace ReyesCuadra
{
    // Base de los estados de Limbus (Poise, Sinking, etc.): todos tienen potencia y cantidad.
    //   Potencia: que tan fuerte es el efecto.
    //   Cantidad: cuantas veces mas se puede activar. Si llega a 0, el estado desaparece.
    // Las clases hijas (Poise, Sinking) agregan que hace cada uno.
    class StatusEffect
    {
    public:
        StatusEffect();
        virtual ~StatusEffect() = default;

        int GetPotency() const;
        int GetCount() const;

        // Devuelve true si el estado esta activo (potencia y cantidad mayores a 0)
        bool IsActive() const;

        // Suma potencia. Si no tenia el estado, arranca con 1 de cantidad (como en Limbus).
        void AddPotency(int amount);

        // Suma cantidad. Si no tenia el estado, arranca con 1 de potencia (como en Limbus).
        void AddCount(int amount);

        // Saca todo el estado (para empezar una partida nueva). Es virtual porque Tremor ademas apaga el Scorch.
        virtual void Clear();

    protected:
        // Baja la cantidad en 1 y, si llega a 0, borra el estado
        void ConsumeCount();

    private:
        int potency;
        int count;
    };
}

#endif // STATUS_EFFECT_H
