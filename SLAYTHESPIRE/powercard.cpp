#include "powercard.h"
#include "player.h"
#include "effect.h"

Inflame::Inflame()
    : Card("Inflame", "Gain 2 Strength.", 1, CardType::Power,
           false, false, false, false),
    strengthAmount(2)
{
}
void Inflame::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Strength, Effect::Category::Buff,
                    strengthAmount);
}
void Inflame::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    strengthAmount = 3;
    name = "Inflame+";
    description = "Gain 3 Strength.";
}


Metallicize::Metallicize()
    : Card("Metallicize", "At the end of your turn, gain 3 Block.", 1,
           CardType::Power, false, false, false, false), blockAmount(3)
{
}
void Metallicize::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Metallicize, Effect::Category::Buff,
                    blockAmount);
}
void Metallicize::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 4;
    name = "Metallicizie+";
    description = "At the end of your turn, gain 4 Block.";
}


DemonForm::DemonForm()
    : Card("Demon Form", "At the start of your turn, gain 2 Strength.", 3,
           CardType::Power, false, false, false, false), strengthPerTurn(2)
{
}
void DemonForm::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::DemonForm, Effect::Category::Buff,
                    strengthPerTurn);
}
void DemonForm::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    strengthPerTurn = 3;
    name = "Demon Form+";
    description = "At the start of your turn, gain 3 Strength.";
}


Brutality::Brutality()
    : Card("Brutality", "At the start of your turn, lose 1 HP and draw 1 card.",
           0, CardType::Power, false, false, false, false)
{
}
void Brutality::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Brutality, Effect::Category::Buff,
                    1);
}
void Brutality::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    isInnate = true;
    description = "Innate. At the start of your turn, lose 1 HP and draw 1 card.";
}