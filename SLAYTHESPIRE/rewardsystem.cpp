#include "rewardsystem.h"
#include "reward.h"
#include "player.h"
#include "relic.h"
#include "relicsystem.h"
#include "card.h"
#include "masterdeck.h"
#include "potion.h"

#include <QRandomGenerator>
#include <QString>

namespace
{

const QVector<QString> kRewardCardPool = {
    // Attack
    "Reaper", "Bludgeon", "Feed", "Immolate", "Twin Strike", "Hemokinesis", "Carnage",
    // Skill
    "Exhume", "Limit Break", "Offering", "Impervious", "Shrug It Off", "True Grit", "Rage",
    // Power
    "Inflame", "Metallicize", "Demon Form", "Brutality", "Barricade", "Feel No Pain", "Berserk", "Dark Embrace"
};
}

RewardSystem::RewardSystem()
{
}

RewardSystem::~RewardSystem()
{
    qDeleteAll(rewards);
    rewards.clear();
}

int RewardSystem::rollGold(int minGold, int maxGold)
{
    return QRandomGenerator::global()->bounded(minGold, maxGold + 1);
}

bool RewardSystem::rollPotionChance()
{
    return QRandomGenerator::global()->bounded(100) < POTION_DROP_CHANCE;
}

QVector<Card*> RewardSystem::generateCardChoices() const
{
    QVector<QString> pool = kRewardCardPool;

    for (int i = pool.size() - 1; i > 0; --i)
    {
        int j = QRandomGenerator::global()->bounded(i + 1);
        pool.swapItemsAt(i, j);
    }

    QVector<Card*> chosen;
    int count = qMin(3, pool.size());

    for (int i = 0; i < count; ++i)
    {
        Card* card = MasterDeck::createCardById(pool[i]);

        if (card)
            chosen.append(card);
    }

    return chosen;
}

Potion* RewardSystem::createRandomPotion() const
{
    int roll = QRandomGenerator::global()->bounded(4);

    switch (roll)
    {
    case 0: return new BlockPotion();
    case 1: return new FirePotion();
    case 2: return new EnergyPotion();
    case 3: return new SwiftPotion();
    }

    return nullptr;
}

void RewardSystem::generateNormalReward(Player* player)
{
    Q_UNUSED(player)

    rewards.append(new Reward(rollGold(15, 25)));

    QVector<Card*> cardChoices = generateCardChoices();
    if (!cardChoices.isEmpty())
        rewards.append(new Reward(cardChoices));

    if (rollPotionChance())
        rewards.append(new Reward(createRandomPotion()));
}

void RewardSystem::generateEliteReward(Player* player)
{
    rewards.append(new Reward(rollGold(30, 40)));

    QVector<Card*> cardChoices = generateCardChoices();
    if (!cardChoices.isEmpty())
        rewards.append(new Reward(cardChoices));

    if (rollPotionChance())
        rewards.append(new Reward(createRandomPotion()));

    // Black Star: Elites drop 2 Relics instead of 1.
    int relicCount = (player && player->hasRelic(RelicId::BlackStar)) ? 2 : 1;

    QVector<Relic*> relics = RelicSystem::createRandomRelics(player, Relic::Tier::Normal, relicCount);

    for (Relic* relic : std::as_const(relics))
        rewards.append(new Reward(relic));
}

void RewardSystem::generateBossReward(Player* player)
{
    rewards.append(new Reward(BOSS_GOLD_AMOUNT));

    QVector<Relic*> bossChoices = RelicSystem::createRandomRelics(player, Relic::Tier::Boss, 3);

    if (!bossChoices.isEmpty())
        rewards.append(new Reward(bossChoices, true));
}

const QVector<Reward*>& RewardSystem::getRewards() const
{
    return rewards;
}

void RewardSystem::removeReward(Reward* reward)
{
    int index = rewards.indexOf(reward);

    if (index == -1)
        return;

    rewards.removeAt(index);
    delete reward;
}

void RewardSystem::discardRemaining()
{
    qDeleteAll(rewards);
    rewards.clear();
}

bool RewardSystem::claimGoldReward(Player* player, Reward* reward)
{
    if (!player || !reward || reward->getType() != RewardType::Gold)
        return false;

    player->gainGold(reward->getGoldAmount());

    removeReward(reward);
    return true;
}

bool RewardSystem::claimPotionReward(Player* player, Reward* reward)
{
    if (!player || !reward || reward->getType() != RewardType::Potion)
        return false;

    if (player->getPotionCount() >= MAX_POTIONS)
        return false; // slots full: reward stays pending, not claimable

    Potion* potion = reward->getPotion();

    if (!potion)
        return false;

    player->addPotion(potion);
    reward->clearPotion();

    removeReward(reward);
    return true;
}

bool RewardSystem::claimRelicReward(Player* player, Reward* reward)
{
    if (!player || !reward || reward->getType() != RewardType::Relic || reward->isChoice())
        return false;

    Relic* relic = reward->getRelic();

    if (!relic)
        return false;

    player->addRelic(relic);
    reward->clearRelic();

    removeReward(reward);
    return true;
}

bool RewardSystem::claimCardReward(Player* player, Reward* reward, Card* chosenCard)
{
    if (!player || !reward || reward->getType() != RewardType::Card || !chosenCard)
        return false;

    if (!reward->getCardChoices().contains(chosenCard))
        return false;

    player->getMasterDeck()->addCard(chosenCard);
    reward->resolveCardChoice(chosenCard); // deletes the other 2 candidates

    removeReward(reward);
    return true;
}

bool RewardSystem::claimBossRelicReward(Player* player, Reward* reward, Relic* chosenRelic)
{
    if (!player || !reward || reward->getType() != RewardType::Relic || !reward->isChoice() || !chosenRelic)
        return false;

    if (!reward->getRelicChoices().contains(chosenRelic))
        return false;

    player->addRelic(chosenRelic);
    reward->resolveRelicChoice(chosenRelic); // deletes the other 2 candidates

    removeReward(reward);
    return true;
}