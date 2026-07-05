#ifndef COMBATDECK_H
#define COMBATDECK_H

#include <QVector>

class Card;
class MasterDeck;

class CombatDeck
{
private:

    QVector<Card*> drawPile;
    QVector<Card*> hand;
    QVector<Card*> discardPile;
    QVector<Card*> exhaustPile;

private:

    bool moveCard(QVector<Card*>& from, QVector<Card*>& to, Card* card);

public:

    CombatDeck();
    ~CombatDeck();

    CombatDeck(const CombatDeck&) = delete;
    CombatDeck& operator=(const CombatDeck&) = delete;

    //----------------------------------
    // Setup
    //----------------------------------

    void initializeFromMasterDeck(const MasterDeck& masterDeck);

    //----------------------------------
    // Draw System
    //----------------------------------

    Card* drawCard();
    void drawCards(int count);

    //----------------------------------
    // Turn Management
    //----------------------------------

    void discardHand();

    //----------------------------------
    // Card State Transitions (ONLY valid API)
    //----------------------------------

    bool moveFromHandToDiscard(Card* card);
    bool moveFromHandToExhaust(Card* card);

    //----------------------------------
    // Shuffle
    //----------------------------------

    void shuffleDrawPile();
    void reshuffleDiscardIntoDrawPile();

    //----------------------------------
    // Getters
    //----------------------------------

    const QVector<Card*>& getHand() const;
    const QVector<Card*>& getDrawPile() const;
    const QVector<Card*>& getDiscardPile() const;
    const QVector<Card*>& getExhaustPile() const;

    int handSize() const;
    int drawPileSize() const;
    int discardPileSize() const;
    int exhaustPileSize() const;
};

#endif