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



MarkOfPain::MarkOfPain()
    : Relic("Mark of Pain",
            "Gain 1 extra Energy per turn. At the start of combat, "
            "2 Wounds are added to your draw pile.",
            Relic::Tier::Boss)
{
}
void MarkOfPain::onEquip(Player* player)
{
    if (!player)
        return;

    player->setEnergy(player->getMaxEnergy() + 1);
}
void MarkOfPain::onCombatStart(Player* player)
{
    if (!player)
        return;

    // TODO (Deck System):
    // player->addCardToDrawPile(new Wound());
    // player->addCardToDrawPile(new Wound());
}