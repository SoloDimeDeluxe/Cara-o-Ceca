#ifndef COIN_H
#define COIN_H

namespace ReyesCuadra
{
    // Moneda de una skill: si sale cara suma su poder, si sale cruz no suma nada
    class Coin
    {
    public:
        Coin(int coinPower, bool coinIsUnbreakable);

        // Poder que suma la moneda cuando sale cara
        int GetPower() const;

        // Las monedas irrompibles siguen pegando aunque la skill pierda el clash
        bool IsUnbreakable() const;

        // Tira la moneda: devuelve true si salio cara
        bool Flip() const;

    private:
        int power;
        bool isUnbreakable;
    };
}

#endif // COIN_H
