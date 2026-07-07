#include "masterdeck.h"

#include "card.h"

#include "attackcards.h"

#include "skillcards.h"

#include "powercard.h"

#include <QRandomGenerator>


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

QVector<Card*> MasterDeck::createFullCardPool(CardType type)
{
    QVector<Card*> pool;

    if (type == CardType::Attack)
    {
        pool.append(new Strike());
        pool.append(new Reaper());
        pool.append(new Bludgeon());
        pool.append(new Feed());
        pool.append(new Immolate());
        pool.append(new TwinStrike());
        pool.append(new Hemokinesis());
        pool.append(new Carnage());
    }
    else if (type == CardType::Skill)
    {
        pool.append(new Defend());
        pool.append(new Exhume());
        pool.append(new LimitBreak());
        pool.append(new Offering());
        pool.append(new Impervious());
        pool.append(new ShrugItOff());
        pool.append(new TrueGrit());
        pool.append(new Rage());
    }
    else if (type == CardType::Power)
    {
        pool.append(new Inflame());
        pool.append(new Metallicize());
        pool.append(new DemonForm());
        pool.append(new Brutality());
        pool.append(new Barricade());
        pool.append(new FeelNoPain());
        pool.append(new Berserk());
        pool.append(new DarkEmbrace());
    }

    return pool;
}

Card* MasterDeck::transformCard(Card* card)
{
    if (card == nullptr || !card->isRemovable())
        return nullptr;

    if (card->getType() == CardType::Curse || card->getType() == CardType::Status)
        return nullptr;

    int index = cards.indexOf(card);
    if (index == -1)
        return nullptr;

    QVector<Card*> pool = createFullCardPool(card->getType());

    for (int i = pool.size() - 1; i >= 0; --i)
    {
        if (pool[i]->getName() == card->getName())
        {
            delete pool[i];
            pool.removeAt(i);
        }
    }

    if (pool.isEmpty())
        return nullptr;

    int randomIndex = QRandomGenerator::global()->bounded(pool.size());

    Card* newCard = pool[randomIndex];
    pool.removeAt(randomIndex);
    qDeleteAll(pool);

    delete cards[index];
    cards[index] = newCard;
    return newCard;
}
