#ifndef MASTERDECK_H
#define MASTERDECK_H


#include <QVector>
#include "card.h"

class Card;

struct MasterDeckSaveData
{
    QVector<CardSaveData> cards;
};

class MasterDeck
{

 private:

    QVector<Card*> cards;


 public:

    MasterDeck();
    ~MasterDeck();

    MasterDeck(const MasterDeck&) = delete;
    MasterDeck& operator=(const MasterDeck&) = delete;


    void addCard(Card* card);

    bool removeCard(Card* card);

    Card* getCard(int index) const;

    int getCardCount() const;

    const QVector<Card*>& getCards() const;


    Card* transformCard(Card* card);

 private:

    static QVector<Card*> createFullCardPool(CardType type);
     // Returns freshly allocated instances of all playable cards of the given type.
     //Curse and Status cards are excluded
    MasterDeckSaveData extractState() const;
    void restoreState(const MasterDeckSaveData& data);

    static Card* createCardById(const QString& cardId);

};

#endif // MASTERDECK_H
