#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "relicsystem.h"
#include "masterdeck.h"

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
class MasterDeck;

struct PlayerSaveData
{
    CharacterSaveData characterData;

    int gold = 0;
    int maxEnergy = 3;
    int cardRemovalCost = 50;

    QVector<RelicSaveData> relics;

    MasterDeckSaveData masterDeckData;
};

class Player : public Character
{
  private:

    int currentEnergy;
    int maxEnergy;

    int gold;

    int cardRemovalCost;

    MasterDeck* masterDeck;
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

    void decreaseMaxHealth(int amount);
    // Permanently decreases the player's maximum HP.

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

    bool canLift() const;
    bool lift();


    // ----------------------------
    // Master Deck Access
    // ----------------------------
    MasterDeck* getMasterDeck();
    const MasterDeck* getMasterDeck() const;


    // ----------------------------
    // Combat Deck Wrappers
    // ----------------------------

    void prepareForCombat();

    Card* drawCard();
    void drawCards(int count);
    void discardHand();
    bool discardCard(Card* card);
    bool exhaustCard(Card* card);
    bool moveFromExhaustToHand(Card* card = nullptr);
    void addCardToDiscardPile(Card* card);

    CombatDeck* getCombatDeck();
    const CombatDeck* getCombatDeck() const;

    Card* getRandomHandCard();
    bool exhaustRandomCardFromHand();

    PlayerSaveData extractState() const;
    void restoreState(const PlayerSaveData& data);

    int getCardRemovalCost() const;
    void increaseCardRemovalCost(int amount = 25);

};

#endif