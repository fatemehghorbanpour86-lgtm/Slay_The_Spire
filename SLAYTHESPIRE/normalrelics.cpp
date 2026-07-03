#include "normalrelics.h"
#include "player.h"
#include "card.h"
#include "effect.h"

//STARTER RELIC
BurningBlood::BurningBlood()
    : Relic("Burning Blood", "At the end of combat, heal 6 HP.", Relic::Tier::Starter)
{
}
void BurningBlood::onCombatEnd(Player* player)
{
    if (!player)
        return;

    player->heal(6);
}


Girya::Girya()
    : Relic("Girya", "You may now lift at campsites.", Relic::Tier::Normal)
{
    setCounter(0); // number of Lifts used so far
}
bool Girya::canLift() const
{
    return getCounter() < MAX_LIFTS;
}
void Girya::lift(Player* player)
{
    if (!player || !canLift())
        return;

    // Permanent Strength -> default duration (-1) never expires.
    player->addEffect(Effect::Type::Strength, Effect::Category::Buff, 1);
    setCounter(getCounter() + 1);
}


//امتیازی**************************************************************
IceCream::IceCream()
    : Relic("Ice Cream", "Energy is now conserved between turns.", Relic::Tier::Normal)
{
    setCounter(0); // Energy saved from the end of the previous turn
}
void IceCream::onTurnEnd(Player* player)
{
    if (!player)
        return;

    // Save whatever Energy is still unspent, BEFORE CombatManager
    // calls Player::resetEnergy() for the next turn and overwrites it.
    setCounter(player->getCurrentEnergy());
}
void IceCream::onTurnStart(Player* player)
{
    if (!player)
        return;

    // At this point resetEnergy() has already run for the new turn.
    // We add back the saved leftover through the normal Energy
    // System entry point - no change to Player::resetEnergy() needed.
    if (getCounter() > 0)
        player->gainEnergy(getCounter());

    setCounter(0);
}


//امتیازی**************************************************************
Shuriken::Shuriken()
    : Relic("Shuriken", "Every time you play 3 Attacks in a turn, gain 1 Strength.", Relic::Tier::Normal)
{
    setCounter(0);
}
void Shuriken::onCardPlayed(Player* player, Card* card)
{
    if (!player || !card)
        return;

    if (card->getType() != CardType::Attack)
        return;

    setCounter(getCounter() + 1);

    if (getCounter() >= ATTACKS_REQUIRED)
    {
        player->addEffect(Effect::Type::Strength, Effect::Category::Buff, 1);
        setCounter(0);
    }
}


//امتیازی**************************************************************
Kunai::Kunai()
    : Relic("Kunai", "Every time you play 3 Attacks in a turn, gain 1 Dexterity.", Relic::Tier::Normal)
{
    setCounter(0);
}
void Kunai::onCardPlayed(Player* player, Card* card)
{
    if (!player || !card)
        return;

    if (card->getType() != CardType::Attack)
        return;

    setCounter(getCounter() + 1);

    if (getCounter() >= ATTACKS_REQUIRED)
    {
        player->addEffect(Effect::Type::Dexterity, Effect::Category::Buff, 1);
        setCounter(0);
    }
}



Anchor::Anchor()
    : Relic("Anchor", "Start each combat with 10 Block.", Relic::Tier::Normal)
{
}
void Anchor::onCombatStart(Player* player)
{
    if (!player)
        return;

    player->addBlock(BLOCK_AMOUNT);
}


HappyFlower::HappyFlower()
    : Relic("Happy Flower", "Every 3 turns, gain 1 Energy.", Relic::Tier::Normal)
{
    setCounter(0);
}
void HappyFlower::onTurnStart(Player* player)
{
    if (!player)
        return;

    setCounter(getCounter() + 1);

    if (getCounter() >= TURNS_REQUIRED)
    {
        player->gainEnergy(1);
        setCounter(0);
    }
}



Orichalcum::Orichalcum()
    : Relic("Orichalcum", "If you end your turn with 0 Block, gain 6 Block.", Relic::Tier::Normal)
{
}
void Orichalcum::onTurnEnd(Player* player)
{
    if (!player)
        return;

    if (!player->hasBlock())
        player->addBlock(BLOCK_AMOUNT);
}



