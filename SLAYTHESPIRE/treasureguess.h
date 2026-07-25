#ifndef TREASUREGUESS_H
#define TREASUREGUESS_H

class Potion;

class TreasureGuessModel
{
public:
    static constexpr int CHEST_COUNT = 3;
    static constexpr int REWARD_CHEST_COUNT = 1;

    TreasureGuessModel();
    ~TreasureGuessModel();

    TreasureGuessModel(const TreasureGuessModel&) = delete;
    TreasureGuessModel& operator=(const TreasureGuessModel&) = delete;

    // Resets state (selection, claim, pending reward) and rolls a brand
    // new random reward chest. Call once whenever the page starts a round.
    void initializeGame();

    // Player picks a chest. Returns false if a chest was already selected
    // this round, or if 'index' is out of range. On success, if this is
    // the reward chest, a Potion is generated and stored internally.
    bool selectChest(int index);

    // True if 'index' is the chest currently holding the reward.
    bool isCorrectChest(int index) const;

    // True once the player has picked a chest this round (right or wrong).
    bool hasSelected() const;

    int getSelectedChestIndex() const; // -1 if none selected yet
    int getRewardChestIndex() const;

    // Read-only peek at the pending reward (not yet claimed).
    Potion* getReward() const;

    // Hands ownership of the pending reward Potion to the caller.
    // Returns nullptr if there is nothing to claim or it was already
    // claimed. Caller becomes responsible for the returned Potion*.
    Potion* claimReward();

    bool isClaimed() const;

    // True once this round is fully resolved: a wrong chest was opened,
    // OR the correct chest was opened AND the reward has been claimed.
    bool gameFinished() const;

private:
    void shuffleRewardChest();

    // Stub - mirrors TreasureManager::createRandomPotion() /
    // RewardSystem::createRandomPotion(). Swap for the real Potion
    // System hookup later.
    static Potion* generateRandomPotion();

    int rewardChestIndex;
    int selectedChestIndex;
    bool selected;
    bool claimed;

    Potion* pendingReward; // owned by this model until claimReward() is called
};

#endif // TREASUREGUESS_H
