#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "relicsystem.h"

/*
 *  Player Responsibilities:
 *  - Manage Energy.
 *  - Manage Gold.
 *  - Own Player Decks.
 *  - Own Potion Inventory.
 *  - Own Relic Inventory.
 */



class Relic;
class Potion;
class Card;
class CombatDeck;



class Player : public Character
{
  private:

    int currentEnergy;
    int maxEnergy;

    int gold;

    CombatDeck* combatDeck;

    // QVector<Card*> hand;
    // QVector<Card*> drawPile;
    // QVector<Card*> discardPile;
    // QVector<Card*> exhaustPile;
    QVector<Potion*> potions;
    RelicSystem relicSystem;


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

    void addRelic(Relic* relic);
    bool hasRelic(RelicId id) const;
    Relic* getRelic(RelicId id) const;
    const QVector<Relic*>& getAllRelics() const;
    bool canRest() const;
    bool canUsePotions() const;
    bool canPlayCard();
    RelicSystem& getRelicSystem();
    const RelicSystem& getRelicSystem() const;
};

#endif