#include "bossrelics.h"
#include "player.h"
#include "card.h"
#include "enemy.h"
#include "effect.h"

CallingBell::CallingBell()
    : Relic(RelicId::CallingBell, "Calling Bell",
            "Obtain the Curse of the Bell and 3 random Normal relics.",
            Relic::Tier::Boss)
{
}
void CallingBell::onEquip(Player* player)
{
    if (!player)
        return;

    // TODO (Deck System):
    // player->addCardToDeck(new CurseOfTheBell());

    RelicSystem::grantRandomRelics(player, Relic::Tier::Normal, 3);
}



MarkOfPain::MarkOfPain()
    : Relic(RelicId::MarkOfPain, "Mark of Pain",
            "Gain 1 extra Energy per turn. At the start of combat, "
            "2 Wounds are added to your draw pile.",
            Relic::Tier::Boss)
{
}
void MarkOfPain::onEquip(Player* player)
{
    if (!player)
        return;

    player->increaseMaxEnergy(1);
}
void MarkOfPain::onCombatStart(Player* player)
{
    if (!player)
        return;

    // TODO (Deck System):
    // player->addCardToDrawPile(new Wound());
    // player->addCardToDrawPile(new Wound());
}



//امتیازی***********************************************************
VelvetChoker::VelvetChoker()
    : Relic(RelicId::VelvetChoker, "Velvet Choker",
            "Gain 1 extra Energy per turn. You can no longer play "
            "more than 6 cards per turn.",
            Relic::Tier::Boss)
{
    setCounter(0);
}
void VelvetChoker::onEquip(Player* player)
{
    if (!player)
        return;

    player->increaseMaxEnergy(1);
}
void VelvetChoker::onTurnStart(Player* player)
{
    Q_UNUSED(player)

    setCounter(0);
}
void VelvetChoker::onCardPlayed(Player* player, Card* card)
{
    Q_UNUSED(player)
    Q_UNUSED(card)

    setCounter(getCounter() + 1);
}
bool VelvetChoker::canPlayCard(Player* player) const
{
    Q_UNUSED(player)

    return getCounter() < MAX_CARDS_PER_TURN;
}



BlackStar::BlackStar()
    : Relic(RelicId::BlackStar, "Black Star", "Elites now drop 2 relics when defeated.",
            Relic::Tier::Boss)
{
}
void BlackStar::onEnemyDeath(Player* player, Enemy* enemy)
{
    if (!player || !enemy)
        return;

    if (enemy->getName() != "Gremlin Knob" || enemy->getName() != "Sentry" || enemy->getName() != "Book of Stabbing"
        || enemy->getName() != "Taskmaster")
        return;

    // TODO (Reward System):
    // Black Star never builds relics itself — it only requests one
    // extra relic on top of the normal Elite reward. Reward System is
    // the single owner of relic-reward generation.
    // Example:
    // player->requestBonusRelicReward(1);
}



CoffeeDripper::CoffeeDripper()
    : Relic(RelicId::CoffeeDripper, "Coffee Dripper",
            "Gain 1 extra Energy per turn. You can no longer Rest at "
            "Rest sites.",
            Relic::Tier::Boss)
{
}
void CoffeeDripper::onEquip(Player* player)
{
    if (!player)
        return;

    player->increaseMaxEnergy(1);
}
bool CoffeeDripper::canRest() const
{
    return false;
}