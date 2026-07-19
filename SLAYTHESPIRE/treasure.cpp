#include "treasure.h"
#include "reward.h"
#include "relicsystem.h"
#include "relic.h"
#include "player.h"
#include "potion.h"

#include <QRandomGenerator>

TreasureManager::TreasureManager()
    : reward(nullptr), claimed(false)
{
}

TreasureManager::~TreasureManager()
{
    delete reward;
}

int TreasureManager::rollGold()
{
    return QRandomGenerator::global()->bounded(MIN_GOLD, MAX_GOLD + 1);
}

Potion* TreasureManager::createRandomPotion()
{
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

Reward* TreasureManager::generateReward(Player* player)
{
    delete reward;
    reward = nullptr;
    claimed = false;

    int roll = QRandomGenerator::global()->bounded(9);

    if (roll < 6)
    {
        Relic* relic = RelicSystem::createRandomRelic(player, Relic::Tier::Normal);

        if (relic != nullptr)
        {
            reward = new Reward(relic);
            return reward;
        }

        // Normal relic pool exhausted for this player - fall back to Gold.
        roll = 6;
    }

    if (roll == 6 || roll == 7)
    {
        reward = new Reward(rollGold());
    }
    else
    {
        reward = new Reward(createRandomPotion());
    }

    return reward;
}

Reward* TreasureManager::getReward() const
{
    return reward;
}

bool TreasureManager::claimReward(Player* player)
{
    if (!player || !reward || claimed)
        return false;

    switch (reward->getType())
    {
    case RewardType::Gold:
        player->gainGold(reward->getGoldAmount());
        break;

    case RewardType::Potion:
    {
        Potion* potion = reward->getPotion();

        if (!potion)
            return false;

        player->addPotion(potion);
        reward->clearPotion();
        break;
    }

    case RewardType::Relic:
    {
        Relic* relic = reward->getRelic();

        if (!relic)
            return false;

        player->addRelic(relic);
        reward->clearRelic();
        break;
    }

    case RewardType::Card:
        // Treasure rewards never generate a Card choice.
        return false;
    }

    claimed = true;
    return true;
}

void TreasureManager::discardReward()
{
    delete reward;
    reward = nullptr;
    claimed = false;
}

bool TreasureManager::isClaimed() const
{
    return claimed;
}
