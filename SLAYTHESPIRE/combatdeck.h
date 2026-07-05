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

    void initializeFromMasterDeck(const MasterDeck& masterDeck);

    Card* drawCard();
    void drawCards(int count);

    void discardHand();
    bool exhaustCardFromHand(Card* card);

    bool removeFromHand(Card* card);

    bool addToDiscard(Card* card);
    bool addToExhaust(Card* card);

    void shuffleDrawPile();
    void reshuffleDiscardIntoDrawPile();

    const QVector<Card*>& getHand() const;
    const QVector<Card*>& getDrawPile() const;
    const QVector<Card*>& getDiscardPile() const;
    const QVector<Card*>& getExhaustPile() const;

    int handSize() const;
    int drawPileSize() const;
    int discardPileSize() const;
    int exhaustPileSize() const;
};

#endif // COMBATDECK_H