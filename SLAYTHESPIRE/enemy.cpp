#include "enemy.h"

Enemy::Enemy(const QString& name, int maxHealth)
    : Character(name, maxHealth)
{
    currentIntent = Intent::Unknown;

    currentMove = 0;

    intentDamage = 0;

    intentHits = 1;

    turnCount = 0;
}

void Enemy::takeDamage(int amount)
{
    if (isDead())
    {
        return;
    }

    Character::takeDamage(amount);

    if (isDead())
    {
        emit died(this);
    }
}

Intent Enemy::getIntent() const
{
    return currentIntent;
}

int Enemy::getCurrentMove() const
{
    return currentMove;
}

int Enemy::getIntentDamage() const
{
    return intentDamage;
}

int Enemy::getIntentHits() const
{
    return intentHits;
}

int Enemy::getTurnCount() const
{
    return turnCount;
}

void Enemy::setIntent(Intent intent)
{
    currentIntent = intent;
}

void Enemy::setCurrentMove(int move)
{
    currentMove = move;
}

void Enemy::setIntentDamage(int damage)
{
    if(damage < 0)
    {
        damage = 0;
    }

    intentDamage = damage;
}

void Enemy::setIntentHits(int hits)
{
    if(hits < 1)
    {
        hits = 1;
    }

    intentHits = hits;
}

void Enemy::increaseTurnCount()
{
    turnCount++;
}