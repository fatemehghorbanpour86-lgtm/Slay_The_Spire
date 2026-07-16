#ifndef SHOP_H
#define SHOP_H

#include <QVector>
#include "card.h"

class Player;
class Potion;

struct ShopCardOffer
{
    Card* card = nullptr;
    int price = 0;
    bool onSale = false;
    // onSale cards are discounted to a fixed price (80 gold).
    // Rare cards can never be onSale.
};
struct ShopPotionOffer
{
    Potion* potion = nullptr;
    int price = 0;
};

/*
 *  Shop Responsibilities:
 *  - Hold a randomly generated stock of cards and potions for a single visit.
 *  - Handle buying cards, buying potions, and removing a card from the player's MasterDeck.
 *  - The rising cost of Card Removal is NOT stored here: it persists on the
 *    Player across the whole run (see Player::getCardRemovalCost /
 *    Player::increaseCardRemovalCost), since the price keeps rising even
 *    after the player leaves this Shop instance.
 */

class Shop
{
private:

    QVector<ShopCardOffer> cardOffers;

    QVector<ShopPotionOffer> potionOffers;

    static const int CARD_COUNT = 5;

    static const int POTION_COUNT = 3;

    static const int SALE_PRICE = 80;

    static const int SALE_CHANCE_PERCENT = 30;

    static Card* rollRandomCard();
    // Picks one random card from the pool of all Attack/Skill/Power cards.
    // Curse and Status cards are never sold.


    static bool isRareCard(const Card* card);
    // Rarity isn't tracked on Card itself, and only matters here in the Shop
    // (pricing + which cards can go on Sale), so it's just a name lookup
    // against the small fixed list of RARE cards from the design doc.


    static int rollCardPrice(Card* card);
    // Returns a random price in the correct range based on whether the
    // card is Rare. Normal: 100-150 gold. Rare: 280-300 gold.


    static Potion* rollRandomPotion();

    static int rollPotionPrice(Potion* potion);
    // Fairy in a Bottle is always 125 gold; every other potion is 60-80 gold.

public:

    Shop();

    ~Shop();

    Shop(const Shop&) = delete;
    Shop& operator=(const Shop&) = delete;

    void generateStock();
    // Rolls a fresh set of cards and potions for this visit.
    // Should be called once when the player enters the Shop room.

    const QVector<ShopCardOffer>& getCardOffers() const;

    const QVector<ShopPotionOffer>& getPotionOffers() const;

    bool buyCard(Player* player, int index);
    // Buys the card at cardOffers[index]; on success it is moved into the
    // player's MasterDeck and removed from the shop's offer list.

    bool buyPotion(Player* player, int index);
    // Buys the potion at potionOffers[index]; on success it is added to
    // the player's potion inventory and removed from the shop's offer list.

    bool removeCard(Player* player, Card* card);
    // Spends Player::getCardRemovalCost() gold to permanently remove
    // "card" from the player's MasterDeck, then increases that cost by 25 for the rest of the run.
    void clear();
    // Frees any unsold cards/potions. Called by the destructor and can be
    // called manually before generateStock() if the shop is reused.
};
#endif // SHOP_H