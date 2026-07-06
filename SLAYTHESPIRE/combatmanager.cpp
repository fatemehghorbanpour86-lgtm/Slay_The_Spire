#include "combatmanager.h"

#include "player.h"
#include "enemy.h"
#include "card.h"
#include "potion.h"
#include "combatcalculator.h"
#include "combatdeck.h"
#include "relicsystem.h"

#include <utility>

CombatManager::CombatManager(Player* p, QVector<Enemy*> e, QObject* parent)
    : QObject(parent),
    currentState(CombatState::BattleStart),
    player(p),
    enemies(e),
    calculator(new CombatCalculator()),
    turnCount(0)
{
    for (Enemy* enemy : enemies)
    {
        connect(enemy, &Enemy::died,
                this, &CombatManager::onEnemyDied);
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
    player->getRelicSystem().onCombatStart(player);

    for(Enemy* enemy : std::as_const(enemies))
    {
        if(enemy && !enemy->isDead())
        {
            enemy->chooseIntent(player);
        }
    }

    emit combatStarted();

    changeState(CombatState::TurnStart);
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

    if (!player->useEnergy(card->getEnergyCost()))
        return false;

    card->play(player, enemies, target);

    if (card->getType() == CardType::Attack)
    {
        Effect* rage = player->getEffect(Effect::Type::Rage);

        if (rage)
        {
            CombatCalculator::grantBlock(player,rage->getAmount());
        }
    }

    emit cardPlayed(card, target);

    player->getRelicSystem().onCardPlayed(player, card);

    if (player->getCombatDeck())
        player->getCombatDeck()->moveFromHandToDiscard(card);

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
        for (Card* card : deck->getHand())
        {
            if (card && card->getName() == "Regret")
            {
                int handSize = deck->handSize();

                int damage = handSize;

                player->loseHP(damage);
            }
        }

        deck->discardHand();
    }

    player->getRelicSystem().onTurnEnd(player);

    changeState(CombatState::EnemyTurn);
}

void CombatManager::handleEnemyTurn()
{
    emit enemyTurnStarted();

    for (Enemy* enemy : std::as_const(enemies) )
    {
        if (!enemy || enemy->isDead())
            continue;

        enemy->executeMove(player);

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
    if (enemies.isEmpty())
    {
        emit combatEnded(true);
        changeState(CombatState::BattleWon);
        return;
    }

    if (player->getCurrentHealth() <= 0)
    {
        emit combatEnded(false);
        changeState(CombatState::BattleLost);
    }
}

void CombatManager::onEnemyDied(Enemy* enemy)
{
    if (!enemy)
        return;

    player->getRelicSystem().onEnemyDeath(player, enemy);

    enemies.removeOne(enemy);

    enemy->deleteLater();

    emit statsUpdated();

    checkWinLossCondition();
}