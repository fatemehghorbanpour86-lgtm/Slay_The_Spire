#include "masterdeck.h"
#include "attackcards.h"
#include "skillcards.h"
#include "powercard.h"
#include "statuscards.h"
#include "cursecards.h"

MasterDeck::MasterDeck()
{
}

MasterDeck::~MasterDeck()
{
    qDeleteAll(cards);
    cards.clear();
}

void MasterDeck::addCard(Card* card)
{
    if(card == nullptr)
        return;

    cards.append(card);
}

bool MasterDeck::removeCard(Card* card)
{
    int index = cards.indexOf(card);

    if(index == -1)
        return false;

    if (!card->isRemovable())
        return false;

    delete cards[index];
    cards.removeAt(index);

    return true;
}

Card* MasterDeck::getCard(int index) const
{
    if(index < 0 || index >= cards.size())
        return nullptr;

    return cards[index];
}

int MasterDeck::getCardCount() const
{
    return cards.size();
}

const QVector<Card*>& MasterDeck::getCards() const
{
    return cards;
}

static QString canonicalCardId(Card* card)
{
    if (!card)
        return QString();

    QString name = card->getName();

    if (card->getIsUpgraded() && name.endsWith('+'))
        name.chop(1);

    return name;
}

MasterDeckSaveData MasterDeck::extractState() const
{
    MasterDeckSaveData data;

    for (Card* card : cards)
    {
        if (!card)
            continue;

        CardSaveData cardData;
        cardData.cardId = canonicalCardId(card);
        cardData.type = card->getType();
        cardData.isUpgraded = card->getIsUpgraded();

        data.cards.append(cardData);
    }

    return data;
}

void MasterDeck::restoreState(const MasterDeckSaveData& data)
{
    qDeleteAll(cards);
    cards.clear();

    for (const CardSaveData& cardData : data.cards)
    {
        Card* card = createCardById(cardData.cardId);

        if (!card)
            continue;

        if (cardData.isUpgraded)
            card->upgrade();

        cards.append(card);
    }
}

Card* MasterDeck::createCardById(const QString& cardId)
{
    // Attack
    if (cardId == "Strike")        return new Strike();
    if (cardId == "Reaper")        return new Reaper();
    if (cardId == "Bludgeon")      return new Bludgeon();
    if (cardId == "Feed")          return new Feed();
    if (cardId == "Immolate")      return new Immolate();
    if (cardId == "Twin Strike")   return new TwinStrike();
    if (cardId == "Hemokinesis")   return new Hemokinesis();
    if (cardId == "Carnage")       return new Carnage();

    // Skill
    if (cardId == "Defend")         return new Defend();
    if (cardId == "Exhume")         return new Exhume();
    if (cardId == "Limit Break")    return new LimitBreak();
    if (cardId == "Offering")       return new Offering();
    if (cardId == "Impervious")     return new Impervious();
    if (cardId == "Shrug It Off")   return new ShrugItOff();
    if (cardId == "True Grit")      return new TrueGrit();
    if (cardId == "Rage")           return new Rage();

    // Power
    if (cardId == "Inflame")        return new Inflame();
    if (cardId == "Metallicize")    return new Metallicize();
    if (cardId == "Demon Form")     return new DemonForm();
    if (cardId == "Brutality")      return new Brutality();
    if (cardId == "Barricade")      return new Barricade();
    if (cardId == "Feel No Pain")   return new FeelNoPain();
    if (cardId == "Berserk")        return new Berserk();
    if (cardId == "Dark Embrace")   return new DarkEmbrace();

    // Status
    if (cardId == "Dazed")  return new Daze();
    if (cardId == "Slimed") return new Slime();
    if (cardId == "Wound")  return new Wound();
    if (cardId == "Burn")   return new Burn();

    // Curse
    if (cardId == "J.A.X.")             return new JAX();
    if (cardId == "Curse of the Bell")  return new CurseOfTheBell();
    if (cardId == "Regret")             return new Regret();

    return nullptr;
}