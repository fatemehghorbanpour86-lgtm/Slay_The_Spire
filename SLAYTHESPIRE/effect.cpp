#include "effect.h"
#include "character.h"
#include "player.h"

Effect::Effect(Type type, Category category, int amount, int duration)
{
    this->type = type;
    if((type == Type::Strength || type == Type::Dexterity) && (amount < 0))
    {
        this->category = Category::Debuff;
    }
    else
    {
        this->category = category;
    }
    this->amount = amount;
    this->duration = duration;
}

QString Effect::getName() const
{
    switch(type)
    {
    case Type::Strength:
        return "Strength";

    case Type::Dexterity:
        return "Dexterity";

    case Type::Metallicize:
        return "Metallicize";

    case Type::Weak:
        return "Weak";

    case Type::Vulnerable:
        return "Vulnerable";

    case Type::Frail:
        return "Frail";

    case Type::Rage:
        return "Rage";

    case Type::DemonForm:
        return "Demon Form";
    }
    return "";
}

Effect::Type Effect::getType() const
{
    return type;
}

Effect::Category Effect::getCategory() const
{
    return category;
}

int Effect::getAmount() const
{
    return amount;
}

int Effect::getDuration() const
{
    return duration;
}

bool Effect::isExpired() const
{
    return duration == 0;
}

bool Effect::isBuff() const
{
    return category == Category::Buff;
}

bool Effect::isDebuff() const
{
    return category == Category::Debuff;
}

void Effect::stack(int value)
{
    amount += value;
}

void Effect::decreaseDuration()
{
    if(duration > 0)
    {
        duration--;
    }
}

void Effect::onTurnStart(Character *target)
{
    Q_UNUSED(target)

    switch(type)
    {
    case Type::Strength:
        break;

    case Type::Dexterity:
        break;

    case Type::Weak:
        break;

    case Type::Vulnerable:
        break;

    case Type::Frail:
        break;

    case Type::Metallicize:
        break;

    case Type::Rage:
        break;

    case Type::DemonForm:
        target->addEffect(Effect::Type::Strength, Effect::Category::Buff, amount);
        break;
    }
}

void Effect::onTurnEnd(Character *target)
{
    switch(type)
    {

    case Type::Metallicize:
        target->addBlock(amount);
        break;

    default:
        break;

    }

    decreaseDuration();
}