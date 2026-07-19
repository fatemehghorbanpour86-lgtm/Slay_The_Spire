#ifndef TREASURE_H
#define TREASURE_H

class Player;
class Potion;
class Reward;

class TreasureManager
{
public:
    TreasureManager();
    ~TreasureManager();

    TreasureManager(const TreasureManager&) = delete;
    TreasureManager& operator=(const TreasureManager&) = delete;

    Reward* generateReward(Player* player);
    Reward* getReward() const;

    bool claimReward(Player* player);

    void discardReward();

    bool isClaimed() const;

private:
    Reward* reward;
    bool claimed;

    static int rollGold();
    static Potion* createRandomPotion();

    static const int MIN_GOLD = 150;
    static const int MAX_GOLD = 200;
};

#endif // TREASURE_H
