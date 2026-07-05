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

    changeState(CombatState::TurnStart);
}

void CombatManager::handleTurnStart()
{
    turnCount++;

    player->resetEnergy();

    if (player->getCombatDeck())
        player->getCombatDeck()->drawCards(5);

    player->getRelicSystem().onTurnStart(player);

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
    if (player->getCombatDeck())

        player->getCombatDeck()->discardHand();

    player->getRelicSystem().onTurnEnd(player);

    changeState(CombatState::EnemyTurn);
}

void CombatManager::handleEnemyTurn()
{
    for (Enemy* enemy : std::as_const(enemies) )
    {
        if (!enemy || enemy->isDead())
            continue;

        enemy->executeMove(player);

        emit statsUpdated();

        checkWinLossCondition();

        if (currentState == CombatState::BattleLost)
            return;
    }

    changeState(CombatState::TurnStart);
}

void CombatManager::checkWinLossCondition()
{
    if (enemies.isEmpty())
    {
        changeState(CombatState::BattleWon);
        return;
    }

    if (player->getCurrentHealth() <= 0)
    {
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