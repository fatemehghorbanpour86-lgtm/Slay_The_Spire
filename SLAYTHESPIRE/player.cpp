#include "player.h"

Player::Player(const QString& name, int maxHealth)
    : Character(name, maxHealth),
    currentEnergy(3),
    maxEnergy(3),
    gold(0)
{

}


int Player::getCurrentEnergy() const
{
    return currentEnergy;
}


int Player::getMaxEnergy() const
{
    return maxEnergy;
}


void Player::setEnergy(int value)
{
    if(value < 0)
        value = 0;

    currentEnergy = value;
}


void Player::gainEnergy(int amount)
{
    if(amount <= 0)
        return;

    setEnergy(currentEnergy + amount);
}

// for playing cards
bool Player::useEnergy(int amount)
{
    if(amount <= 0)
        return true;

    if(currentEnergy < amount)
        return false;

    currentEnergy -= amount;

    return true;
}


void Player::resetEnergy()
{
    currentEnergy = maxEnergy;

    // TODO (Anahita)
    // Apply card effects that change Energy.


    // TODO (Fateme)
    // Apply Relic effects that grant Energy.
}


int Player::getGold() const
{
    return gold;
}


void Player::gainGold(int amount)
{
    if(amount <= 0)
        return;

    gold += amount;
}

// for shop
bool Player::spendGold(int amount)
{
    if(amount <= 0)
        return true;

    if(gold < amount)
        return false;

    gold -= amount;

    return true;
}