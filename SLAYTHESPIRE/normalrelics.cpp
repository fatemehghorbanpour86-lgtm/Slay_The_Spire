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
