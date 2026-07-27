#include "combatmanager.h"

#include "player.h"
#include "enemy.h"
#include "card.h"
#include "potion.h"
#include "combatcalculator.h"
#include "combatdeck.h"
#include "relicsystem.h"
#include "statuscards.h"
#include "eliteenemy.h"

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
        emit battleWon();
        break;

    case CombatState::BattleLost:
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
    turnCount++;

    if (player->getEffect(Effect::Type::Barricade) == nullptr)
    {
        player->clearBlock();
    }

    player->resetEnergy();
    player->drawCards(5);
    player->getRelicSystem().onTurnStart(player);

    emit playerTurnStarted();
    emit statsUpdated();

    changeState(CombatState::PlayerAction);
}

bool CombatManager::playCard(Card* card, Enemy* target)
{
    if (currentState != CombatState::PlayerAction)
        return false;

    if (!player || !card)
        return false;

    if (!card->canPlay())
        return false;

    if (!player->useEnergy(card->getEnergyCost()))
        return false;

    // Common "on play" reactions that should still happen for Exhume
    if (card->getType() == CardType::Attack)
    {
        Effect* rage = player->getEffect(Effect::Type::Rage);

        if (rage)
        {
            CombatCalculator::grantBlock(player, rage->getAmount());
        }
    }

    if (card->getType() == CardType::Skill)
    {
        for (Enemy* enemy : std::as_const(enemies))
        {
            GremlinNob* nob = dynamic_cast<GremlinNob*>(enemy);
            if (nob && nob->isEnraged())
            {
                nob->addEffect(Effect::Type::Strength, Effect::Category::Buff, 2);
            }
        }
    }

    emit cardPlayed(card, target);
    player->getRelicSystem().onCardPlayed(player, card);

    // Special delayed resolution for Exhume
    if (card->getName() == "Exhume")
    {
        beginExhumeSelection(card);
        emit statsUpdated();
        checkWinLossCondition();
        return true;
    }

    // Normal cards resolve immediately
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

    if (!potion || !player)
        return false;

    if (!player->getRelicSystem().canUsePotions())
        return false;

    if (!potion->canUse(player))
        return false;

    potion->use(player, target);

    emit statsUpdated();
    checkWinLossCondition();

    return true;
}

void CombatManager::endTurn()
{
    if (currentState == CombatState::PlayerAction)
        changeState(CombatState::TurnEnd);
}

void CombatManager::handleTurnEnd()
{
    CombatDeck* deck = player->getCombatDeck();

    if (deck)
    {
        QVector<Card*> handCards = deck->getHand();

        // 1) End-of-turn effects for cards still in hand
        for (Card* card : handCards)
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
                Burn* burn = dynamic_cast<Burn*>(card);

                if (burn)
                {
                    CombatCalculator::dealDamage(nullptr, player, burn->getDamageAmount());
                }
            }
        }

        emit statsUpdated();

        checkWinLossCondition();
        if (currentState == CombatState::BattleLost || currentState == CombatState::BattleWon)
            return;

        // 2) Move cards based on Ethereal / Retain / Normal
        handCards = deck->getHand();

        for (Card* card : std::as_const(handCards))
        {
            if (!card)
                continue;

            if (card->doesEthereal())
            {
                deck->moveFromHandToExhaust(card);
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

    player->getRelicSystem().onTurnEnd(player);

    emit statsUpdated();

    checkWinLossCondition();
    if (currentState == CombatState::BattleLost || currentState == CombatState::BattleWon)
        return;

    changeState(CombatState::EnemyTurn);
}

void CombatManager::handleEnemyTurn()
{
    emit enemyTurnStarted();

    for (Enemy* enemy : std::as_const(enemies))
    {
        if (!enemy || enemy->isDead())
            continue;

        emit enemyAttacking(enemy);

        enemy->executeMove(player);
        enemy->finishTurn();

        if (!enemy->isDead() && player->getCurrentHealth() > 0)
        {
            enemy->chooseIntent(player);
            emit enemyIntentUpdated(enemy);
        }

        emit statsUpdated();

        checkWinLossCondition();

        if (currentState == CombatState::BattleLost || currentState == CombatState::BattleWon)
            return;
    }

    changeState(CombatState::TurnStart);
}

void CombatManager::checkWinLossCondition()
{
    if (!player)
    {
        return;
    }

    if (player->getCurrentHealth() <= 0)
    {
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
        deck->moveFromHandToExhaust(card);
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
