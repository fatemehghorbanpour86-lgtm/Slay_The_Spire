#include "potion.h"

#include "player.h"

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

void BlockPotion::use(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(enemies)
    Q_UNUSED(target)

    if (!user)
        return;

    CombatCalculator::grantBlock(user, blockAmount);
}
