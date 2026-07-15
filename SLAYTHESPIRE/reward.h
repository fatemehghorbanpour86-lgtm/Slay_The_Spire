#ifndef REWARD_H
#define REWARD_H

#include <QVector>

class Card;
class Relic;
class Potion;

enum class RewardType
{
    Gold,
    Card,
    Relic,
    Potion
};

/*
 * Reward
 * ------
 * A single pending reward "slot" shown on RewardPage (one button, or one
 * row of buttons for a choice-reward).
 *
 * Depending on its RewardType, only the matching data below is populated:
 *
 *   Gold   -> goldAmount
 *   Potion -> potion                     (single, ready-made Potion*)
 *   Relic  -> relic                      (Normal / Elite tier: 1 ready-made Relic*)
 *          -> relicChoices (isChoice())  (Boss tier: 3 candidates, pick 1)
 *   Card   -> cardChoices (isChoice())   (always 3 candidates, pick 1)
 *
 * Ownership: Reward owns whatever Card*/Relic*/Potion* it was given until
                    * it is claimed. RewardSystem calls clearPotion()/clearRelic() (single
                           * rewards) or resolveCardChoice()/resolveRelicChoice() (choice rewards)
           * right after handing the chosen object to Player, so the destructor
            * below only ever deletes objects nobody else owns.
                */
        class Reward
{
public:

    // Gold reward.
    explicit Reward(int goldAmount);

    // Potion reward.
    explicit Reward(Potion* potion);

    // Single, already-decided Relic reward (Normal / Elite tier).
    explicit Reward(Relic* relic);

    // Card reward: exactly 3 candidates, player chooses 1.
    explicit Reward(const QVector<Card*>& cardChoices);

    // Boss Relic reward: exactly 3 candidates, player chooses 1.
    // The extra bool disambiguates this from the single-Relic constructor,
    // since both share RewardType::Relic.
    Reward(const QVector<Relic*>& relicChoices, bool isBossChoice);

    ~Reward();

    Reward(const Reward&) = delete;
    Reward& operator=(const Reward&) = delete;

    RewardType getType() const;

    // True only for the Card reward and the Boss Relic reward - both
    // require the player to pick one out of several candidates.
    bool isChoice() const;

    int getGoldAmount() const;
    Potion* getPotion() const;
    Relic* getRelic() const;                          // single relic reward
    const QVector<Card*>& getCardChoices() const;
    const QVector<Relic*>& getRelicChoices() const;    // boss relic reward

    // ---- Ownership hand-off, called by RewardSystem after a claim ----

    // Single-value rewards: Player now owns the object, don't delete it.
    void clearPotion();
    void clearRelic();

    // Choice rewards: 'chosen' is removed (not deleted) since Player now
    // owns it; every other candidate is deleted immediately.
    void resolveCardChoice(Card* chosen);
    void resolveRelicChoice(Relic* chosen);

private:
    RewardType type;
    bool choice;

    int goldAmount;
    Potion* potion;
    Relic* relic;
    QVector<Card*> cardChoices;
    QVector<Relic*> relicChoices;
};

#endif // REWARD_H
