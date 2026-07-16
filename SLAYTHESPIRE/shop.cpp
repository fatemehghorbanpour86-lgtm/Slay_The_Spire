#include "shop.h"
#include <utility>
#include <QRandomGenerator>
#include "masterdeck.h"
#include "potion.h"
#include "player.h"
#include<QSet>

Shop::Shop()
{
}

Shop::~Shop()
{
    clear();
}

Card* Shop::rollRandomCard()
{
    QVector<Card*> pool;
    pool += MasterDeck::createFullCardPool(CardType::Attack);
    pool += MasterDeck::createFullCardPool(CardType::Skill);
    pool += MasterDeck::createFullCardPool(CardType::Power);
    int index = QRandomGenerator::global()->bounded(pool.size());
    Card* chosen = pool[index];
    pool.removeAt(index);
    qDeleteAll(pool);
    return chosen;
}

bool Shop::isRareCard(const Card* card)
{
    if (card == nullptr)
        return false;
    // Matches the RARE cards listed in the design doc for Attack/Skill/Power.
    static const QSet<QString> rareCardNames =
    {
        "Reaper", "Bludgeon", "Feed", "Immolate",
        "Exhume", "Limit Break", "Offering", "Impervious",
        "Demon Form", "Brutality", "Barricade", "Berserk"
    };
    return rareCardNames.contains(card->getName());
}

int Shop::rollCardPrice(Card* card)
{
    if (isRareCard(card))
        return QRandomGenerator::global()->bounded(280, 301);
    return QRandomGenerator::global()->bounded(100, 151);
}


Potion* Shop::rollRandomPotion()
{
    QVector<Potion*> pool;
    pool.append(new BlockPotion());
    pool.append(new FirePotion());
    pool.append(new EnergyPotion());
    pool.append(new SwiftPotion());
    pool.append(new FairyInABottle());
    int index = QRandomGenerator::global()->bounded(pool.size());
    Potion* chosen = pool[index];
    pool.removeAt(index);
    qDeleteAll(pool);
    return chosen;
}


int Shop::rollPotionPrice(Potion* potion)
{
    if (potion != nullptr && potion->getName() == "Fairy in a Bottle")
        return 125;
    return QRandomGenerator::global()->bounded(60, 81);
}


void Shop::generateStock()
{
    clear();
    for (int i = 0; i < CARD_COUNT; ++i)
    {
        ShopCardOffer offer;
        offer.card = rollRandomCard();
        offer.price = rollCardPrice(offer.card);
        offer.onSale = false;
        cardOffers.append(offer);
    }
    // Sale: 30% chance that one non-Rare card in this shop is discounted to a fixed price of 80 gold.
    QVector<int> saleCandidates;
    for (int i = 0; i < cardOffers.size(); ++i)
    {
        if (!isRareCard(cardOffers[i].card))
        {
            saleCandidates.append(i);
        }
    }
    if (!saleCandidates.isEmpty() && QRandomGenerator::global()->bounded(100) < SALE_CHANCE_PERCENT)
    {
        int chosen = saleCandidates[QRandomGenerator::global()->bounded(saleCandidates.size())];
        cardOffers[chosen].price = SALE_PRICE;
        cardOffers[chosen].onSale = true;
    }
    for (int i = 0; i < POTION_COUNT; ++i)
    {
        ShopPotionOffer offer;
        offer.potion = rollRandomPotion();
        offer.price = rollPotionPrice(offer.potion);
        potionOffers.append(offer);
    }
}
const QVector<ShopCardOffer>& Shop::getCardOffers() const
{
    return cardOffers;
}
const QVector<ShopPotionOffer>& Shop::getPotionOffers() const
{
    return potionOffers;
}
bool Shop::buyCard(Player* player, int index)
{
    if (player == nullptr)
        return false;
    if (index < 0 || index >= cardOffers.size())
        return false;
    const ShopCardOffer offer = cardOffers[index];
    if (!player->spendGold(offer.price))
        return false;
    player->getMasterDeck()->addCard(offer.card); // ownership transfers to MasterDeck
    cardOffers.removeAt(index);
    return true;
}
bool Shop::buyPotion(Player* player, int index)
{
    if (player == nullptr)
        return false;
    if (index < 0 || index >= potionOffers.size())
        return false;
    const ShopPotionOffer offer = potionOffers[index];
    if (!player->spendGold(offer.price))
        return false;
    player->addPotion(offer.potion); // ownership transfers to Player
    potionOffers.removeAt(index);
    return true;
}
bool Shop::removeCard(Player* player, Card* card)
{
    if (player == nullptr || card == nullptr)
        return false;
    int cost = player->getCardRemovalCost();
    if (!player->spendGold(cost))
        return false;
    if (!player->getMasterDeck()->removeCard(card))
    {
        // Removal failed (e.g. the card was not removable or not found in
        // the deck) - refund the gold so the player isn't charged for nothing.
        player->gainGold(cost);
        return false;
    }
    player->increaseCardRemovalCost();
    return true;
}
void Shop::clear()
{
    for (const ShopCardOffer& offer : std::as_const(cardOffers))
        delete offer.card;
    cardOffers.clear();
    for (const ShopPotionOffer& offer : std::as_const(potionOffers))
        delete offer.potion;
    potionOffers.clear();
}