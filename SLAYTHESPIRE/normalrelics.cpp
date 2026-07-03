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