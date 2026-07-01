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
    name = "Brutality+";
    description = "Innate. At the start of your turn, lose 1 HP and draw 1 card.";
}


Barricade::Barricade()
    : Card("Barricade", "Block is not removed at the start of your turn.", 3,
           CardType::Power, false, false, false, false)
{
    // این کارت هیچ منطقی برای "پاک نکردن Block" ندارد - این باید توسط
    // CombatManager رعایت شود: قبل از clearBlock() در شروع نوبت بازیکن، چک کند
    // user->getEffect(Effect::Type::Barricade) و اگر موجود بود clearBlock صدا
    // زده نشود.

    //این کارت امتیازی است***************************************************
}
void Barricade::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Barricade, Effect::Category::Buff, 1);
}
void Barricade::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    setEnergyCost(2);
    name = "Barricade+";
}


FeelNoPain::FeelNoPain()
    : Card("Feel No Pain", "Whenever a card is Exhausted, gain 3 Block.", 1,
           CardType::Power, false, false, false, false),
    blockPerExhaust(3)
{
    //باید CombatManager، هر بار
    // کارتی را به Exhaust Pile منتقل می‌کند، چک کند user->getEffect(FeelNoPain)
    // و در صورت وجود addBlock(amount) صدا بزند


    //این کارت امتیازی است***************************************************
}
void FeelNoPain::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::FeelNoPain, Effect::Category::Buff,
                    blockPerExhaust);
}
void FeelNoPain::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockPerExhaust = 4;
    name = "Feel No Pain+";
    description = "Whenever a card is Exhausted, gain 4 Block.";
}


Berserk::Berserk()
    : Card("Berserk", "Gain 2 Vulnerable. At the start of your turn, gain 1 extra Energy.",
           0, CardType::Power, false, false, false, false),
    vulnerableAmount(2)
{
}
void Berserk::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Vulnerable, Effect::Category::Debuff,
                    vulnerableAmount);
    user->addEffect(Effect::Type::Berserk, Effect::Category::Buff, 1);
}
void Berserk::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    vulnerableAmount = 1;
    name = "Berserk+";
    description = "Gain 1 Vulnerable. At the start of your turn, gain 1 extra Energy.";
}