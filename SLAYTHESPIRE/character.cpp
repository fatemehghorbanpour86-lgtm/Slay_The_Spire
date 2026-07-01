#include "character.h"
#include <utility>

Character::Character()
{
    name = "";

    maxHealth = 0;
    currentHealth = 0;

    block = 0;
}

Character::Character(const QString& name, int maxHealth)
{
    this->name = name;

    this->maxHealth = maxHealth;
    currentHealth = maxHealth;

    block = 0;
}

Character::~Character()
{
    for(Effect* effect : std::as_const(activeEffects))
    {
        delete effect;
    }

    activeEffects.clear();
}

QString Character::getName() const
{
    return name;
}

int Character::getCurrentHealth() const
{
    return currentHealth;
}

int Character::getMaxHealth() const
{
    return maxHealth;
}

int Character::getBlock() const
{
    return block;
}

void Character::setBlock(int value)
{
    if(value < 0)
        value = 0;

    block = value;
}

void Character::addBlock(int amount)
{
    if(amount <= 0)
        return;

    // TODO
    // Receives final block value.
    // CombatCalculator handles Dexterity and other modifiers.

    block += amount;
}

void Character::clearBlock()
{
    block = 0;
}

void Character::removeBlock(int amount)
{
    if(amount <= 0)
        return;

    if(amount >= block)
        block = 0;
    else
        block -= amount;
}

int Character::absorbBlock(int damage)
{
    if(block >= damage)
    {
        removeBlock(damage);
        return 0;
    }

    damage -= block;
    block = 0;

    return damage;
}

void Character::takeDamage(int damage)
{
    if(damage <= 0)
        return;

    // TODO
    // Receives final calculated damage.
    // CombatCalculator applies Strength, Weak, Vulnerable and similar effects.

    damage = absorbBlock(damage);

    if(damage > 0)
    {
        currentHealth -= damage;

        if(currentHealth < 0)
            currentHealth = 0;

    }

}

void Character::heal(int amount)
{
    if(amount <= 0)
        return;

    currentHealth += amount;

    if(currentHealth > maxHealth)
        currentHealth = maxHealth;

}

void Character::addEffect(Effect::Type type, Effect::Category category, int amount, int duration)
{
    for (Effect* e : std::as_const(activeEffects))
    {
        if (e->getType() == type)
        {
            e->stack(amount);
            return;
        }
    }
    activeEffects.append(new Effect(type, category, amount, duration));
}

Effect* Character::getEffect(Effect::Type type) const
{
    for (Effect* e : activeEffects)
    {
        if (e->getType() == type)
            return e;
    }
    return nullptr;
}


void Character::loseHP(int amount)
{
    if (amount <= 0) return;
    currentHealth -= amount;
    if (currentHealth < 0) currentHealth = 0;
}


bool Character::isDead() const
{
    return currentHealth == 0;
}