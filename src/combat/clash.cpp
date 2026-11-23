#include "combat/clash.h"

#include <cmath>
#include <string>
#include <vector>

namespace ReyesCuadra
{
    // Tope de rondas de un clash, para que nunca quede trabado si empatan muchas veces seguidas
    static const int MAX_CLASH_ROUNDS = 99;

    // Cantidad de clashes de prueba que se simulan para calcular el porcentaje de victoria
    static const int WIN_CHANCE_SIMULATIONS = 1000;

    // Un golpe critico hace este multiplicador de dano (+20%)
    static const float CRITICAL_DAMAGE_MULTIPLIER = 1.2f;

    // Una unidad staggereada recibe el doble de dano de los golpes
    static const float STAGGERED_DAMAGE_MULTIPLIER = 2.0f;

    // Cordura que gana quien gana un clash: base 10, +20% por cada ronda despues de la primera (redondeado para abajo)
    static const int SANITY_PER_CLASH_WIN = 10;
    static const float SANITY_BONUS_PER_EXTRA_ROUND = 0.2f;

    // Bonus de poder de moneda: +1 cada 6 de (potencia de Poise propio + potencia de Sinking del objetivo), hasta +2
    static const int POISE_PER_COIN_POWER = 6;
    static const int MAX_POISE_COIN_POWER_BONUS = 2;

    // Un lado del clash: que skill usa, cuanto poder extra suma cada cara y que chance tiene de sacar cara
    struct ClashSide
    {
        const Skill* skill;
        int basePowerBonus;
        int coinPowerBonus;
        float headsChance;
    };

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
        int sinkingDamage = 0;
        int sinkingSanityLoss = 0;
        int tremorBurstRaise = 0;
        int criticalHits = 0;
        int evadedHits = 0;
    };

    // Devuelve cuanto poder extra suma cada moneda de la skill segun el Poise de quien la usa y el Sinking del objetivo
    static int GetCoinPowerBonus(const Skill& skill, const Unit& user, const Unit& target);

    // Igual que la anterior pero "adivinando" el Poise que va a tener la unidad despues del [Al usarla] de la skill.
    // Se usa en las simulaciones, que no pueden cambiar los estados de verdad.
    static int PredictCoinPowerBonus(const Unit& user, const Skill& skill, const Unit& target);

    // Calcula el bonus a partir de los numeros sueltos (+1 cada 6, maximo +2)
    static int CalculateCoinPowerBonus(const Skill& skill, int poisePotency, int sinkingPotency);

    // Suma de la potencia de Burn y de Tremor del objetivo (varias skills de Lei Heng escalan con eso)
    static int GetBurnPlusTremor(const Unit& target);

    // Bonus de poder de moneda por el (Burn + Tremor) del objetivo (Tajo Destrozador)
    static int CalculateStatusCoinPowerBonus(const Skill& skill, const Unit& target);

    // Bonus de poder final por el (Burn + Tremor) del objetivo (Doble Corte y Triple Corte - Explosion)
    static int GetBasePowerBonus(const Skill& skill, const Unit& target);

    // Aplica el Burn y el Tremor de la moneda indicada sobre el objetivo y, si corresponde, le hace Tremor Burst.
    // Devuelve cuantos puntos subio el umbral de Stagger por los Burst (0 si no hubo).
    static int ApplyOnHitTargetStatuses(const Skill& skill, size_t coinIndex, Unit& target);

    // Aplica los efectos [Al golpear] de Sinking de la skill sobre el objetivo
    static void ApplyOnHitSinking(const Skill& skill, Unit& target);

    // Devuelve true si el golpe hizo "Cracking". Todavia no existe ese sistema, asi que por ahora siempre es false
    // (todos los golpes cuentan como "sin Cracking"). Cuando se agregue, se cambia solo aca.
    static bool DidHitCrack();

    // Aplica los efectos [Al usarla] de la skill sobre quien la usa (por ejemplo, ganar Poise)
    static void ApplyOnUseEffects(Unit& user, const Skill& skill);

    // Tira las primeras "coinsToFlip" monedas del lado y devuelve el poder total (base + caras * (poder + bonus))
    // Si coinFaces no es nullptr, ahi se anota como cayo cada moneda (true = cara), para mostrarlo en pantalla
    static int RollClashPower(const ClashSide& side, int coinsToFlip, std::vector<bool>* coinFaces);

    // Hace las rondas del clash: cada ronda tiran todas las monedas que les quedan y el que saca menos pierde una moneda.
    // Si empatan, se repite. Termina cuando a uno no le quedan monedas.
    // Si timeline no es nullptr, se anota cada ronda (en el calculo de porcentajes se pasa nullptr)
    static ClashOutcome SimulateClash(const ClashSide& playerSide, const ClashSide& enemySide, CombatTimeline* timeline);

    // Cuanta cordura gana quien gana un clash que duro esa cantidad de rondas
    static int GetClashWinSanity(int rounds);

    // Ataca al objetivo con las primeras "coinsToUse" monedas de la skill. El poder va subiendo moneda por moneda
    // (base, base + primera cara, etc.) y cada moneda hace tanto dano como el poder que hay en ese momento.
    // Cada golpe puede ser critico segun el Poise del atacante, y aplica los efectos [Al golpear].
    // Si onlyUnbreakable es true, solo pegan las monedas irrompibles.
    // Si evadePower es mayor o igual a 0, el objetivo esta esquivando: solo pegan los golpes que superan ese numero.
    // Cada golpe, Stagger y Tremor Burst se anota en la linea de tiempo.
    static AttackResult PerformAttack(Unit& attacker, const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable, int evadePower, CombatTimeline& timeline);

    // Si el objetivo acaba de quedar staggereado, lo anota (cartel grande y linea en el registro)
    static void RecordStaggerIfAny(Unit& target, CombatTimeline& timeline);

    // Cuenta cuantas monedas irrompibles tiene la skill
    static int CountUnbreakableCoins(const Skill& skill);

    // Arma el texto extra del registro: " (+6 de Sinking, 2 criticos)", o nada si no hubo ninguno de los dos
    static std::string DescribeExtras(const AttackResult& result);

    // Ataque sin oposicion: la skill pega con todas sus monedas (las defensas solas no hacen nada)
    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatTimeline& timeline);

    // Clash entre dos skills: tiran monedas hasta que a una se le acaban, y la que gana pega con lo que le queda
    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatTimeline& timeline);

    // Devuelve true si "evadeSkill" es una esquiva y "otherSkill" es algo que pega (ataque o contraataque)
    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill);

    // Simula una esquiva sin hacer dano: devuelve true si se esquivaron todos los golpes
    static bool SimulateFullEvade(const ClashSide& evadeSide, const ClashSide& attackSide);

    // Resuelve la esquiva de verdad: la esquiva NO es un clash y no rompe monedas. Se tira una sola vez y
    // el atacante pega moneda por moneda; solo hacen dano los golpes cuyo poder supera al de la esquiva.
    // Devuelve true si se esquivaron todos los golpes (la esquiva sigue en pie).
    static bool ResolveEvade(Unit& evader, const Skill& evadeSkill, Unit& attacker, const Skill& attackSkill, CombatTimeline& timeline);

    bool ResolveColumn(Unit& player, const Skill* playerSkill, Unit& enemy, const Skill* enemySkill, CombatTimeline& timeline)
    {
        // Una esquiva del jugador sigue en pie salvo que la ataquen y algun golpe la supere
        const bool playerIsEvading = playerSkill != nullptr && playerSkill->GetType() == SkillType::Evade;
        bool playerEvadeHeld = playerIsEvading;

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
                playerEvadeHeld = ResolveEvade(player, *playerSkill, enemy, *enemySkill, timeline);
            }
            else if (IsEvadeAgainstAttack(*enemySkill, *playerSkill))
            {
                ResolveEvade(enemy, *enemySkill, player, *playerSkill, timeline);
            }
            else
            {
                ResolveClash(player, *playerSkill, enemy, *enemySkill, timeline);
            }
        }
        else if (playerSkill != nullptr)
        {
            ResolveOneSidedAction(player, *playerSkill, enemy, timeline);
        }
        else if (enemySkill != nullptr)
        {
            ResolveOneSidedAction(enemy, *enemySkill, player, timeline);
        }

        return playerEvadeHeld && !player.IsDead() && !player.IsStaggered();
    }

    float EstimateClashWinChance(const Unit& player, const Skill& playerSkill, const Unit& enemy, const Skill& enemySkill)
    {
        const ClashSide playerSide = { &playerSkill, GetBasePowerBonus(playerSkill, enemy), PredictCoinPowerBonus(player, playerSkill, enemy), player.GetHeadsChance() };
        const ClashSide enemySide = { &enemySkill, GetBasePowerBonus(enemySkill, player), PredictCoinPowerBonus(enemy, enemySkill, player), enemy.GetHeadsChance() };
        int playerWins = 0;

        for (int simulation = 0; simulation < WIN_CHANCE_SIMULATIONS; simulation++)
        {
            bool playerWon = false;

            if (IsEvadeAgainstAttack(playerSkill, enemySkill))
            {
                // Si el jugador esquiva, "ganar" es esquivar todos los golpes
                playerWon = SimulateFullEvade(playerSide, enemySide);
            }
            else if (IsEvadeAgainstAttack(enemySkill, playerSkill))
            {
                // Si el enemigo esquiva, "ganar" es que al menos un golpe del jugador entre
                playerWon = !SimulateFullEvade(enemySide, playerSide);
            }
            else
            {
                playerWon = SimulateClash(playerSide, enemySide, nullptr).playerWon;
            }

            if (playerWon)
            {
                playerWins++;
            }
        }

        return static_cast<float>(playerWins) / static_cast<float>(WIN_CHANCE_SIMULATIONS);
    }

    static int GetCoinPowerBonus(const Skill& skill, const Unit& user, const Unit& target)
    {
        return CalculateCoinPowerBonus(skill, user.GetPoise().GetPotency(), target.GetSinking().GetPotency()) + CalculateStatusCoinPowerBonus(skill, target);
    }

    static int PredictCoinPowerBonus(const Unit& user, const Skill& skill, const Unit& target)
    {
        const PoiseEffects& effects = skill.GetPoiseEffects();
        int predictedPotency = user.GetPoise().GetPotency() + effects.onUsePotency;

        // Si gana cantidad sin tener Poise, arranca con 1 de potencia
        if (predictedPotency == 0 && effects.onUseCount > 0)
        {
            predictedPotency = 1;
        }

        return CalculateCoinPowerBonus(skill, predictedPotency, target.GetSinking().GetPotency()) + CalculateStatusCoinPowerBonus(skill, target);
    }

    static int CalculateCoinPowerBonus(const Skill& skill, int poisePotency, int sinkingPotency)
    {
        if (!skill.GetPoiseEffects().gainsCoinPowerFromPoise)
        {
            return 0;
        }

        const int bonus = (poisePotency + sinkingPotency) / POISE_PER_COIN_POWER;

        return (bonus > MAX_POISE_COIN_POWER_BONUS) ? MAX_POISE_COIN_POWER_BONUS : bonus;
    }

    static int GetBurnPlusTremor(const Unit& target)
    {
        return target.GetBurn().GetPotency() + target.GetTremor().GetPotency();
    }

    static int CalculateStatusCoinPowerBonus(const Skill& skill, const Unit& target)
    {
        const TargetStatusEffects& effects = skill.GetTargetStatusEffects();

        if (effects.statusPerCoinPower <= 0)
        {
            return 0;
        }

        const int bonus = GetBurnPlusTremor(target) / effects.statusPerCoinPower;

        return (bonus > effects.maxCoinPowerBonus) ? effects.maxCoinPowerBonus : bonus;
    }

    static int GetBasePowerBonus(const Skill& skill, const Unit& target)
    {
        const TargetStatusEffects& effects = skill.GetTargetStatusEffects();

        if (effects.statusPerFinalPower <= 0)
        {
            return 0;
        }

        const int bonus = GetBurnPlusTremor(target) / effects.statusPerFinalPower;

        return (bonus > effects.maxFinalPowerBonus) ? effects.maxFinalPowerBonus : bonus;
    }

    static int ApplyOnHitTargetStatuses(const Skill& skill, size_t coinIndex, Unit& target)
    {
        const std::vector<CoinStatusEffect>& perCoin = skill.GetTargetStatusEffects().perCoin;

        if (coinIndex >= perCoin.size())
        {
            return 0;
        }

        const CoinStatusEffect& effect = perCoin[coinIndex];

        // Burn
        if (effect.burnPotency > 0)
        {
            target.GetBurn().AddPotency(effect.burnPotency);
        }

        if (effect.burnCount > 0)
        {
            target.GetBurn().AddCount(effect.burnCount);
        }

        // Tremor (lo "sin Cracking" se suma si el golpe no hizo Cracking)
        int tremorPotency = effect.tremorPotency;
        int tremorCount = effect.tremorCount;

        if (!DidHitCrack())
        {
            tremorPotency += effect.tremorWithoutCrackingPotency;
            tremorCount += effect.tremorWithoutCrackingCount;
        }

        if (tremorPotency > 0)
        {
            target.GetTremor().AddPotency(tremorPotency);
        }

        if (tremorCount > 0)
        {
            target.GetTremor().AddCount(tremorCount);
        }

        // El Burst va despues de poner los estados de esta moneda, y solo si se cumple su condicion
        const bool crackingConditionMet = !effect.burstOnlyWithoutCracking || !DidHitCrack();
        int totalRaise = 0;

        if (effect.burstTimes > 0 && crackingConditionMet)
        {
            for (int burstIndex = 0; burstIndex < effect.burstTimes; burstIndex++)
            {
                totalRaise += target.TriggerTremorBurst();
            }

            for (int lossIndex = 0; lossIndex < effect.countLossAfterBurst; lossIndex++)
            {
                target.GetTremor().LoseOneCount();
            }
        }

        return totalRaise;
    }

    static void ApplyOnHitSinking(const Skill& skill, Unit& target)
    {
        const SinkingEffects& effects = skill.GetSinkingEffects();

        int potency = effects.onHitPotency;
        int count = effects.onHitCount;

        if (!DidHitCrack())
        {
            potency += effects.onHitWithoutCrackingPotency;
            count += effects.onHitWithoutCrackingCount;
        }

        if (potency > 0)
        {
            target.GetSinking().AddPotency(potency);
        }

        if (count > 0)
        {
            target.GetSinking().AddCount(count);
        }
    }

    static bool DidHitCrack()
    {
        return false;
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

    static int RollClashPower(const ClashSide& side, int coinsToFlip, std::vector<bool>* coinFaces)
    {
        const std::vector<Coin>& coins = side.skill->GetCoins();
        int power = side.skill->GetBasePower() + side.basePowerBonus;

        if (coinFaces != nullptr)
        {
            coinFaces->clear();
        }

        for (int coinIndex = 0; coinIndex < coinsToFlip; coinIndex++)
        {
            const bool isHeads = coins[static_cast<size_t>(coinIndex)].Flip(side.headsChance);

            if (isHeads)
            {
                power += coins[static_cast<size_t>(coinIndex)].GetPower() + side.coinPowerBonus;
            }

            if (coinFaces != nullptr)
            {
                coinFaces->push_back(isHeads);
            }
        }

        return power;
    }

    static ClashOutcome SimulateClash(const ClashSide& playerSide, const ClashSide& enemySide, CombatTimeline* timeline)
    {
        // Solo se guardan las caras/cruces cuando hay que mostrarlas
        std::vector<bool> playerFaces;
        std::vector<bool> enemyFaces;
        std::vector<bool>* playerFacesOutput = (timeline != nullptr) ? &playerFaces : nullptr;
        std::vector<bool>* enemyFacesOutput = (timeline != nullptr) ? &enemyFaces : nullptr;

        int playerCoins = playerSide.skill->GetCoinCount();
        int enemyCoins = enemySide.skill->GetCoinCount();
        int rounds = 0;

        while (playerCoins > 0 && enemyCoins > 0 && rounds < MAX_CLASH_ROUNDS)
        {
            const int playerPower = RollClashPower(playerSide, playerCoins, playerFacesOutput);
            const int enemyPower = RollClashPower(enemySide, enemyCoins, enemyFacesOutput);

            if (timeline != nullptr)
            {
                timeline->AddClashRound(playerPower, playerFaces, playerSide.skill->GetCoinCount(), enemyPower, enemyFaces, enemySide.skill->GetCoinCount());
            }

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

    static int GetClashWinSanity(int rounds)
    {
        const int extraRounds = (rounds > 1) ? rounds - 1 : 0;
        const float multiplier = 1.0f + SANITY_BONUS_PER_EXTRA_ROUND * static_cast<float>(extraRounds);

        return static_cast<int>(static_cast<float>(SANITY_PER_CLASH_WIN) * multiplier);
    }

    static AttackResult PerformAttack(Unit& attacker, const Skill& skill, int coinsToUse, Unit& target, bool onlyUnbreakable, int evadePower, CombatTimeline& timeline)
    {
        const std::vector<Coin>& coins = skill.GetCoins();
        const PoiseEffects& effects = skill.GetPoiseEffects();
        AttackResult result;
        int power = skill.GetBasePower() + GetBasePowerBonus(skill, target);

        for (int coinIndex = 0; coinIndex < coinsToUse && !target.IsDead(); coinIndex++)
        {
            const Coin& coin = coins[static_cast<size_t>(coinIndex)];

            if (onlyUnbreakable && !coin.IsUnbreakable())
            {
                continue;
            }

            // El bonus se recalcula en cada moneda porque el Poise puede subir golpe a golpe
            if (coin.Flip(attacker.GetHeadsChance()))
            {
                power += coin.GetPower() + GetCoinPowerBonus(skill, attacker, target);
            }

            // Si el objetivo esquiva, para pegar hay que superar su numero (si empata, se esquiva)
            if (evadePower >= 0 && power <= evadePower)
            {
                result.evadedHits++;
                continue;
            }

            int hitDamage = (power > 0) ? power : 0;

            const bool isCritical = attacker.GetPoise().RollCritical();

            if (isCritical)
            {
                hitDamage = static_cast<int>(std::round(static_cast<float>(hitDamage) * CRITICAL_DAMAGE_MULTIPLIER));
                result.criticalHits++;

                if (effects.onCriticalCount > 0)
                {
                    attacker.GetPoise().AddCount(effects.onCriticalCount);
                }
            }

            // Si el objetivo esta staggereado, el golpe pega el doble (el Sinking, que es dano fijo, no)
            if (target.IsStaggered())
            {
                hitDamage = static_cast<int>(std::round(static_cast<float>(hitDamage) * STAGGERED_DAMAGE_MULTIPLIER));
            }

            target.TakeDamage(hitDamage);
            result.damage += hitDamage;
            timeline.AddHit(target, hitDamage, isCritical);
            RecordStaggerIfAny(target, timeline);

            // Si el objetivo tenia Sinking, se activa con el golpe: pierde cordura, o vida si ya esta en -45
            const int sinkingAmount = target.GetSinking().Trigger();

            if (sinkingAmount > 0)
            {
                if (target.IsAtMinSanity())
                {
                    target.TakeDamage(sinkingAmount);
                    result.sinkingDamage += sinkingAmount;
                    timeline.AddHit(target, sinkingAmount, false);
                    RecordStaggerIfAny(target, timeline);
                }
                else
                {
                    target.ChangeSanity(-sinkingAmount);
                    result.sinkingSanityLoss += sinkingAmount;
                }
            }

            // El Sinking nuevo se pone despues, asi no se activa con el mismo golpe que lo puso
            ApplyOnHitSinking(skill, target);

            // Burn y Tremor de esta moneda (y Tremor Burst si la moneda lo tiene)
            const int burstRaise = ApplyOnHitTargetStatuses(skill, static_cast<size_t>(coinIndex), target);

            if (burstRaise > 0)
            {
                result.tremorBurstRaise += burstRaise;
                timeline.AddBanner("TREMOR BURST");
                RecordStaggerIfAny(target, timeline);
            }

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

    static void RecordStaggerIfAny(Unit& target, CombatTimeline& timeline)
    {
        if (target.ConsumeStaggerEvent())
        {
            timeline.AddBanner("¡STAGGER!");
            timeline.AddLogLine("¡" + target.GetName() + " queda staggereado! Pierde lo que le quedaba del turno y no actúa el próximo.");
        }
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

    static std::string DescribeExtras(const AttackResult& result)
    {
        std::string extras;

        if (result.sinkingSanityLoss > 0)
        {
            extras = "Sinking: -" + std::to_string(result.sinkingSanityLoss) + " SP";
        }

        if (result.sinkingDamage > 0)
        {
            if (!extras.empty())
            {
                extras += ", ";
            }

            extras += "+" + std::to_string(result.sinkingDamage) + " de daño por Sinking";
        }

        if (result.tremorBurstRaise > 0)
        {
            if (!extras.empty())
            {
                extras += ", ";
            }

            extras += "Tremor Burst: umbral de Stagger +" + std::to_string(result.tremorBurstRaise) + "%";
        }

        if (result.criticalHits > 0)
        {
            if (!extras.empty())
            {
                extras += ", ";
            }

            extras += (result.criticalHits == 1) ? "1 crítico" : std::to_string(result.criticalHits) + " críticos";
        }

        return extras.empty() ? "" : " (" + extras + ")";
    }

    static void ResolveOneSidedAction(Unit& attacker, const Skill& skill, Unit& target, CombatTimeline& timeline)
    {
        // Esquivar o contraatacar sin nadie enfrente no hace nada
        if (skill.IsDefensive())
        {
            timeline.AddLogLine(attacker.GetName() + " se queda en guardia con " + skill.GetShortName() + ", pero nadie lo ataca.");
            return;
        }

        const AttackResult result = PerformAttack(attacker, skill, skill.GetCoinCount(), target, false, -1, timeline);

        timeline.AddLogLine(attacker.GetName() + " ataca sin oposición con " + skill.GetShortName() + ": " + std::to_string(result.damage) + " de daño" + DescribeExtras(result) + ".");
    }

    static void ResolveClash(Unit& player, const Skill& playerSkill, Unit& enemy, const Skill& enemySkill, CombatTimeline& timeline)
    {
        // Dos defensas enfrentadas no pelean entre si
        if (playerSkill.IsDefensive() && enemySkill.IsDefensive())
        {
            timeline.AddLogLine(player.GetName() + " y " + enemy.GetName() + " se quedan a la defensiva. No pasa nada.");
            return;
        }

        const ClashSide playerSide = { &playerSkill, GetBasePowerBonus(playerSkill, enemy), GetCoinPowerBonus(playerSkill, player, enemy), player.GetHeadsChance() };
        const ClashSide enemySide = { &enemySkill, GetBasePowerBonus(enemySkill, player), GetCoinPowerBonus(enemySkill, enemy, player), enemy.GetHeadsChance() };
        const ClashOutcome outcome = SimulateClash(playerSide, enemySide, &timeline);

        Unit& winner = outcome.playerWon ? player : enemy;
        Unit& loser = outcome.playerWon ? enemy : player;
        const Skill& winnerSkill = outcome.playerWon ? playerSkill : enemySkill;
        const Skill& loserSkill = outcome.playerWon ? enemySkill : playerSkill;

        // Se explica en criollo que paso: quien gano, cuantas monedas le quedaron y cuanto pego
        // Ganar el clash da cordura (mas si el clash duro varias rondas)
        const int sanityGain = GetClashWinSanity(outcome.rounds);
        winner.ChangeSanity(sanityGain);

        std::string line = winner.GetName() + " gana el clash (" + winnerSkill.GetShortName() + " vs " + loserSkill.GetShortName() + ", +" + std::to_string(sanityGain) + " SP): le quedan " + std::to_string(outcome.winnerCoinsLeft);

        line += (outcome.winnerCoinsLeft == 1) ? " moneda" : " monedas";

        if (winnerSkill.DealsDamageOnWin())
        {
            const AttackResult result = PerformAttack(winner, winnerSkill, outcome.winnerCoinsLeft, loser, false, -1, timeline);

            line += " y pega " + std::to_string(result.damage) + " de daño" + DescribeExtras(result) + ".";
        }
        else
        {
            line += " y esquiva el ataque.";
        }

        timeline.AddLogLine(line);

        // Las monedas irrompibles del que perdio pegan igual despues del ganador (si sigue vivo)
        if (!loser.IsDead() && !loserSkill.IsDefensive() && CountUnbreakableCoins(loserSkill) > 0)
        {
            const AttackResult result = PerformAttack(loser, loserSkill, loserSkill.GetCoinCount(), winner, true, -1, timeline);

            timeline.AddLogLine("   Las monedas irrompibles de " + loser.GetName() + " pegan igual: " + std::to_string(result.damage) + " de daño" + DescribeExtras(result) + ".");
        }
    }

    static bool IsEvadeAgainstAttack(const Skill& evadeSkill, const Skill& otherSkill)
    {
        return evadeSkill.GetType() == SkillType::Evade && otherSkill.DealsDamageOnWin();
    }

    static bool SimulateFullEvade(const ClashSide& evadeSide, const ClashSide& attackSide)
    {
        const int evadePower = RollClashPower(evadeSide, evadeSide.skill->GetCoinCount(), nullptr);
        int power = attackSide.skill->GetBasePower() + attackSide.basePowerBonus;

        for (const Coin& coin : attackSide.skill->GetCoins())
        {
            if (coin.Flip(attackSide.headsChance))
            {
                power += coin.GetPower() + attackSide.coinPowerBonus;
            }

            if (power > evadePower)
            {
                return false;
            }
        }

        return true;
    }

    static bool ResolveEvade(Unit& evader, const Skill& evadeSkill, Unit& attacker, const Skill& attackSkill, CombatTimeline& timeline)
    {
        // La esquiva se tira una sola vez
        const ClashSide evadeSide = { &evadeSkill, GetBasePowerBonus(evadeSkill, attacker), GetCoinPowerBonus(evadeSkill, evader, attacker), evader.GetHeadsChance() };
        std::vector<bool> evadeFaces;
        const int evadePower = RollClashPower(evadeSide, evadeSkill.GetCoinCount(), &evadeFaces);

        timeline.AddEvadeRoll(evadePower, evadeFaces, evadeSkill.GetCoinCount());
        const AttackResult result = PerformAttack(attacker, attackSkill, attackSkill.GetCoinCount(), evader, false, evadePower, timeline);

        std::string line = evader.GetName() + " esquiva (" + evadeSkill.GetShortName() + ") con " + std::to_string(evadePower) + ": evita " + std::to_string(result.evadedHits) + " de " + std::to_string(attackSkill.GetCoinCount()) + " golpes de " + attacker.GetName() + " (" + attackSkill.GetShortName() + ")";

        if (result.damage > 0)
        {
            line += " y recibe " + std::to_string(result.damage) + " de daño" + DescribeExtras(result) + ".";
        }
        else
        {
            line += ". ¡No le hacen nada!";
        }

        timeline.AddLogLine(line);

        // La esquiva aguanta solo si no la supero ningun golpe
        return result.evadedHits >= attackSkill.GetCoinCount();
    }
}
