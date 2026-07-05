#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "relic.h"
#include "combatdeck.h"

/*
 *  Player Responsibilities:
 *  - Manage Energy.
 *  - Manage Gold.
 *  - Own Player Decks.
 *  - Own Potion Inventory.
 *  - Own Relic Inventory.(TODO)
 */



  class Card;
  class CombatDeck;
  class Relic;


class Potion;



class Player : public Character
{
  private:

    int currentEnergy;
    int maxEnergy;

    int gold;

     CombatDeck* combatDeck;

     QVector<Relic*> relics;
     QVector<Potion*> potions;


    // TODO (Fateme)
    // Add RelicInventory after Relic System is implemented.


public:

    Player(const QString& name, int maxHealth);

    ~Player() override;


    int getCurrentEnergy() const;
    int getMaxEnergy() const;
    void setEnergy(int value);
    void gainEnergy(int amount);
    bool useEnergy(int amount);
    void resetEnergy();


    int getGold() const;
    void gainGold(int amount);
    bool spendGold(int amount);

    void increaseMaxHealth(int amount);
    // Permanently increases the player's maximum HP.
    // Current HP is also increased by the same amount.

    void increaseMaxEnergy(int amount);
    //Permanently increases maxEnergy
    //Does NOT touch currentEnergy directly;


    void addPotion(Potion* potion);
    bool removePotion(Potion* potion);
    Potion* getPotion(int index) const;
    int getPotionCount() const;
    const QVector<Potion*>& getPotions() const;
};

#endif