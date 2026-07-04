#include "eventrelics.h"

#include "player.h"

#include "effect.h"

#include <QDebug>

//======================================================
//  WarpedTongs
//======================================================

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


//======================================================
//  MutagenicStrength
//======================================================


MutagenicStrength::MutagenicStrength()
    : Relic("Mutagenic Strength",
            "At the start of combat gain 3 Strength and lose it after that turn. ",
            Relic::Tier::Event),

    strengthActive(false)
{
}

void MutagenicStrength::onCombatStart(Player* player)
{
    if (!player)
        return;

    player->addEffect(Effect::Type::Strength,Effect::Category::Buff,3);

    strengthActive = true;
}

void MutagenicStrength::onTurnEnd(Player* player)
{
    if (!player)
        return;

    if (!strengthActive)
        return;

    player->addEffect(Effect::Type::Strength,Effect::Category::Buff,-3);

    strengthActive = false;
}

//======================================================
//  CultistHeadpiece
//======================================================

CultistHeadpiece::CultistHeadpiece()
    : Relic(
          "Cultist Headpiece",
          "At the start of each combat, CAW! CAAAW!",
          Relic::Tier::Event)
{
}

void CultistHeadpiece::onCombatStart(Player* player)
{
    Q_UNUSED(player)

    qDebug() << "CAW! CAAAW!";

    // TODO (UI / Audio System)
    // TODO (AudioManager)
    // AudioManager::playCultistCaw();
}