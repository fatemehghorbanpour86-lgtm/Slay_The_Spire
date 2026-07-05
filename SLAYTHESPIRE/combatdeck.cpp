#include "combatdeck.h"
#include "masterdeck.h"
#include "card.h"

#include <QRandomGenerator>
#include <algorithm>

CombatDeck::CombatDeck()
{
}

CombatDeck::~CombatDeck()
{
    qDeleteAll(drawPile);
    qDeleteAll(hand);
    qDeleteAll(discardPile);
    qDeleteAll(exhaustPile);
}

//----------------------------------
// Setup
//----------------------------------

void CombatDeck::initializeFromMasterDeck(const MasterDeck& masterDeck)
{
    qDeleteAll(drawPile);
    qDeleteAll(hand);
    qDeleteAll(discardPile);
    qDeleteAll(exhaustPile);

    drawPile.clear();
    hand.clear();
    discardPile.clear();
    exhaustPile.clear();

    for(Card* card : masterDeck.getCards())
    {
        drawPile.append(card->clone());
    }

    shuffleDrawPile();
}

//----------------------------------
// Internal Move Helper
//----------------------------------

bool CombatDeck::moveCard(QVector<Card*>& from, QVector<Card*>& to, Card* card)
{
    int index = from.indexOf(card);

    if(index == -1)
        return false;

    from.removeAt(index);
    to.append(card);

    return true;
}

//----------------------------------
// Draw System
//----------------------------------

Card* CombatDeck::drawCard()
{
    if(drawPile.isEmpty())
        reshuffleDiscardIntoDrawPile();

    if(drawPile.isEmpty())
        return nullptr;

    Card* card = drawPile.takeLast();
    hand.append(card);

    return card;
}

void CombatDeck::drawCards(int count)
{
    for(int i = 0; i < count; i++)
    {
        if(drawCard() == nullptr)
            return;
    }
}

//----------------------------------
// Turn Management
//----------------------------------

void CombatDeck::discardHand()
{
    while(!hand.isEmpty())
    {
        discardPile.append(hand.takeLast());
    }
}

//----------------------------------
// Card Transitions
//----------------------------------

bool CombatDeck::moveFromHandToDiscard(Card* card)
{
    if(!moveCard(hand, discardPile, card))
        return false;

    return true;
}

bool CombatDeck::moveFromHandToExhaust(Card* card)
{
    if(!moveCard(hand, exhaustPile, card))
        return false;

    return true;
}

//----------------------------------
// Shuffle System
//----------------------------------

void CombatDeck::shuffleDrawPile()
{
    std::shuffle(drawPile.begin(), drawPile.end(), *QRandomGenerator::global());
}

void CombatDeck::reshuffleDiscardIntoDrawPile()
{
    if(discardPile.isEmpty())
        return;

    drawPile += discardPile;
    discardPile.clear();

    shuffleDrawPile();
}

//----------------------------------
// Getters
//----------------------------------

const QVector<Card*>& CombatDeck::getHand() const
{
    return hand;
}

const QVector<Card*>& CombatDeck::getDrawPile() const
{
    return drawPile;
}

const QVector<Card*>& CombatDeck::getDiscardPile() const
{
    return discardPile;
}

const QVector<Card*>& CombatDeck::getExhaustPile() const
{
    return exhaustPile;
}

//----------------------------------
// Sizes
//----------------------------------

int CombatDeck::handSize() const
{
    return hand.size();
}

int CombatDeck::drawPileSize() const
{
    return drawPile.size();
}

int CombatDeck::discardPileSize() const
{
    return discardPile.size();
}

int CombatDeck::exhaustPileSize() const
{
    return exhaustPile.size();
}