#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

/*
 *  Player Responsibilities:
 *  - Manage Energy.
 *  - Manage Gold.
 *  - Own Player Decks.(TODO)
 *  - Own Potion Inventory.(TODO)
 *  - Own Relic Inventory.(TODO)
 */


/*
 * Forward(TODO)
 * class Card;
 * class Potion;
 * class Relic;
 */



class Player : public Character
{
  private:

    int currentEnergy;
    int maxEnergy;

    int gold;

    // QVector<Card*> hand;
    // QVector<Card*> drawPile;
    // QVector<Card*> discardPile;
    // QVector<Card*> exhaustPile;
    // QVector<Relic*> relics;
    // QVector<Potion*> potions;


    // TODO (Anahita)
    // Add MasterDeck* after MasterDeck is implemented.

    // TODO (Anahita)
    // Add CombatDeck* after CombatDeck is implemented.

    // TODO (Fateme)
    // Add RelicInventory after Relic System is implemented.

    // TODO (Anahita)
    // Add PotionInventory after Potion System is implemented.


public:

    Player(const QString& name, int maxHealth);


    int getCurrentEnergy() const;
    int getMaxEnergy() const;
    void setEnergy(int value);
    void gainEnergy(int amount);
    bool useEnergy(int amount);
    void resetEnergy();


    int getGold() const;
    void gainGold(int amount);
    bool spendGold(int amount);

};

#endif