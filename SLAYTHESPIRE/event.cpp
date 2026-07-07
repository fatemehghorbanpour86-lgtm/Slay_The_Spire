#include "event.h"

#include "player.h"

#include "masterdeck.h"

#include "card.h"

#include "eventrelics.h"

#include "cursecards.h"

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


void OminousForge::chooseOption(Player* player, int optionIndex)
{
    if (player == nullptr)
        return;

    switch (optionIndex)
    {
    case 0:
        handleForge(player);
        break;
    case 1:
        handleRummage(player);
        break;
    case 2:
        // [Leave]: Nothing happens.
        break;
    default:
        break;
    }
}


void OminousForge::handleForge(Player* player)
{
    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr || deck->getCardCount() == 0)
        return;

    // TODO (UI): Let the player pick a specific card to upgrade.
    // The UI should display all upgradeable cards from MasterDeck,
    // wait for selection, then call card->upgrade().
    // For now, pick a random upgradeable card.

    QVector<Card*> upgradeable;

    for (Card* card : deck->getCards())
    {
        if (card != nullptr && !card->getIsUpgraded())
            upgradeable.append(card);
    }

    if (upgradeable.isEmpty())
        return;

    int index = QRandomGenerator::global()->bounded(upgradeable.size());
    upgradeable[index]->upgrade();
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


void GoldenIdol::chooseOption(Player* player, int optionIndex)
{
    if (player == nullptr)
        return;

    switch (optionIndex)
    {
    case 0:
        handleSteal(player);
        break;
    case 1:
        // [Leave]: Nothing happens.
        break;
    default:
        break;
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


void Augmenter::chooseOption(Player* player, int optionIndex)
{
    if (player == nullptr)
        return;

    switch (optionIndex)
    {
    case 0:
        handleTestJAX(player);
        break;
    case 1:
        handleBecomeTestSubject(player);
        break;
    case 2:
        handleIngestMutagens(player);
        break;
    default:
        break;
    }
}


void Augmenter::handleTestJAX(Player* player)
{
    player->getMasterDeck()->addCard(new JAX());
}

void Augmenter::handleBecomeTestSubject(Player* player)
{
    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr)
        return;

    // TODO (UI): Let the player pick 2 specific cards to transform.
    // For now, pick 2 random transformable cards.

    // Build pool of transformable cards (isRemovable == true)

    QVector<Card*> pool;

    for (Card* card : deck->getCards())
    {
        if (card != nullptr && card->isRemovable())
            pool.append(card);
    }

    // Shuffle pool and transform up to 2 cards
    for (int i = pool.size() - 1; i > 0; --i)
    {
        int j = QRandomGenerator::global()->bounded(i + 1);
        pool.swapItemsAt(i, j);
    }

    int transformCount = qMin(2, pool.size());
    for (int i = 0; i < transformCount; ++i)
    {
        deck->transformCard(pool[i]);
    }
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


void FaceTrader::chooseOption(Player* player, int optionIndex)
{
    if (player == nullptr)
        return;

    switch (optionIndex)
    {
    case 0:
        handleTouch(player);
        break;

    case 1:
        handleTrade(player);
        break;

    case 2:
        // [Leave]: Nothing happens.
        break;

    default:
        break;
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


void TheCleric::chooseOption(Player* player, int optionIndex)
{
    if (player == nullptr)
        return;

    switch (optionIndex)
    {
    case 0:

        handleHeal(player);
        break;
    case 1:

        handlePurify(player);
        break;
    case 2:

        // [Leave]: Nothing happens.
        break;

    default:
        break;
    }
}


void TheCleric::handleHeal(Player* player)
{
    if (!player->spendGold(35))
        return;

    int healAmount = qRound(player->getMaxHealth() * 0.25);

    player->heal(healAmount);
}

void TheCleric::handlePurify(Player* player)
{
    if (!player->spendGold(50))
        return;

    MasterDeck* deck = player->getMasterDeck();
    if (deck == nullptr || deck->getCardCount() == 0)
        return;

    // TODO (UI): Let the player pick a specific card to remove.
    // For now, pick a random removable card.

    QVector<Card*> removable;

    for (Card* card : deck->getCards())
    {
        if (card != nullptr && card->isRemovable())
            removable.append(card);
    }

    if (removable.isEmpty())
        return;

    int index = QRandomGenerator::global()->bounded(removable.size());
    deck->removeCard(removable[index]);
}
