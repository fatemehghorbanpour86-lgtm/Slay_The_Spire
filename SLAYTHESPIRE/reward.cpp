#include "reward.h"
#include "card.h"
#include "relic.h"
#include "potion.h"

Reward::Reward(int goldAmount)
    : type(RewardType::Gold), choice(false),
    goldAmount(goldAmount), potion(nullptr), relic(nullptr)
{
}

Reward::Reward(Potion* potion)
    : type(RewardType::Potion), choice(false),
    goldAmount(0), potion(potion), relic(nullptr)
{
}

Reward::Reward(Relic* relic)
    : type(RewardType::Relic), choice(false),
    goldAmount(0), potion(nullptr), relic(relic)
{
}

Reward::Reward(const QVector<Card*>& cardChoices)
    : type(RewardType::Card), choice(true),
    goldAmount(0), potion(nullptr), relic(nullptr), cardChoices(cardChoices)
{
}

Reward::Reward(const QVector<Relic*>& relicChoices, bool isBossChoice)
    : type(RewardType::Relic), choice(isBossChoice),
    goldAmount(0), potion(nullptr), relic(nullptr), relicChoices(relicChoices)
{
}

Reward::~Reward()
{
    // Anything still non-null / non-empty here was never claimed by the
    // player, so this Reward still owns it and must clean it up.
    delete potion;
    delete relic;

    qDeleteAll(cardChoices);
    qDeleteAll(relicChoices);
}

RewardType Reward::getType() const
{
    return type;
}

bool Reward::isChoice() const
{
    return choice;
}

int Reward::getGoldAmount() const
{
    return goldAmount;
}

Potion* Reward::getPotion() const
{
    return potion;
}

Relic* Reward::getRelic() const
{
    return relic;
}

const QVector<Card*>& Reward::getCardChoices() const
{
    return cardChoices;
}

const QVector<Relic*>& Reward::getRelicChoices() const
{
    return relicChoices;
}

void Reward::clearPotion()
{
    potion = nullptr;
}

void Reward::clearRelic()
{
    relic = nullptr;
}

void Reward::resolveCardChoice(Card* chosen)
{
    for (Card* card : std::as_const(cardChoices))
    {
        if (card != chosen)
            delete card;
    }

    cardChoices.clear();
}

void Reward::resolveRelicChoice(Relic* chosen)
{
    for (Relic* relic : std::as_const(relicChoices))
    {
        if (relic != chosen)
            delete relic;
    }

    relicChoices.clear();
}
