#include "potion.h"

#include "player.h"

#include "enemy.h"

#include "combatcalculator.h"


Potion::Potion(const QString& name, const QString& description)
    : name(name), description(description)
{
}

QString Potion::getName() const
{
    return name;
}

QString Potion::getDescription() const
{
    return description;
}

bool Potion::canUse(Player* user) const
{
    Q_UNUSED(user)

    return true;
}




//======================================================
//  BlockPotion
//======================================================


BlockPotion::BlockPotion()
    : Potion("Block Potion","Gain 12 Block.")
{
    blockAmount = 12;
}

void BlockPotion::use(Player* user, Enemy* target)
{
    Q_UNUSED(target)

    if (!user)
        return;

    CombatCalculator::grantBlock(user, blockAmount);
}

//======================================================
//  FirePotion
//======================================================


FirePotion::FirePotion()
    : Potion("Fire Potion", "Deal 20 damage.")
{
}

void FirePotion::use(Player* user, Enemy* target)
{

    if (!user || !target)
        return;

    CombatCalculator::dealDamage(user, target, 20);
}


//======================================================
//  EnergyPotion
//======================================================

EnergyPotion::EnergyPotion()
    : Potion("Energy Potion", "Gain 2 Energy.")
{
}

void EnergyPotion::use(Player* user, Enemy* target)
{
    Q_UNUSED(target)

    if (!user)
        return;

    user->gainEnergy(2);
}

//======================================================
//  SwiftPotion
//======================================================

SwiftPotion::SwiftPotion()
    : Potion("Swift Potion", "Draw 3 cards.")
{
}

void SwiftPotion::use(Player* user, Enemy* target)
{
    Q_UNUSED(target)

    if (!user)
        return;

    // TODO (Anahita)
    // Draw 3 cards from the CombatDeck.
    // Example: user->drawCards(3);
}


//======================================================
//  Fairy In A Bottle
//======================================================

FairyInABottle::FairyInABottle()
    : Potion("Fairy in a Bottle",
             "When you would die, heal to 30% of your Max HP instead.")
{
}

void FairyInABottle::use(Player* user, Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(target)

    // Unplayable.
    // CombatManager automatically activates this Potion when the player would die.
}

bool FairyInABottle::canUse(Player* user) const
{
    Q_UNUSED(user)
    return false;
}