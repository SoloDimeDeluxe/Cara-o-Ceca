#ifndef POISE_H
#define POISE_H

namespace ReyesCuadra
{
    // Estado "Poise" de Limbus: da chance de critico a los golpes.
    //   Potencia: cada punto da 5% de chance de critico (con 20 o mas, el critico es seguro).
    //   Cantidad: cuantos criticos mas puede dar. Baja 1 por cada critico y 1 al final de cada turno.
    // Si la cantidad llega a 0, el Poise desaparece (la potencia vuelve a 0).
    class Poise
    {
    public:
        Poise();

        int GetPotency() const;
        int GetCount() const;

        // Devuelve true si la unidad tiene Poise (potencia y cantidad mayores a 0)
        bool IsActive() const;

        // Chance de critico del proximo golpe (0 a 1)
        float GetCriticalChance() const;

        // Suma potencia. Si no tenia Poise, arranca con 1 de cantidad (como en Limbus).
        void AddPotency(int amount);

        // Suma cantidad. Si no tenia Poise, arranca con 1 de potencia (como en Limbus).
        void AddCount(int amount);

        // Tira si el golpe es critico. Si lo es, gasta 1 de cantidad y devuelve true.
        bool RollCritical();

        // Al final del turno la cantidad baja 1
        void OnTurnEnd();

        // Saca todo el Poise (para empezar una partida nueva)
        void Clear();

    private:
        // Si la cantidad llego a 0, se borra tambien la potencia
        void ClearIfEmpty();

        int potency;
        int count;
    };
}

#endif // POISE_H
