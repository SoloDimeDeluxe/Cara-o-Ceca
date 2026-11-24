#include "combat/combat_timeline.h"

#include "combat/unit.h"

namespace ReyesCuadra
{
    // Valor de columna que significa "fuera de las columnas"
    static const int NO_COLUMN = -1;

    CombatTimeline::CombatTimeline(const Unit& playerUnit, const Unit& enemyUnit)
        : player(&playerUnit)
        , enemy(&enemyUnit)
        , currentColumn(NO_COLUMN)
        , isHoldingLogLines(false)
    {
    }

    void CombatTimeline::StartColumn(int column)
    {
        currentColumn = column;
        events.push_back(MakeEvent(CombatEventType::ColumnStart));
    }

    void CombatTimeline::EndColumn()
    {
        currentColumn = NO_COLUMN;
    }

    void CombatTimeline::AddHit(const Unit& target, int damage, bool isCritical)
    {
        CombatEvent event = MakeEvent(CombatEventType::Hit);
        event.targetIsPlayer = (&target == player);
        event.damage = damage;
        event.isCritical = isCritical;

        events.push_back(event);
    }

    void CombatTimeline::AddClashRound(int playerPower, const std::vector<bool>& playerCoinFaces, int playerTotalCoins, int enemyPower, const std::vector<bool>& enemyCoinFaces, int enemyTotalCoins)
    {
        CombatEvent event = MakeEvent(CombatEventType::ClashRound);
        event.playerPower = playerPower;
        event.enemyPower = enemyPower;
        event.playerCoinFaces = playerCoinFaces;
        event.enemyCoinFaces = enemyCoinFaces;
        event.playerTotalCoins = playerTotalCoins;
        event.enemyTotalCoins = enemyTotalCoins;

        events.push_back(event);
    }

    void CombatTimeline::AddEvadeRoll(int evadePower, const std::vector<bool>& coinFaces, int totalCoins)
    {
        CombatEvent event = MakeEvent(CombatEventType::EvadeRoll);
        event.playerPower = evadePower;
        event.playerCoinFaces = coinFaces;
        event.playerTotalCoins = totalCoins;

        events.push_back(event);
    }

    void CombatTimeline::AddBanner(const std::string& text)
    {
        CombatEvent event = MakeEvent(CombatEventType::Banner);
        event.text = text;

        events.push_back(event);
    }

    void CombatTimeline::AddLogLine(const std::string& text)
    {
        CombatEvent event = MakeEvent(CombatEventType::LogLine);
        event.text = text;

        if (isHoldingLogLines)
        {
            heldLogLines.push_back(event);
        }
        else
        {
            events.push_back(event);
        }
    }

    void CombatTimeline::HoldLogLines()
    {
        isHoldingLogLines = true;
    }

    void CombatTimeline::AddSummaryAndReleaseHeld(const std::string& summaryLine)
    {
        isHoldingLogLines = false;
        AddLogLine(summaryLine);

        for (const CombatEvent& heldLine : heldLogLines)
        {
            events.push_back(heldLine);
        }

        heldLogLines.clear();
    }

    const std::vector<CombatEvent>& CombatTimeline::GetEvents() const
    {
        return events;
    }

    void CombatTimeline::Clear()
    {
        events.clear();
        heldLogLines.clear();
        isHoldingLogLines = false;
        currentColumn = NO_COLUMN;
    }

    CombatEvent CombatTimeline::MakeEvent(CombatEventType type) const
    {
        CombatEvent event;
        event.type = type;
        event.column = currentColumn;
        event.playerHpAfter = player->GetHp();
        event.enemyHpAfter = enemy->GetHp();

        return event;
    }
}
