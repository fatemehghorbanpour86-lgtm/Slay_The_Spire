#include "player.h"
#include "combatdeck.h"
#include "potion.h"

Player::Player(const QString& name, int maxHealth)
    : Character(name, maxHealth),
    currentEnergy(3),
    maxEnergy(3),
    gold(0),
    combatDeck(nullptr)
{

}

Player::~Player()
{
    delete combatDeck;

    qDeleteAll(potions);
    potions.clear();
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

void Player::increaseMaxHealth(int amount)
{
    if(amount <= 0)
        return;

    maxHealth += amount;
    currentHealth += amount;

    if(currentHealth > maxHealth)
    {
        currentHealth = maxHealth;
    }
}

void Player::increaseMaxEnergy(int amount)
{
    if(amount <= 0)
        return;

    maxEnergy += amount;
}

void Player::addPotion(Potion* potion)
{
    if (potion == nullptr)
        return;

    potions.append(potion);
}

bool Player::removePotion(Potion* potion)
{
    int index = potions.indexOf(potion);

    if(index == -1)
        return false;

    delete potions[index];
    potions.removeAt(index);

    return true;
}


Potion* Player::getPotion(int index) const
{
    if(index < 0 || index >= potions.size())
        return nullptr;

    return potions[index];
}


int Player::getPotionCount() const
{
    return potions.size();
}

const QVector<Potion*>& Player::getPotions() const
{
    return potions;
}

void Player::addRelic(Relic* relic)
{
    relicSystem.addRelic(relic, this);
}

bool Player::hasRelic(RelicId id) const
{
    return relicSystem.hasRelic(id);
}

Relic* Player::getRelic(RelicId id) const
{
    return relicSystem.getRelic(id);
}

const QVector<Relic*>& Player::getAllRelics() const
{
    return relicSystem.getAllRelics();
}

bool Player::canRest() const
{
    return relicSystem.canRest();
}

bool Player::canUsePotions() const
{
    return relicSystem.canUsePotions();
}

bool Player::canPlayCard()
{
    return relicSystem.canPlayCard(this);
}

RelicSystem& Player::getRelicSystem()
{
    return relicSystem;
}

const RelicSystem& Player::getRelicSystem() const
{
    return relicSystem;
}

bool Player::canLift() const
{
    return relicSystem.canLift();
}

bool Player::lift()
{
    return relicSystem.lift(this);
}