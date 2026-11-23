#include "combat/clash.h"

#include <string>

namespace ReyesCuadra
{
    // Tope de rondas de un clash, para que nunca quede trabado si empatan muchas veces seguidas
    static const int MAX_CLASH_ROUNDS = 99;

    // Cantidad de clashes de prueba que se simulan para calcular el porcentaje de victoria
    static const int WIN_CHANCE_SIMULATIONS = 1000;

    // Resultado de tirar las monedas de un clash (sin aplicar el dano)
    struct ClashOutcome
    {
        bool playerWon;
        int winnerCoinsLeft;
        int rounds;
    };

    // Tira las primeras "coinsToFlip" monedas de la skill y devuelve el poder total (base + caras)
    static int RollClashPower(const Skill& skill, int coinsToFlip);

    // Hace las rondas del clash: cada ronda tiran todas las monedas que les quedan y el que saca menos pierde una moneda.
    // Si empatan, se repite. Termina cuando a uno no le quedan monedas.
    static ClashOutcome SimulateClash(const Skill& playerSkill, const Skill& enemySkill);

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

    // Devuelve true si "evadeSkill" es una esquiva y "otherSkill" es algo que pega (ataque o contraataque)
    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill);

    // Tira una sola vez las monedas de la esquiva y devuelve el numero que hay que superar para pegarle
    static int RollEvadePower(const Skill& evadeSkill);

    // Hace el ataque contra alguien que esquiva: la esquiva NO es un clash y no rompe monedas.
    // El atacante pega moneda por moneda como siempre, pero solo hacen dano los golpes cuyo poder supera al de la esquiva.
    // Devuelve el dano total y guarda en evadedHits cuantos golpes se esquivaron.
    static int PerformAttackAgainstEvade(const Skill& attackSkill, int evadePower, Unit& target, int& evadedHits);

    // Simula una esquiva sin hacer dano: devuelve true si se esquivaron todos los golpes
    static bool SimulateFullEvade(const Skill& evadeSkill, const Skill& attackSkill);

    // Resuelve la esquiva de verdad (con dano) y lo anota en el registro
    static void ResolveEvade(Unit& evader, const Skill& evadeSkill, Unit& attacker, const Skill& attackSkill, CombatLog& log);

    void ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatLog& log)
    {
        if (playerSkill != nullptr && enemySkill != nullptr)
        {
            // La esquiva no clashea: el otro ataca igual y se ve cuantos golpes se esquivan
            if (IsEvadeAgainstAttack(*playerSkill, *enemySkill))
            {
                ResolveEvade(player, *playerSkill, enemy, *enemySkill, log);
            }
            else if (IsEvadeAgainstAttack(*enemySkill, *playerSkill))
            {
                ResolveEvade(enemy, *enemySkill, player, *playerSkill, log);
            }
            else
            {
                ResolveClash(player, *playerSkill, enemy, *enemySkill, log);
            }
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

    float EstimateClashWinChance(const Skill& playerSkill, const Skill& enemySkill)
    {
        int playerWins = 0;

        for (int simulation = 0; simulation < WIN_CHANCE_SIMULATIONS; simulation++)
        {
            bool playerWon = false;

            if (IsEvadeAgainstAttack(playerSkill, enemySkill))
            {
                // Si el jugador esquiva, "ganar" es esquivar todos los golpes
                playerWon = SimulateFullEvade(playerSkill, enemySkill);
            }
            else if (IsEvadeAgainstAttack(enemySkill, playerSkill))
            {
                // Si el enemigo esquiva, "ganar" es que al menos un golpe del jugador entre
                playerWon = !SimulateFullEvade(enemySkill, playerSkill);
            }
            else
            {
                playerWon = SimulateClash(playerSkill, enemySkill).playerWon;
            }

            if (playerWon)
            {
                playerWins++;
            }
        }

        return static_cast<float>(playerWins) / static_cast<float>(WIN_CHANCE_SIMULATIONS);
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

    static ClashOutcome SimulateClash(const Skill& playerSkill, const Skill& enemySkill)
    {
        int playerCoins = playerSkill.GetCoinCount();
        int enemyCoins = enemySkill.GetCoinCount();
        int rounds = 0;

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

        ClashOutcome outcome;
        outcome.playerWon = playerCoins > 0;
        outcome.winnerCoinsLeft = outcome.playerWon ? playerCoins : enemyCoins;
        outcome.rounds = rounds;

        return outcome;
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

        const ClashOutcome outcome = SimulateClash(playerSkill, enemySkill);

        Unit& winner = outcome.playerWon ? player : enemy;
        Unit& loser = outcome.playerWon ? enemy : player;
        const Skill& winnerSkill = outcome.playerWon ? playerSkill : enemySkill;
        const Skill& loserSkill = outcome.playerWon ? enemySkill : playerSkill;

        // Se explica en criollo que paso: quien gano, cuantas monedas le quedaron y cuanto pego
        std::string line = winner.GetName() + " gana el clash (" + winnerSkill.GetShortName() + " vs " + loserSkill.GetShortName() + "): le quedan " + std::to_string(outcome.winnerCoinsLeft);

        line += (outcome.winnerCoinsLeft == 1) ? " moneda" : " monedas";

        if (winnerSkill.DealsDamageOnWin())
        {
            const int damage = PerformAttack(winnerSkill, outcome.winnerCoinsLeft, loser, false);

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

    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill)
    {
        return evadeSkill.GetType() == SkillType::Evade && otherSkill.DealsDamageOnWin();
    }

    static int RollEvadePower(const Skill& evadeSkill)
    {
        return RollClashPower(evadeSkill, evadeSkill.GetCoinCount());
    }

    static int PerformAttackAgainstEvade(const Skill& attackSkill, int evadePower, Unit& target, int& evadedHits)
    {
        int power = attackSkill.GetBasePower();
        int totalDamage = 0;

        evadedHits = 0;

        for (const Coin& coin : attackSkill.GetCoins())
        {
            if (target.IsDead())
            {
                break;
            }

            if (coin.Flip())
            {
                power += coin.GetPower();
            }

            // Para pegar hay que superar el numero de la esquiva (si empata, se esquiva)
            if (power > evadePower)
            {
                target.TakeDamage(power);
                totalDamage += power;
            }
            else
            {
                evadedHits++;
            }
        }

        return totalDamage;
    }

    static bool SimulateFullEvade(const Skill& evadeSkill, const Skill& attackSkill)
    {
        const int evadePower = RollEvadePower(evadeSkill);
        int power = attackSkill.GetBasePower();

        for (const Coin& coin : attackSkill.GetCoins())
        {
            if (coin.Flip())
            {
                power += coin.GetPower();
            }

            if (power > evadePower)
            {
                return false;
            }
        }

        return true;
    }

    static void ResolveEvade(Unit& evader, const Skill& evadeSkill, Unit& attacker, const Skill& attackSkill, CombatLog& log)
    {
        const int evadePower = RollEvadePower(evadeSkill);
        int evadedHits = 0;
        const int damage = PerformAttackAgainstEvade(attackSkill, evadePower, evader, evadedHits);
        const int totalHits = attackSkill.GetCoinCount();

        std::string line = evader.GetName() + " esquiva (" + evadeSkill.GetShortName() + ") con " + std::to_string(evadePower) + ": evita " + std::to_string(evadedHits) + " de " + std::to_string(totalHits) + " golpes de " + attacker.GetName() + " (" + attackSkill.GetShortName() + ")";

        if (damage > 0)
        {
            line += " y recibe " + std::to_string(damage) + " de daño.";
        }
        else
        {
            line += ". ¡No le hacen nada!";
        }

        log.AddLine(line);
    }
}
