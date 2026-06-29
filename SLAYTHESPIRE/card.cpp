#include "card.h"


Card::Card(const QString& name, const QString& description, int energyCost, CardType type,
           bool exhaust, bool ethereal, bool retain, bool unplayable)
    : name(name),
    description(description),
    energyCost(energyCost),
    type(type),
    isExhaust(exhaust),
    isEthereal(ethereal),
    isRetain(retain),
    isUnplayable(unplayable),
    isUpgraded(false)
{
}


QString Card::getName() const
{
    return name;
}

QString Card::getDescription() const
{
    return description;
}

int Card::getEnergyCost() const
{
    return energyCost;
}

CardType Card::getType() const
{
    return type;
}

bool Card::doesExhaust() const
{
    return isExhaust;
}

bool Card::doesEthereal() const
{
    return isEthereal;
}

bool Card::doesRetain() const
{
    return isRetain;
}

bool Card::canPlay() const
{
    return !isUnplayable;
}

bool Card::getIsUpgraded() const
{
    return isUpgraded;
}

void Card::setEnergyCost(int cost)
{
    if(cost < 0)
    {
        cost = 0;
    }

    energyCost = cost;
}
