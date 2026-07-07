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

