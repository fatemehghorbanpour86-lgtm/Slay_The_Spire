#include "combatmanager.h"

#include "normalenemies.h"
#include "player.h"
#include "enemy.h"
#include "card.h"
#include "potion.h"
#include "combatcalculator.h"
#include "combatdeck.h"
#include "relicsystem.h"
#include "statuscards.h"

#include <utility>
#include <QDebug>

CombatManager::CombatManager(Player* p, QVector<Enemy*> e, QObject* parent)
    : QObject(parent),
    currentState(CombatState::BattleStart),
    player(p),
    enemies(e),
    calculator(new CombatCalculator()),
    turnCount(0)
{

    for (Enemy* enemy : std::as_const(enemies))
    {
        connectEnemy(enemy);
    }
}

CombatManager::~CombatManager()
{
    delete calculator;
}

void CombatManager::startCombat()
{
    changeState(CombatState::BattleStart);
}

void CombatManager::changeState(CombatState newState)
{
    currentState = newState;
    emit combatStateChanged(currentState);

    switch (currentState)
    {
    case CombatState::BattleStart:
        handleBattleStart();
        break;

    case CombatState::TurnStart:
        handleTurnStart();
        break;

    case CombatState::PlayerAction:
        break;

    case CombatState::TurnEnd:
        handleTurnEnd();
        break;

    case CombatState::EnemyTurn:
        handleEnemyTurn();
        break;

    case CombatState::BattleWon:
        cleanupAfterCombat();
        emit battleWon();
        break;

    case CombatState::BattleLost:
        cleanupAfterCombat();
        emit battleLost();
        break;

    }
}

void CombatManager::handleBattleStart()
{
    player->prepareForCombat();
    player->getRelicSystem().onCombatStart(player);

    for (Enemy* enemy : std::as_const(enemies))
    {
        if (enemy && !enemy->isDead())
        {
            enemy->chooseIntent(player);
        }
    }

    emit combatStarted();

    changeState(CombatState::TurnStart);

    emit statsUpdated();
}

void CombatManager::handleTurnStart()
{
    if (!player)
        return;

    turnCount++;

    player->startTurnBlockReset();

    player->resetEnergy();
    player->drawCards(5);
    player->getRelicSystem().onTurnStart(player);
    player->onTurnStartEffects();

    emit statsUpdated();

    checkWinLossCondition();
    if (isBattleFinished())
        return;

    emit playerTurnStarted();
    changeState(CombatState::PlayerAction);
}

bool CombatManager::playCard(Card* card, Enemy* target)
{
    if (currentState != CombatState::PlayerAction)
        return false;

    if (waitingForExhumeSelection)
        return false;

    if (!player || !card)
        return false;

    if (card->getType() == CardType::Attack &&
        player->hasEffect(Effect::Type::Entangle))
    {
        return false;
    }

    if (!card->canPlay())
        return false;

    if (!player->useEnergy(card->getEnergyCost()))
        return false;

    if (card->getType() == CardType::Attack)
    {
        if (Effect* rage = player->getEffect(Effect::Type::Rage))
        {
            CombatCalculator::grantBlock(player, rage->getAmount());
        }
    }

    if (card->getType() == CardType::Skill)
    {
        for (Enemy* enemy : std::as_const(enemies))
        {
            if (!enemy)
                continue;

            if (Effect* enrage = enemy->getEffect(Effect::Type::Enrage))
            {
                enemy->addEffect(Effect::Type::Strength,
                                 Effect::Category::Buff,
                                 enrage->getAmount(),
                                 0);
            }
        }
    }



    emit cardPlayed(card, target);
    player->getRelicSystem().onCardPlayed(player, card);

    if (card->getName() == "Exhume")
    {
        beginExhumeSelection(card);
        emit statsUpdated();
        checkWinLossCondition();
        return true;
    }

    card->play(player, enemies, target);

    finalizeCardAfterUse(card);

    emit statsUpdated();
    checkWinLossCondition();

    return true;
}

bool CombatManager::usePotion(Potion* potion, Enemy* target)
{
    if (currentState != CombatState::PlayerAction)
        return false;

    if (waitingForExhumeSelection)
        return false;

    if (!potion || !player)
        return false;

    if (!player->getRelicSystem().canUsePotions())
        return false;

    if (!potion->canUse(player))
        return false;

    potion->use(player, target);

    player->removePotion(potion);

    emit statsUpdated();
    checkWinLossCondition();

    return true;
}

void CombatManager::endTurn()
{
    if (currentState != CombatState::PlayerAction)
        return;

    if (waitingForExhumeSelection)
        return;

    changeState(CombatState::TurnEnd);
}

void CombatManager::handleTurnEnd()
{
    if (!player)
        return;

    CombatDeck* deck = player->getCombatDeck();

    if (deck)
    {
        QVector<Card*> handCards = deck->getHand();

        // 1) End-of-turn card effects while still in hand
        for (Card* card : std::as_const(handCards))
        {
            if (!card)
                continue;

            if (card->getName() == "Regret")
            {
                int handSize = deck->handSize();
                int damage = handSize;
                player->loseHP(damage);
            }

            if (card->getName() == "Burn")
            {
                if (Burn* burn = dynamic_cast<Burn*>(card))
                {
                    CombatCalculator::dealDamage(nullptr, player, burn->getDamageAmount());
                }
            }
        }
    }

    emit statsUpdated();
    checkWinLossCondition();
    if (isBattleFinished())
        return;

    // 2) Character end-of-turn effects
    player->onTurnEndEffects();

    // 3) Timed debuffs/buffs tick down here
    decreaseTimedEffects(player);

    emit statsUpdated();
    checkWinLossCondition();
    if (isBattleFinished())
        return;

    // 4) Cleanup hand: Ethereal / Retain / normal discard
    if (deck)
    {
        QVector<Card*> handCards = deck->getHand();

        for (Card* card : std::as_const(handCards))
        {
            if (!card)
                continue;

            if (card->doesEthereal())
            {
                moveCardFromHandToExhaust(card);
            }
            else if (card->doesRetain())
            {
                continue;
            }
            else
            {
                deck->moveFromHandToDiscard(card);
            }
        }
    }

    // 5) Relics
    player->getRelicSystem().onTurnEnd(player);

    emit statsUpdated();
    checkWinLossCondition();
    if (isBattleFinished())
        return;

    changeState(CombatState::EnemyTurn);
}

void CombatManager::handleEnemyTurn()
{
    emit enemyTurnStarted();

    for (int i = 0; i < enemies.size(); ++i)
    {
        Enemy* enemy = enemies[i];
        if (!enemy || enemy->isDead())
            continue;

        enemy->startTurnBlockReset();
        enemy->onTurnStartEffects();

        emit statsUpdated();
        checkWinLossCondition();
        if (isBattleFinished())
            return;

        if (enemy->isDead())
            continue;

        emit enemyAttacking(enemy);

        enemy->executeMove(player);

        handleEnemySplit(enemy);

        if (!enemies.contains(enemy))
        {
            i += 1;
        }

        emit statsUpdated();
        checkWinLossCondition();
        if (isBattleFinished())
            return;

        if (enemies.contains(enemy) && !enemy->isDead())
        {
            enemy->onTurnEndEffects();
            decreaseTimedEffects(enemy);
            enemy->finishTurn();

            if (!enemy->isDead() && player->getCurrentHealth() > 0)
            {
                enemy->chooseIntent(player);
                emit enemyIntentUpdated(enemy);
            }
        }

        emit statsUpdated();
        checkWinLossCondition();
        if (isBattleFinished())
            return;
    }

    changeState(CombatState::TurnStart);
}

void CombatManager::checkWinLossCondition()
{
    if (!player) return;

    if (player->getCurrentHealth() <= 0)
    {
        Potion* fairy = nullptr;

        for (Potion* p : player->getPotions())
        {
            if (p && p->getName() == "Fairy in a Bottle")
            {
                fairy = p;
                break;
            }
        }

        if (fairy)
        {
            int healAmount = static_cast<int>(player->getMaxHealth() * 0.3);
            player->heal(healAmount);

            player->removePotion(fairy);

            emit statsUpdated();

            return;
        }

        emit combatEnded(false);
        changeState(CombatState::BattleLost);
        return;
    }

    bool hasAliveEnemy = false;

    for (Enemy* enemy : std::as_const(enemies))
    {
        if (enemy && !enemy->isDead())
        {
            hasAliveEnemy = true;
            break;
        }
    }

    if (!hasAliveEnemy)
    {
        emit combatEnded(true);
        changeState(CombatState::BattleWon);
    }
}

void CombatManager::connectEnemy(Enemy* enemy)
{
    if (!enemy)
        return;

    connect(enemy, &Enemy::died, this, &CombatManager::onEnemyDied);
}

void CombatManager::onEnemyDied(Enemy* enemy)
{
    if (!enemy)
    {
        return;
    }

    player->getRelicSystem().onEnemyDeath(player, enemy);

    // Keep the dead enemy in the enemies vector.
    // The UI may still need it to display the death state.
    emit statsUpdated();

    checkWinLossCondition();
}

void CombatManager::beginExhumeSelection(Card* sourceCard)
{
    if (!sourceCard || !player)
        return;

    if (waitingForExhumeSelection)
        return;

    CombatDeck* deck = player->getCombatDeck();
    if (!deck)
        return;

    const QVector<Card*>& exhaustPile = deck->getExhaustPile();

    pendingExhumeCard = sourceCard;
    waitingForExhumeSelection = true;

    if (exhaustPile.isEmpty())
    {
        finalizeExhumeResolution();
        return;
    }

    emit requestPileSelection(PileType::Exhaust);
}

void CombatManager::handleExhumeSelection(Card* selectedCard)
{
    if (!waitingForExhumeSelection || !pendingExhumeCard || !player)
        return;

    CombatDeck* deck = player->getCombatDeck();
    if (!deck)
    {
        finalizeExhumeResolution();
        return;
    }

    if (selectedCard)
    {
        deck->moveFromExhaustToHand(selectedCard);
    }

    finalizeExhumeResolution();
}

void CombatManager::cancelExhumeSelection()
{
    if (!waitingForExhumeSelection)
        return;

    finalizeExhumeResolution();
}

void CombatManager::finalizeExhumeResolution()
{
    if (pendingExhumeCard)
    {
        finalizeCardAfterUse(pendingExhumeCard);
    }

    pendingExhumeCard = nullptr;
    waitingForExhumeSelection = false;

    emit statsUpdated();
    checkWinLossCondition();
}

void CombatManager::finalizeCardAfterUse(Card* card)
{
    if (!player || !card)
        return;

    CombatDeck* deck = player->getCombatDeck();
    if (!deck)
        return;

    if (card->doesExhaust())
        moveCardFromHandToExhaust(card);
    else
        deck->moveFromHandToDiscard(card);
}


CombatState CombatManager::getCurrentState() const
{
    return currentState;
}

const QVector<Enemy*>& CombatManager::getEnemies() const
{
    return enemies;
}

bool CombatManager::isBattleFinished() const
{
    return currentState == CombatState::BattleWon ||
           currentState == CombatState::BattleLost;
}

void CombatManager::decreaseTimedEffects(Character* character)
{
    if (!character)
        return;

    for (Effect* effect : character->getEffects())
    {
        if (!effect)
            continue;

        if (effect->usesDuration() && !effect->isPermanent())
        {
            effect->decreaseDuration();
        }
    }

    character->removeExpiredEffects();
}

void CombatManager::triggerOnCardExhaust(Card* card)
{
    Q_UNUSED(card);

    if (!player)
        return;

    if (Effect* feelNoPain = player->getEffect(Effect::Type::FeelNoPain))
    {
        CombatCalculator::grantBlock(player, feelNoPain->getAmount());
    }

    if (Effect* darkEmbrace = player->getEffect(Effect::Type::DarkEmbrace))
    {
        int drawAmount = darkEmbrace->getAmount();
        if (drawAmount <= 0)
            drawAmount = 1;

        player->drawCards(drawAmount);
    }
}

void CombatManager::moveCardFromHandToExhaust(Card* card)
{
    if (!player || !card)
        return;

    CombatDeck* deck = player->getCombatDeck();
    if (!deck)
        return;

    deck->moveFromHandToExhaust(card);
    triggerOnCardExhaust(card);
}

void CombatManager::cleanupAfterCombat()
{
    if (combatCleanedUp)
        return;

    combatCleanedUp = true;

    if (player)
    {
        player->clearNonPermanentEffects();
        player->clearBlock();
    }

    for (Enemy* enemy : std::as_const(enemies))
    {
        if (!enemy)
            continue;

        enemy->clearNonPermanentEffects();
        enemy->clearBlock();
    }

    emit statsUpdated();
}

void CombatManager::handleEnemySplit(Enemy* enemy)
{
    auto* largeSlime = dynamic_cast<LargeSlime*>(enemy);

    if (!largeSlime || !largeSlime->isSplitRequested())
        return;

    int remainingHp = largeSlime->getCurrentHealth();

    int index = enemies.indexOf(largeSlime);
    if (index == -1)
        return;

    largeSlime->disconnect(this);
    enemies.removeAt(index);

    for (int i = 0; i < 2; ++i)
    {

        MediumSlime* medium = new MediumSlime(remainingHp);

        connectEnemy(medium);

        enemies.insert(index + i, medium);
    }

    largeSlime->deleteLater();

    emit enemiesChanged();
    emit statsUpdated();
}



