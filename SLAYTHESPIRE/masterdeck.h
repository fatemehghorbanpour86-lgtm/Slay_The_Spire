#ifndef MASTERDECK_H
#define MASTERDECK_H


#include <QVector>

class Card;

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

};

#endif // MASTERDECK_H