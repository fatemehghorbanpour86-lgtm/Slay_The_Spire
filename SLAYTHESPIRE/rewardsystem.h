#ifndef REWARDSYSTEM_H
#define REWARDSYSTEM_H

#include <QVector>

class Player;
class Reward;
class Card;
class Relic;
class Potion;


class RewardSystem
{
public:
    RewardSystem();
    ~RewardSystem();

    RewardSystem(const RewardSystem&) = delete;
    RewardSystem& operator=(const RewardSystem&) = delete;

    // ---- Reward generation (called once, right after Battle ends) ----

    // Normal enemy: 15-25 Gold, a 3-card choice, and a chance of Potion.
    void generateNormalReward(Player* player);

    // Elite: 30-40 Gold, a 3-card choice, a chance of Potion, and 1 Normal
    // Relic (2 if the Player already has Black Star).
    void generateEliteReward(Player* player);

    // Boss: 80 Gold and a choice of 1 out of 3 Boss Relics.
    void generateBossReward(Player* player);

    // ---- Access for RewardPage ----

    const QVector<Reward*>& getRewards() const;

    // ---- Claiming (called by RewardPage when the player clicks a button) ----

    bool claimGoldReward(Player* player, Reward* reward);
    bool claimPotionReward(Player* player, Reward* reward);
    bool claimRelicReward(Player* player, Reward* reward);
    bool claimCardReward(Player* player, Reward* reward, Card* chosenCard);
    bool claimBossRelicReward(Player* player, Reward* reward, Relic* chosenRelic);

    // Removes 'reward' from the pending list and deletes it (used right
    // after a successful claim, and also by discardRemaining()).
    void removeReward(Reward* reward);

    // Called when the player presses Continue on RewardPage: every
    // Reward still pending is discarded (never applied to Player).
    void discardRemaining();

    static const int MAX_POTIONS = 3;

private:
    QVector<Reward*> rewards;

    static int rollGold(int minGold, int maxGold);
    static bool rollPotionChance();

    QVector<Card*> generateCardChoices() const;
    Potion* createRandomPotion() const;

    static const int POTION_DROP_CHANCE = 40; // percent
    static const int BOSS_GOLD_AMOUNT = 80;
};

#endif // REWARDSYSTEM_H
