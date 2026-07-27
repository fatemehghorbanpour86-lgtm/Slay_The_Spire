#include "effect.h"
#include "character.h"
#include "player.h"
#include "combatcalculator.h"

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

    case Type::Brutality:
        return "Brutality";

    case Type::Barricade:
        return "Barricade";

    case Type::FeelNoPain:
        return "Feel No Pain";

    case Type::Berserk:
        return "Berserk";

    case Type::DarkEmbrace:
        return "Dark Embrace";

    case Type::Entangle:
        return "Entangle";
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


QString Effect::getTooltip() const
{
    switch (type)
    {
    case Type::Strength:
        return QString("Attack damage is increased by %1.").arg(amount);

    case Type::Dexterity:
        return QString("Block gained from cards is increased by %1.").arg(amount);

    case Type::Metallicize:
        return QString("At the end of your turn, gain %1 Block.").arg(amount);

    case Type::Weak:
        return QString("Attacks deal 25% less damage.\n%1 turn(s) remaining.").arg(duration);

    case Type::Vulnerable:
        return QString("Attacks against this target deal 50% more damage.\n%1 turn(s) remaining.").arg(duration);

    case Type::Frail:
        return QString("Block gained is reduced by 25%.\n%1 turn(s) remaining.").arg(duration);

    case Type::Rage:
        return QString("nWhenever you play an Attack, gain %1 Block.").arg(amount);

    case Type::DemonForm:
        return QString("At the start of your turn, gain %1 Strength.").arg(amount);

    case Type::Brutality:
        return QString("At the start of your turn, lose %1 HP and draw 1 card.").arg(amount);

    case Type::Barricade:
        return QString("Block is not removed at the start of your turn.");

    case Type::FeelNoPain:
        return QString("Whenever you Exhaust a card, gain %1 Block.").arg(amount);

    case Type::Berserk:
        return QString("At the start of your turn, gain %1 Energy.").arg(amount);

    case Type::DarkEmbrace:
        return QString("Whenever you Exhaust a card, draw 1 card.");

    case Type::Entangle:
        return QString("You cannot play Attack cards.\n%1 turn(s) remaining.").arg(duration);

    case Type::Enrage:
        return QString("Whenever you play a Skill card, this creature gains %1 Strength.").arg(amount);
    }

    return getName();
}

bool Effect::isExpired() const
{
    return usesDuration() && duration == 0;
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
    switch (type)
    {
     case Type::Entangle:
        duration = 1;
        break;

     case Type::Weak:
     case Type::Vulnerable:
     case Type::Frail:
        duration += value;
        break;

     default:
        amount += value;
        break;
    }
}


int Effect::getDisplayValue() const
{
    switch (type)
    {
    case Type::Weak:
    case Type::Vulnerable:
    case Type::Frail:
    case Type::Entangle:
        return duration;

    case Type::Barricade:
    case Type::DarkEmbrace:
        return 0;

    default:
        return amount;
    }
}

bool Effect::shouldShowNumber() const
{
    return getDisplayValue() != 0;
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
    if (!target) return;

    switch(type)
    {
    case Type::DemonForm:
        target->addEffect(Effect::Type::Strength, Effect::Category::Buff, amount);
        break;

    case Type::Brutality:
        if (Player* p = dynamic_cast<Player*>(target))
        {
            p->loseHP(amount);
            p->drawCards(1);
        }
        break;

    case Type::Berserk:
        if (Player* p = dynamic_cast<Player*>(target))
        {
            p->gainEnergy(amount);
        }
        break;

    default:
        break;
    }
}

void Effect::onTurnEnd(Character *target)
{
    if (!target) return;

    switch(type)
    {
    case Type::Metallicize:
        target->addBlock(CombatCalculator::calculateBlock(target, amount));
        break;

    default:
        break;
    }
}


bool Effect::usesDuration() const
{
    switch (type)
    {
    case Type::Weak:
    case Type::Vulnerable:
    case Type::Frail:
    case Type::Entangle:
        return true;
    default:
        return false;
    }
}

bool Effect::isPermanent() const
{
    return duration < 0;
}
