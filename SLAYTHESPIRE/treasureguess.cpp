#include "treasureguess.h"
#include "potion.h"

#include <QRandomGenerator>

TreasureGuessModel::TreasureGuessModel()
    : rewardChestIndex(0),
    selectedChestIndex(-1),
    selected(false),
    claimed(false),
    pendingReward(nullptr)
{
    initializeGame();
}

TreasureGuessModel::~TreasureGuessModel()
{
    delete pendingReward;
}

void TreasureGuessModel::initializeGame()
{
    delete pendingReward;
    pendingReward = nullptr;

    selectedChestIndex = -1;
    selected = false;
    claimed = false;

    shuffleRewardChest();
}

void TreasureGuessModel::shuffleRewardChest()
{
    rewardChestIndex = QRandomGenerator::global()->bounded(CHEST_COUNT);
}

bool TreasureGuessModel::selectChest(int index)
{
    if (selected)
        return false;

    if (index < 0 || index >= CHEST_COUNT)
        return false;

    selectedChestIndex = index;
    selected = true;

    if (isCorrectChest(index))
    {
        pendingReward = generateRandomPotion();
    }

    return true;
}

bool TreasureGuessModel::isCorrectChest(int index) const
{
    return index == rewardChestIndex;
}

bool TreasureGuessModel::hasSelected() const
{
    return selected;
}

int TreasureGuessModel::getSelectedChestIndex() const
{
    return selectedChestIndex;
}

int TreasureGuessModel::getRewardChestIndex() const
{
    return rewardChestIndex;
}

Potion* TreasureGuessModel::getReward() const
{
    return pendingReward;
}

Potion* TreasureGuessModel::claimReward()
{
    if (claimed || pendingReward == nullptr)
        return nullptr;

    Potion* reward = pendingReward;
    pendingReward = nullptr;
    claimed = true;

    return reward;
}

bool TreasureGuessModel::isClaimed() const
{
    return claimed;
}

bool TreasureGuessModel::gameFinished() const
{
    if (!selected)
        return false;

    if (isCorrectChest(selectedChestIndex))
        return claimed;

    return true; // wrong chest: the round is over the moment it's opened
}

Potion* TreasureGuessModel::generateRandomPotion()
{
    // Stub implementation - same 4-way roll used by
    // TreasureManager::createRandomPotion() and
    // RewardSystem::createRandomPotion(). Replace with the real
    // Potion System call once this Room is wired in.
    int roll = QRandomGenerator::global()->bounded(4);

    switch (roll)
    {
    case 0: return new BlockPotion();
    case 1: return new FirePotion();
    case 2: return new EnergyPotion();
    case 3: return new SwiftPotion();
    }

    return new BlockPotion();
}
