#include "masterdeck.h"

#include "card.h"

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