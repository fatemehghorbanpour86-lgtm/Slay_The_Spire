#include "event.h"

#include "player.h"

#include "masterdeck.h"

#include "card.h"

#include "eventrelics.h"

#include "normalrelics.h"

#include "normalenemies.h"

#include "relicsystem.h"

#include "cursecards.h"

#include "campfire.h"

#include "upgradecards.h"

#include "eventpage.h"

#include <QWidget>

#include <QRandomGenerator>

Event::Event(const QString& name, const QString& description)
    : name(name), description(description)
{
}

// Event Flow

void Event::start(Player* player)
{
    Q_UNUSED(player)

    // UI should display:
    //   - Event name
    //   - Description
    //   - Available options
}

// Getters

const QString& Event::getName() const
{
    return name;
}

const QString& Event::getDescription() const
{
    return description;
}

const QVector<EventOption>& Event::getOptions() const
{
    return options;
}

//======================================================
// Ominous Forge
//======================================================


OminousForge::OminousForge()
    : Event("Ominous Forge","The fire of the forge beckons. What will you do?")
{
    options.append(EventOption("[Forge] Upgrade a card."));
    options.append(EventOption("[Rummage] Gain Warped Tongs. Receive a Curse."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool OminousForge::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        return handleForge(player, parentWidget);
    case 1:
        handleRummage(player);
        return true;
    case 2:
        // [Leave]: Nothing happens.
        return true;
    default:
        return false;
    }
}


bool OminousForge::handleForge(Player* player, QWidget* parentWidget)
{
    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr || deck->getCardCount() == 0)
        return true;

    Campfire campfireLogic;

    UpgradeCardsDialog dialog(player, &campfireLogic, parentWidget);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    return true;
}

void OminousForge::handleRummage(Player* player)
{
    player->addRelic(new WarpedTongs());

    // TODO (Design): Confirm which Curse type Ominous Forge grants.
    // Using CurseOfTheBell as the default "generic" curse per cursecards.h.
    player->getMasterDeck()->addCard(new CurseOfTheBell());
}


//======================================================
// GoldenIdol
//======================================================


GoldenIdol::GoldenIdol()
    : Event("Golden Idol",
            "A golden idol sits on a pedestal. You feel a strange pull towards it...")
{
    options.append(EventOption("[Steal] Gain Golden Idol. Lose 5 Max HP."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool GoldenIdol::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)

    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        handleSteal(player);
        return true;
    case 1:
        // [Leave]: Nothing happens.
        return true;
    default:
        return false;
    }
}


void GoldenIdol::handleSteal(Player* player)
{
    player->decreaseMaxHealth(5);

    player->addRelic(new GoldenIdolRelic());
}



//======================================================
// Augmenter
//======================================================

Augmenter::Augmenter()
    : Event("Augmenter",
            "A scientist offers to enhance you with experimental procedures...")
{
    options.append(EventOption("[Test J.A.X] Receive the J.A.X card."));
    options.append(EventOption("[Become Test Subject] Transform 2 cards."));
    options.append(EventOption("[Ingest Mutagens] Gain Mutagenic Strength relic."));
}


bool Augmenter::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        handleTestJAX(player);
        return true;
    case 1:
        return handleBecomeTestSubject(player, parentWidget);
    case 2:
        handleIngestMutagens(player);
        return true;
    default:
        return false;
    }
}


void Augmenter::handleTestJAX(Player* player)
{
    player->getMasterDeck()->addCard(new JAX());
}

bool Augmenter::handleBecomeTestSubject(Player* player, QWidget* parentWidget)
{
    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr)
        return false;

    TransformCardsDialog dialog(player, parentWidget);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    return true;
}

void Augmenter::handleIngestMutagens(Player* player)
{
    player->addRelic(new MutagenicStrength());
}

//======================================================
// FaceTrader
//======================================================

FaceTrader::FaceTrader()
    : Event("Face Trader",
            "A strange face carved into the wall stares at you. It seems to want something...")
{
    options.append(EventOption("[Touch] Lose 5-10% Max HP. Gain 75 Gold."));
    options.append(EventOption("[Trade] Gain a random face relic."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool FaceTrader::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)

    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        handleTouch(player);
        return true;

    case 1:
        handleTrade(player);
        return true;

    case 2:
        // [Leave]: Nothing happens.
        return true;

    default:
        return false;
    }
}


void FaceTrader::handleTouch(Player* player)
{
    int maxHp = player->getMaxHealth();
    int percent = QRandomGenerator::global()->bounded(5, 11); // [5, 10]
    int loss = qRound(maxHp * percent / 100.0);
    player->decreaseMaxHealth(loss);

    player->gainGold(75);
}

void FaceTrader::handleTrade(Player* player)
{


    int roll = QRandomGenerator::global()->bounded(3);

    switch (roll)
    {
    case 0:
        player->addRelic(new FaceOfCleric());
        break;
    case 1:
        player->addRelic(new GremlinVisage());
        break;
    case 2:
        player->addRelic(new CultistHeadpiece());
        break;
    default:
        break;
    }
}

//======================================================
// TheCleric
//======================================================

TheCleric::TheCleric()
    : Event("The Cleric",
            "A cleric offers you their services, for the right price...")
{
    options.append(EventOption("[Heal] Lose 35 Gold. Heal 25% Max HP."));
    options.append(EventOption("[Purify] Lose 50 Gold. Remove a card."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool TheCleric::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:

        handleHeal(player);
        return true;
    case 1:

        return handlePurify(player, parentWidget);
    case 2:

        // [Leave]: Nothing happens.
        return true;

    default:
        return false;
    }
}


void TheCleric::handleHeal(Player* player)
{
    if (!player->spendGold(35))
        return;

    int healAmount = qRound(player->getMaxHealth() * 0.25);

    player->heal(healAmount);
}

bool TheCleric::handlePurify(Player* player, QWidget* parentWidget)
{
    if (player->getGold() < 50)
        return false;

    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr || deck->getCardCount() == 0)
        return true;

    RemoveCardDialog dialog(player, parentWidget);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    player->spendGold(50);
    return true;
}


//======================================================
// GoldenShrine
//======================================================


GoldenShrine::GoldenShrine()
    : Event("Golden Shrine","A golden shrine stands before you, emanating a warm glow...")
{
    options.append(EventOption("[Pray] Gain 100 Gold."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool GoldenShrine::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)

    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        handlePray(player);
        return true;
    case 1:
        // [Leave]: Nothing happens.
        return true;
    default:
        return false;
    }
}


void GoldenShrine::handlePray(Player* player)
{
    player->gainGold(100);
}


//======================================================
// TheJoust
//======================================================


TheJoust::TheJoust()
    : Event("The Joust",
            "Two knights prepare to joust. You may bet on the outcome...")
{
    options.append(EventOption("[Murderer] Bet 50 Gold on the Murderer. 70% chance to win 100 Gold."));
    options.append(EventOption("[Owner] Bet 50 Gold on the Owner. 30% chance to win 250 Gold."));
}


bool TheJoust::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)

    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:

        handleMurderer(player);
        return true;

    case 1:

        handleOwner(player);
        return true;

    default:
        return false;
    }
}


void TheJoust::handleMurderer(Player* player)
{

    if (!player->spendGold(50))
        return;


    int roll = QRandomGenerator::global()->bounded(100);
    if (roll < 70)
        player->gainGold(100);
}

void TheJoust::handleOwner(Player* player)
{

    if (!player->spendGold(50))
        return;


    int roll = QRandomGenerator::global()->bounded(100);
    if (roll < 30)
        player->gainGold(250);
}



//======================================================
// PleadingVagrant
//======================================================


PleadingVagrant::PleadingVagrant()
    : Event("Pleading Vagrant",
            "A vagrant pleads for your help. You could help... or take what they have.")
{
    options.append(EventOption("[Give 85 Gold] Spend 85 Gold. Gain a random relic."));
    options.append(EventOption("[Rob] Gain a random relic. Receive Curse: Regret."));
    options.append(EventOption("[Leave] Nothing happens."));
}


bool PleadingVagrant::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)

    if (player == nullptr)
        return false;

    switch (optionIndex)
    {
    case 0:
        handleGiveGold(player);
        return true;
    case 1:
        handleRob(player);
        return true;
    case 2:
        // [Leave]: Nothing happens.
        return true;
    default:
        return false;
    }
}


void PleadingVagrant::handleGiveGold(Player* player)
{
    if (!player->spendGold(85))
        return;

    grantRandomNormalRelic(player);
}

void PleadingVagrant::handleRob(Player* player)
{

    grantRandomNormalRelic(player);

    player->getMasterDeck()->addCard(new Regret());
}

void PleadingVagrant::grantRandomNormalRelic(Player* player)
{

    QVector<Relic*> pool;
    pool.append(new Girya());
    pool.append(new IceCream());
    pool.append(new Shuriken());
    pool.append(new Kunai());
    pool.append(new Anchor());
    pool.append(new HappyFlower());
    pool.append(new Orichalcum());
    pool.append(new Vajra());

    for (int i = pool.size() - 1; i >= 0; --i)
    {
        if (player->hasRelic(pool[i]->getId()))
        {
            delete pool[i];
            pool.removeAt(i);
        }
    }

    if (pool.isEmpty())
        return;

    int index = QRandomGenerator::global()->bounded(pool.size());
    Relic* chosen = pool[index];
    pool.removeAt(index);

    qDeleteAll(pool);

    player->addRelic(chosen);
}


//======================================================
// Colosseum(امتیازی)
//======================================================


Colosseum::Colosseum(QObject* parent)
    : QObject(parent),
    Event("The Colosseum",
          "The crowd roars. You must fight for their entertainment...")
{
    options.append(EventOption("[Fight] Face a random Elite enemy. Gain Elite rewards on victory."));
}


bool Colosseum::chooseOption(Player* player, int optionIndex, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget)
    Q_UNUSED(optionIndex)
    Q_UNUSED(player)

    return true;
    // if (player == nullptr)
    //     return false;

    // switch (optionIndex)
    // {
    // case 0:
    //     handleFight(player);
    //     return true;

    // default:
    //     return false;
    // }
}

// void Colosseum::handleFight(Player* player)
// {
//     QVector<Enemy*> encounter = buildRandomEliteEncounter();

//     // TODO (GameManager): Connect to requestEliteCombat signal.
//     // GameManager :
//     //   1. Create CombatManager with (player, encounter)
//     //   2. Call combatManager->startCombat()
//     //   3. Connect CombatManager::battleWon to call grantEliteReward(player)
//     emit requestEliteCombat(player, encounter);
// }

// void Colosseum::grantEliteReward(Player* player)
// {
//     if (player == nullptr)
//         return;

//     // Standard Elite reward: grant a random Event-tier relic.
//     // TODO (GameManager): Wire this call to CombatManager::battleWon signal.
//     RelicSystem::grantRandomRelics(player, Relic::Tier::Event, 1);
// }

// QVector<Enemy*> Colosseum::buildRandomEliteEncounter() const
// {
//     QVector<Enemy*> encounter;

//     int roll = QRandomGenerator::global()->bounded(4);

//     switch (roll)
//     {
//     case 0:
//         encounter.append(new GremlinNob());
//         break;

//     case 1:
//         encounter.append(new Sentry(false));
//         encounter.append(new Sentry(true));
//         encounter.append(new Sentry(false));
//         break;

//     case 2:
//         encounter.append(new BookOfStabbing());
//         break;

//     case 3:

//         encounter.append(new BlueSlaver());
//         encounter.append(new RedSlaver());
//         encounter.append(new Taskmaster());
//         break;
//     default:
//         break;
//     }

//     return encounter;
// }
