#include "combatcalculator.h"

#include "character.h"
#include "player.h"
#include "enemy.h"
#include "effect.h"

#include <algorithm>

// Damage Calculation

int CombatCalculator::calculateDamage(Character* attacker, Character* defender, int baseDamage)
{
    if(attacker == nullptr || defender == nullptr)
        return 0;

    int damage = baseDamage;

    // Strength

    if(Effect* strength = attacker->getEffect(Effect::Type::Strength))
    {
        damage += strength->getAmount();
    }

    // Weak

    if(attacker->hasEffect(Effect::Type::Weak))
    {
        damage = static_cast<int>(damage * 0.75);
    }

    // Vulnerable

    if(defender->hasEffect(Effect::Type::Vulnerable))
    {
        damage = static_cast<int>(damage * 1.5);
    }

    if (Player* pAttacker = dynamic_cast<Player*>(attacker))
    {
        damage = pAttacker->getRelicSystem().modifyDamageDealt(damage);
    }

    if (Player* pDefender = dynamic_cast<Player*>(defender))
    {
        damage = pDefender->getRelicSystem().modifyDamageTaken(damage);
    }

    damage = std::max(0, damage);

    return damage;
}

// Deal Damage

int CombatCalculator::dealDamage(Character* attacker, Character* defender, int baseDamage)
{
    if(attacker == nullptr || defender == nullptr)
        return 0;

    int finalDamage = calculateDamage(attacker, defender, baseDamage);

    int hpBefore = defender->getCurrentHealth();

    defender->takeDamage(finalDamage);

    int hpAfter = defender->getCurrentHealth();

    return hpBefore - hpAfter;
}

// Enemy Intent Damage

int CombatCalculator::calculateIntentDamage(Enemy* attacker, Player* defender,int baseDamage)
{
    return calculateDamage(attacker, defender, baseDamage);
}

// Block Calculation

int CombatCalculator::calculateBlock(Character* target, int baseBlock)
{
    if(target == nullptr)
        return 0;

    int block = baseBlock;

    // Dexterity

    if(Effect* dexterity = target->getEffect(Effect::Type::Dexterity))
    {
        block += dexterity->getAmount();
    }

    // Frail

    if(target->hasEffect(Effect::Type::Frail))
    {
        block = static_cast<int>(block * 0.75);
    }

    return std::max(0, block);
}

// Grant Block

void CombatCalculator::grantBlock(Character* target,int baseBlock)
{
    if(target == nullptr)
        return;

    target->addBlock(calculateBlock(target, baseBlock));
}

// Heal

void CombatCalculator::healCharacter(Character* target, int amount)
{
    if(target == nullptr)
        return;

    target->heal(amount);
}

// Fatal Check

bool CombatCalculator::willBeFatal(Character* attacker, Character* defender,int baseDamage)
{
    if(attacker == nullptr || defender == nullptr)
        return false;

    int finalDamage = calculateDamage(attacker, defender,baseDamage);

    int remainingBlock = defender->getBlock();

    int hpDamage = std::max(0, finalDamage - remainingBlock);

    return hpDamage >= defender->getCurrentHealth();
}