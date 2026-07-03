#include "bossrelics.h"
#include "player.h"
#include "card.h"
#include "enemy.h"
#include "effect.h"

CallingBell::CallingBell()
    : Relic("Calling Bell",
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

    // TODO (Relic Manager / Reward System):
    // RelicManager::grantRandomRelics(player, Relic::Tier::Normal, 3);
}