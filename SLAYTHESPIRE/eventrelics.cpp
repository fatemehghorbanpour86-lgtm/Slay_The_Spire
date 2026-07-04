#include "eventrelics.h"

WarpedTongs::WarpedTongs()
    : Relic("Warped Tongs",
   "At the start of combat, upgrade a random card for the rest of this combat.",
            Relic::Tier::Event)
{
}

void WarpedTongs::onCombatStart(Player* player)
{
    if (!player)
        return;

    // TODO (CombatDeck - Ana)

    // Upgrade one random card currently belonging to the combat deck, This upgrade is TEMPORARY and lasts only for this combat.

    // CombatDeck is responsible for:
    //  - selecting an eligible random card.
    //  - preventing duplicate upgrades.
    //  - restoring the original card after combat.

    // Example: player->getCombatDeck()->upgradeRandomCardForCombat();
}
