#include "combat/clash.h"

#include <string>

namespace ReyesCuadra
{
    // Tope de rondas de un clash, para que nunca quede trabado si empatan muchas veces seguidas
    static const int MAX_CLASH_ROUNDS = 99;

    // Tira las primeras "coinsToFlip" monedas de la skill y devuelve el poder total (base + caras)
    static int RollClashPower(const Skill& skill, int coinsToFlip);

    // Ataca al objetivo con las primeras "coinsToUse" monedas de la skill. El poder va subiendo moneda por moneda
    // (base, base + primera cara, etc.) y cada moneda hace tanto dano como el poder que hay en ese momento.
    // Si onlyUnbreakable es true, solo pegan las monedas irrompibles. Devuelve el dano total que hizo.
    static int PerformAttack(const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable);

    // Cuenta cuantas monedas irrompibles tiene la skill
    static int CountUnbreakableCoins(const Skill& skill);

    // Ataque sin oposicion: la skill pega con todas sus monedas (las defensas solas no hacen nada)
    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatLog& log);

    // Clash entre dos skills: tiran monedas hasta que a una se le acaban, y la que gana pega con lo que le queda
    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatLog& log);

    void ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatLog& log)
    {
        if (playerSkill != nullptr && enemySkill != nullptr)
        {
            ResolveClash(player, *playerSkill, enemy, *enemySkill, log);
        }
        else if (playerSkill != nullptr)
        {
            ResolveOneSidedAction(player, *playerSkill, enemy, log);
        }
        else if (enemySkill != nullptr)
        {
            ResolveOneSidedAction(enemy, *enemySkill, player, log);
        }
    }

    static int RollClashPower(const Skill& skill, int coinsToFlip)
    {
        const std::vector<Coin>& coins = skill.GetCoins();
        int power = skill.GetBasePower();

        for (int coinIndex = 0; coinIndex < coinsToFlip; coinIndex++)
        {
            if (coins[static_cast<size_t>(coinIndex)].Flip())
            {
                power += coins[static_cast<size_t>(coinIndex)].GetPower();
            }
        }

        return power;
    }

    static int PerformAttack(const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable)
    {
        const std::vector<Coin>& coins = skill.GetCoins();
        int power = skill.GetBasePower();
        int totalDamage = 0;

        for (int coinIndex = 0; coinIndex < coinsToUse && !target.IsDead(); coinIndex++)
        {
            const Coin& coin = coins[static_cast<size_t>(coinIndex)];

            if (onlyUnbreakable && !coin.IsUnbreakable())
            {
                continue;
            }

            if (coin.Flip())
            {
                power += coin.GetPower();
            }

            // El poder nunca hace dano negativo
            const int hitDamage = (power > 0) ? power : 0;

            target.TakeDamage(hitDamage);
            totalDamage += hitDamage;
        }

        return totalDamage;
    }

    static int CountUnbreakableCoins(const Skill& skill)
    {
        int unbreakableCount = 0;

        for (const Coin& coin : skill.GetCoins())
        {
            if (coin.IsUnbreakable())
            {
                unbreakableCount++;
            }
        }

        return unbreakableCount;
    }

    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatLog& log)
    {
        // Esquivar o contraatacar sin nadie enfrente no hace nada
        if (skill.IsDefensive())
        {
            log.AddLine(attacker.GetName() + " se queda en guardia con " + skill.GetShortName() + ", pero nadie lo ataca.");
            return;
        }

        const int damage = PerformAttack(skill, skill.GetCoinCount(), target, false);

        log.AddLine(attacker.GetName() + " ataca sin oposición con " + skill.GetShortName() + ": " + std::to_string(damage) + " de daño.");
    }

    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatLog& log)
    {
        // Dos defensas enfrentadas no pelean entre si
        if (playerSkill.IsDefensive() && enemySkill.IsDefensive())
        {
            log.AddLine(player.GetName() + " y " + enemy.GetName() + " se quedan a la defensiva. No pasa nada.");
            return;
        }

        int playerCoins = playerSkill.GetCoinCount();
        int enemyCoins = enemySkill.GetCoinCount();
        int rounds = 0;

        // Cada ronda tiran todas las monedas que les quedan; el que saca menos pierde una moneda. Si empatan se repite.
        while (playerCoins > 0 && enemyCoins > 0 && rounds < MAX_CLASH_ROUNDS)
        {
            const int playerPower = RollClashPower(playerSkill, playerCoins);
            const int enemyPower = RollClashPower(enemySkill, enemyCoins);

            if (playerPower > enemyPower)
            {
                enemyCoins--;
            }
            else if (enemyPower > playerPower)
            {
                playerCoins--;
            }

            rounds++;
        }

        const bool playerWon = playerCoins > 0;

        Unit& winner = playerWon ? player : enemy;
        Unit& loser = playerWon ? enemy : player;
        const Skill& winnerSkill = playerWon ? playerSkill : enemySkill;
        const Skill& loserSkill = playerWon ? enemySkill : playerSkill;
        const int winnerCoinsLeft = playerWon ? playerCoins : enemyCoins;

        std::string line = winner.GetName() + " (" + winnerSkill.GetShortName() + ") gana el clash a " + loserSkill.GetShortName() + " en " + std::to_string(rounds) + " rondas";

        if (winnerSkill.DealsDamageOnWin())
        {
            const int damage = PerformAttack(winnerSkill, winnerCoinsLeft, loser, false);

            line += " y pega " + std::to_string(damage) + " de daño.";
        }
        else
        {
            line += " y esquiva el ataque.";
        }

        log.AddLine(line);

        // Las monedas irrompibles del que perdio pegan igual despues del ganador (si sigue vivo)
        if (!loser.IsDead() && !loserSkill.IsDefensive() && CountUnbreakableCoins(loserSkill) > 0)
        {
            const int damage = PerformAttack(loserSkill, loserSkill.GetCoinCount(), winner, true);

            log.AddLine("   Las monedas irrompibles de " + loser.GetName() + " pegan igual: " + std::to_string(damage) + " de daño.");
        }
    }
}
