#include "enemy.h"

Enemy::Enemy(const QString& name, int maxHealth)
    : Character(name, maxHealth)
{
    currentIntent = Intent::Unknown;

    intentDamage = 0;

    intentHits = 1;

    turnCount = 0;
}

Intent Enemy::getIntent() const
{
    return currentIntent;
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