#include "combat/clash.h"

#include <cmath>
#include <string>

namespace ReyesCuadra
{
    // Tope de rondas de un clash, para que nunca quede trabado si empatan muchas veces seguidas
    static const int MAX_CLASH_ROUNDS = 99;

    // Cantidad de clashes de prueba que se simulan para calcular el porcentaje de victoria
    static const int WIN_CHANCE_SIMULATIONS = 1000;

    // Un golpe critico hace este multiplicador de dano (+20%)
    static const float CRITICAL_DAMAGE_MULTIPLIER = 1.2f;

    // Bonus de poder de moneda por Poise: +1 cada 6 de potencia, hasta +2
    static const int POISE_PER_COIN_POWER = 6;
    static const int MAX_POISE_COIN_POWER_BONUS = 2;

    // Resultado de tirar las monedas de un clash (sin aplicar el dano)
    struct ClashOutcome
    {
        bool playerWon;
        int winnerCoinsLeft;
        int rounds;
    };

    // Resultado de un ataque: cuanto dano hizo y cuantos golpes fueron criticos o se esquivaron
    struct AttackResult
    {
        int damage = 0;
        int criticalHits = 0;
        int evadedHits = 0;
    };

    // Devuelve cuanto poder extra suma cada moneda de la skill segun la potencia de Poise indicada
    static int GetCoinPowerBonus(const Skill& skill, int poisePotency);

    // Igual que la anterior pero "adivinando" la potencia que va a tener la unidad despues del [Al usarla] de la skill.
    // Se usa en las simulaciones, que no pueden cambiar el Poise de verdad.
    static int PredictCoinPowerBonus(const Unit& unit, const Skill& skill);

    // Aplica los efectos [Al usarla] de la skill sobre quien la usa (por ejemplo, ganar Poise)
    static void ApplyOnUseEffects(Unit& user, const Skill& skill);

    // Tira las primeras "coinsToFlip" monedas de la skill y devuelve el poder total (base + caras * (poder + bonus))
    static int RollClashPower(const Skill& skill, int coinsToFlip, int coinPowerBonus);

    // Hace las rondas del clash: cada ronda tiran todas las monedas que les quedan y el que saca menos pierde una moneda.
    // Si empatan, se repite. Termina cuando a uno no le quedan monedas.
    static ClashOutcome SimulateClash(const Skill& playerSkill, int playerBonus, const Skill& enemySkill, int enemyBonus);

    // Ataca al objetivo con las primeras "coinsToUse" monedas de la skill. El poder va subiendo moneda por moneda
    // (base, base + primera cara, etc.) y cada moneda hace tanto dano como el poder que hay en ese momento.
    // Cada golpe puede ser critico segun el Poise del atacante, y aplica los efectos [Al golpear].
    // Si onlyUnbreakable es true, solo pegan las monedas irrompibles.
    // Si evadePower es mayor o igual a 0, el objetivo esta esquivando: solo pegan los golpes que superan ese numero.
    static AttackResult PerformAttack(Unit& attacker, const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable, int evadePower);

    // Cuenta cuantas monedas irrompibles tiene la skill
    static int CountUnbreakableCoins(const Skill& skill);

    // Arma el texto " (2 criticos)" para el registro, o nada si no hubo criticos
    static std::string DescribeCriticals(int criticalHits);

    // Ataque sin oposicion: la skill pega con todas sus monedas (las defensas solas no hacen nada)
    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatLog& log);

    // Clash entre dos skills: tiran monedas hasta que a una se le acaban, y la que gana pega con lo que le queda
    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatLog& log);

    // Devuelve true si "evadeSkill" es una esquiva y "otherSkill" es algo que pega (ataque o contraataque)
    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill);

    // Simula una esquiva sin hacer dano: devuelve true si se esquivaron todos los golpes
    static bool SimulateFullEvade(const Skill& evadeSkill, int evadeBonus, const Skill& attackSkill, int attackBonus);

    // Resuelve la esquiva de verdad: la esquiva NO es un clash y no rompe monedas. Se tira una sola vez y
    // el atacante pega moneda por moneda; solo hacen dano los golpes cuyo poder supera al de la esquiva.
    static void ResolveEvade(Unit& evader, const Skill& evadeSkill, Unit& attacker, const Skill& attackSkill, CombatLog& log);

    void ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatLog& log)
    {
        // Lo primero es el [Al usarla] de cada skill, asi el Poise ganado ya cuenta para el clash
        if (playerSkill != nullptr)
        {
            ApplyOnUseEffects(player, *playerSkill);
        }

        if (enemySkill != nullptr)
        {
            ApplyOnUseEffects(enemy, *enemySkill);
        }

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

    float EstimateClashWinChance(const Unit& player, const Skill& playerSkill, const Unit& enemy, const Skill& enemySkill)
    {
        const int playerBonus = PredictCoinPowerBonus(player, playerSkill);
        const int enemyBonus = PredictCoinPowerBonus(enemy, enemySkill);
        int playerWins = 0;

        for (int simulation = 0; simulation < WIN_CHANCE_SIMULATIONS; simulation++)
        {
            bool playerWon = false;

            if (IsEvadeAgainstAttack(playerSkill, enemySkill))
            {
                // Si el jugador esquiva, "ganar" es esquivar todos los golpes
                playerWon = SimulateFullEvade(playerSkill, playerBonus, enemySkill, enemyBonus);
            }
            else if (IsEvadeAgainstAttack(enemySkill, playerSkill))
            {
                // Si el enemigo esquiva, "ganar" es que al menos un golpe del jugador entre
                playerWon = !SimulateFullEvade(enemySkill, enemyBonus, playerSkill, playerBonus);
            }
            else
            {
                playerWon = SimulateClash(playerSkill, playerBonus, enemySkill, enemyBonus).playerWon;
            }

            if (playerWon)
            {
                playerWins++;
            }
        }

        return static_cast<float>(playerWins) / static_cast<float>(WIN_CHANCE_SIMULATIONS);
    }

    static int GetCoinPowerBonus(const Skill& skill, int poisePotency)
    {
        if (!skill.GetPoiseEffects().gainsCoinPowerFromPoise)
        {
            return 0;
        }

        const int bonus = poisePotency / POISE_PER_COIN_POWER;

        return (bonus > MAX_POISE_COIN_POWER_BONUS) ? MAX_POISE_COIN_POWER_BONUS : bonus;
    }

    static int PredictCoinPowerBonus(const Unit& unit, const Skill& skill)
    {
        const PoiseEffects& effects = skill.GetPoiseEffects();
        int predictedPotency = unit.GetPoise().GetPotency() + effects.onUsePotency;

        // Si gana cantidad sin tener Poise, arranca con 1 de potencia
        if (predictedPotency == 0 && effects.onUseCount > 0)
        {
            predictedPotency = 1;
        }

        return GetCoinPowerBonus(skill, predictedPotency);
    }

    static void ApplyOnUseEffects(Unit& user, const Skill& skill)
    {
        const PoiseEffects& effects = skill.GetPoiseEffects();

        if (effects.onUsePotency > 0)
        {
            user.GetPoise().AddPotency(effects.onUsePotency);
        }

        if (effects.onUseCount > 0)
        {
            user.GetPoise().AddCount(effects.onUseCount);
        }
    }

    static int RollClashPower(const Skill& skill, int coinsToFlip, int coinPowerBonus)
    {
        const std::vector<Coin>& coins = skill.GetCoins();
        int power = skill.GetBasePower();

        for (int coinIndex = 0; coinIndex < coinsToFlip; coinIndex++)
        {
            if (coins[static_cast<size_t>(coinIndex)].Flip())
            {
                power += coins[static_cast<size_t>(coinIndex)].GetPower() + coinPowerBonus;
            }
        }

        return power;
    }

    static ClashOutcome SimulateClash(const Skill& playerSkill, int playerBonus, const Skill& enemySkill, int enemyBonus)
    {
        int playerCoins = playerSkill.GetCoinCount();
        int enemyCoins = enemySkill.GetCoinCount();
        int rounds = 0;

        while (playerCoins > 0 && enemyCoins > 0 && rounds < MAX_CLASH_ROUNDS)
        {
            const int playerPower = RollClashPower(playerSkill, playerCoins, playerBonus);
            const int enemyPower = RollClashPower(enemySkill, enemyCoins, enemyBonus);

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

    static AttackResult PerformAttack(Unit& attacker, const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable, int evadePower)
    {
        const std::vector<Coin>& coins = skill.GetCoins();
        const PoiseEffects& effects = skill.GetPoiseEffects();
        AttackResult result;
        int power = skill.GetBasePower();

        for (int coinIndex = 0; coinIndex < coinsToUse && !target.IsDead(); coinIndex++)
        {
            const Coin& coin = coins[static_cast<size_t>(coinIndex)];

            if (onlyUnbreakable && !coin.IsUnbreakable())
            {
                continue;
            }

            // El bonus se recalcula en cada moneda porque el Poise puede subir golpe a golpe
            if (coin.Flip())
            {
                power += coin.GetPower() + GetCoinPowerBonus(skill, attacker.GetPoise().GetPotency());
            }

            // Si el objetivo esquiva, para pegar hay que superar su numero (si empata, se esquiva)
            if (evadePower >= 0 && power <= evadePower)
            {
                result.evadedHits++;
                continue;
            }

            int hitDamage = (power > 0) ? power : 0;

            if (attacker.GetPoise().RollCritical())
            {
                hitDamage = static_cast<int>(std::round(static_cast<float>(hitDamage) * CRITICAL_DAMAGE_MULTIPLIER));
                result.criticalHits++;

                if (effects.onCriticalCount > 0)
                {
                    attacker.GetPoise().AddCount(effects.onCriticalCount);
                }
            }

            target.TakeDamage(hitDamage);
            result.damage += hitDamage;

            // Efectos [Al golpear]
            if (effects.onHitPotency > 0)
            {
                attacker.GetPoise().AddPotency(effects.onHitPotency);
            }

            if (effects.onHitCount > 0)
            {
                attacker.GetPoise().AddCount(effects.onHitCount);
            }
        }

        return result;
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

    static std::string DescribeCriticals(int criticalHits)
    {
        if (criticalHits <= 0)
        {
            return "";
        }

        return (criticalHits == 1) ? " (1 crítico)" : " (" + std::to_string(criticalHits) + " críticos)";
    }

    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatLog& log)
    {
        // Esquivar o contraatacar sin nadie enfrente no hace nada
        if (skill.IsDefensive())
        {
            log.AddLine(attacker.GetName() + " se queda en guardia con " + skill.GetShortName() + ", pero nadie lo ataca.");
            return;
        }

        const AttackResult result = PerformAttack(attacker, skill, skill.GetCoinCount(), target, false, -1);

        log.AddLine(attacker.GetName() + " ataca sin oposición con " + skill.GetShortName() + ": " + std::to_string(result.damage) + " de daño" + DescribeCriticals(result.criticalHits) + ".");
    }

    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatLog& log)
    {
        // Dos defensas enfrentadas no pelean entre si
        if (playerSkill.IsDefensive() && enemySkill.IsDefensive())
        {
            log.AddLine(player.GetName() + " y " + enemy.GetName() + " se quedan a la defensiva. No pasa nada.");
            return;
        }

        const int playerBonus = GetCoinPowerBonus(playerSkill, player.GetPoise().GetPotency());
        const int enemyBonus = GetCoinPowerBonus(enemySkill, enemy.GetPoise().GetPotency());
        const ClashOutcome outcome = SimulateClash(playerSkill, playerBonus, enemySkill, enemyBonus);

        Unit& winner = outcome.playerWon ? player : enemy;
        Unit& loser = outcome.playerWon ? enemy : player;
        const Skill& winnerSkill = outcome.playerWon ? playerSkill : enemySkill;
        const Skill& loserSkill = outcome.playerWon ? enemySkill : playerSkill;

        // Se explica en criollo que paso: quien gano, cuantas monedas le quedaron y cuanto pego
        std::string line = winner.GetName() + " gana el clash (" + winnerSkill.GetShortName() + " vs " + loserSkill.GetShortName() + "): le quedan " + std::to_string(outcome.winnerCoinsLeft);

        line += (outcome.winnerCoinsLeft == 1) ? " moneda" : " monedas";

        if (winnerSkill.DealsDamageOnWin())
        {
            const AttackResult result = PerformAttack(winner, winnerSkill, outcome.winnerCoinsLeft, loser, false, -1);

            line += " y pega " + std::to_string(result.damage) + " de daño" + DescribeCriticals(result.criticalHits) + ".";
        }
        else
        {
            line += " y esquiva el ataque.";
        }

        log.AddLine(line);

        // Las monedas irrompibles del que perdio pegan igual despues del ganador (si sigue vivo)
        if (!loser.IsDead() && !loserSkill.IsDefensive() && CountUnbreakableCoins(loserSkill) > 0)
        {
            const AttackResult result = PerformAttack(loser, loserSkill, loserSkill.GetCoinCount(), winner, true, -1);

            log.AddLine("   Las monedas irrompibles de " + loser.GetName() + " pegan igual: " + std::to_string(result.damage) + " de daño" + DescribeCriticals(result.criticalHits) + ".");
        }
    }

    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill)
    {
        return evadeSkill.GetType() == SkillType::Evade && otherSkill.DealsDamageOnWin();
    }

    static bool SimulateFullEvade(const Skill& evadeSkill, int evadeBonus, const Skill& attackSkill, int attackBonus)
    {
        const int evadePower = RollClashPower(evadeSkill, evadeSkill.GetCoinCount(), evadeBonus);
        int power = attackSkill.GetBasePower();

        for (const Coin& coin : attackSkill.GetCoins())
        {
            if (coin.Flip())
            {
                power += coin.GetPower() + attackBonus;
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
        // La esquiva se tira una sola vez
        const int evadeBonus = GetCoinPowerBonus(evadeSkill, evader.GetPoise().GetPotency());
        const int evadePower = RollClashPower(evadeSkill, evadeSkill.GetCoinCount(), evadeBonus);
        const AttackResult result = PerformAttack(attacker, attackSkill, attackSkill.GetCoinCount(), evader, false, evadePower);

        std::string line = evader.GetName() + " esquiva (" + evadeSkill.GetShortName() + ") con " + std::to_string(evadePower) + ": evita " + std::to_string(result.evadedHits) + " de " + std::to_string(attackSkill.GetCoinCount()) + " golpes de " + attacker.GetName() + " (" + attackSkill.GetShortName() + ")";

        if (result.damage > 0)
        {
            line += " y recibe " + std::to_string(result.damage) + " de daño" + DescribeCriticals(result.criticalHits) + ".";
        }
        else
        {
            line += ". ¡No le hacen nada!";
        }

        log.AddLine(line);
    }
}
