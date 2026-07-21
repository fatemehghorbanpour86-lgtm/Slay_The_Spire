#include "eventrelics.h"

#include "player.h"

#include "effect.h"

#include "audiomanager.h"

#include <QDebug>

//======================================================
//  WarpedTongs
//======================================================

WarpedTongs::WarpedTongs()
    : Relic(RelicId::WarpedTongs, "Warped Tongs",
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
    : Relic(RelicId::MutagenicStrength, "Mutagenic Strength",
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
    : Relic(RelicId::CultistHeadpiece,
          "Cultist Headpiece",
          "At the start of each combat, CAW! CAAAW!",
          Relic::Tier::Event)
{
}

void CultistHeadpiece::onCombatStart(Player* player)
{
    Q_UNUSED(player)

    qDebug() << "CAW! CAAAW!";

    AudioManager::instance().play(AudioManager::Sound::Raven);
}

//======================================================
//  GremlinVisage
//======================================================

GremlinVisage::GremlinVisage()
    : Relic(RelicId::GremlinVisage,
          "Gremlin Visage",
          "At the start of combat, gain 1 Weak.",
          Relic::Tier::Event)
{
}

void GremlinVisage::onCombatStart(Player* player)
{
    if (!player)
        return;

    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff,1,1);
}

//======================================================
//  FaceOfCleric
//======================================================

FaceOfCleric::FaceOfCleric()
    : Relic(RelicId::FaceOfCleric,
          "Face Of Cleric",
          "Increase your Max HP by 1 after every combat.",
          Relic::Tier::Event)
{
}

void FaceOfCleric::onCombatEnd(Player* player)
{
    if (!player)
        return;

    player->increaseMaxHealth(1);
}

//======================================================
//  GoldenIdolRelic
//======================================================

GoldenIdolRelic::GoldenIdolRelic()
    : Relic(RelicId::GoldenIdolRelic,
            "Golden Idol",
            "Enemies drop 25% more gold.",
            Relic::Tier::Event)
{
}

void GoldenIdolRelic::onEnemyDeath(Player* player, Enemy* enemy)
{
    if (player == nullptr || enemy == nullptr)
        return;

    // TODO: enemy->getGoldDrop()
    // int bonus = qRound(enemy->getGoldDrop() * 0.25f);
    // player->gainGold(bonus);
}