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


    if(Effect* dexterity = getEffect(Effect::Type::Dexterity))
    {
        amount += dexterity->getAmount();
    }
    if(getEffect(Effect::Type::Frail))
    {
        amount = static_cast<int>(amount * 0.75);
    }

    block += amount;
}

void Character::loseBlock(int amount)
{
    removeBlock(amount);
}

void Character::clearBlock()
{
    block = 0;
}

void Character::startTurnBlockReset()
{
    if(getEffect(Effect::Type::Barricade) == nullptr)
    {
        clearBlock();
    }
    // If Barricade is active, block is intentionally left untouched.
}

bool Character::hasBlock() const
{
    return block > 0;
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

void Character::onTurnStartEffects()
{
    for (Effect* e : std::as_const(activeEffects))
    {
        if (e != nullptr)
            e->onTurnStart(this);
    }
    // Explain:
    // Called by CombatManager at the start of this character's turn.
    // Loops through all active effects and triggers their onTurnStart logic
    // (e.g. DemonForm, Brutality, Berserk).
}

void Character::onTurnEndEffects()
{
    for (Effect* e : std::as_const(activeEffects))
    {
        if (e != nullptr)
            e->onTurnEnd(this); // also calls decreaseDuration()
    }

    removeExpiredEffects();

    // Explain:
    // Called by CombatManager at the end of this character's turn.
    // Loops through all active effects, triggers onTurnEnd logic
    // (e.g. Metallicize), and then removes expired effects.

}

void Character::removeExpiredEffects()
{
    for (int i = activeEffects.size() - 1; i >= 0; --i)
    {
        Effect* e = activeEffects[i];

        if (e != nullptr && e->isExpired())
        {
            activeEffects.removeAt(i);
            delete e;
        }
    }

    // Explain:
    // Deletes and removes any effect with duration == 0.
    // Called automatically after onTurnEnd(); keeps activeEffects clean
    // so UI and combat logic never see stale/expired effects.
}

void Character::removeEffect(Effect::Type type)
{
    for (int i = 0; i < activeEffects.size(); ++i)
    {
        if (activeEffects[i]->getType() == type)
        {
            delete activeEffects[i];
            activeEffects.removeAt(i);
            return;
        }
    }

    // Explain:
    // Manually removes a specific effect type (e.g. used by CombatManager
    // for event-based effects like consuming Rage after it fires, or by
    // relic/card logic that cleanses a debuff).
}

bool Character::hasEffect(Effect::Type type) const
{
    return getEffect(type) != nullptr;

    // Explain:
    // Quick check used by CombatManager/CombatCalculator to see if a
    // character currently has a given effect (e.g. "if(hasEffect(Barricade))
    // skip clearing block").
}

const QVector<Effect*>& Character::getEffects() const
{
    return activeEffects;

    // Explain:
    // Read-only access to all active effects, used by UI (to display
    // buff/debuff icons) and by CombatManager for event-based checks
    // (e.g. iterating to find Rage/FeelNoPain/DarkEmbrace).
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