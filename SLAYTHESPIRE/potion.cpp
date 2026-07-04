#include "potion.h"


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
