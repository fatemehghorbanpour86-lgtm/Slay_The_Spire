#include "skillcards.h"
#include "effect.h"
#include "player.h"
#include "combatcalculator.h"

//#include "enemy.h"


Defend::Defend()
    : Card("Defend", "Gain 5 Block.", 1, CardType::Skill,
           false, false, false, false), blockAmount(5)
{
}
void Defend::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)
    if (!user)
        return;

    user->addBlock(CombatCalculator::calculateBlock(user, blockAmount));
}
void Defend::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 8;
    name = "Defend+";
    description = "Gain 8 Block.";
}


Exhume::Exhume()
    : Card("Exhume", "Put a card from your exhaust pile into your hand.", 1,
           CardType::Skill, true, false, false, false)
{
}
void Exhume::play(Player* user,QVector <Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    // TODO: Player::returnRandomCardFromExhaustToHand()
    // Deck/CombatManager (Anahita)
    //user->returnRandomCardFromExhaustToHand();
}
void Exhume::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    setEnergyCost(0);
    name = "Exhume+";
}


LimitBreak::LimitBreak()
    : Card("Limit Break", "Double your Strength.", 1, CardType::Skill,
           true, false, false, false)
{
}
void LimitBreak::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    Effect* strength = user->getEffect(Effect::Type::Strength);

    if (strength)
    {
        user->addEffect(Effect::Type::Strength, Effect::Category::Buff,
                        strength->getAmount());
    }
}
void LimitBreak::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    isExhaust = false;
    name = "Limit Break+";
    description = "Double your Strength.";
}


Offering::Offering()
    : Card("Offering", "Lose 6 HP. Gain 2 Energy. Draw 3 cards.", 0,
           CardType::Skill, true, false, false, false), drawcards(3)
{
}
void Offering::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->loseHP(6);
    user->gainEnergy(2);

    // TODO: Player::drawCards(int) - Deck/CombatManager (Anahita)
    //user->drawCards(drawcards);
}
void Offering::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    drawcards = 5;
    name = "Offering+";
    description = "Lose 5 HP. Gain 2 Energy. Draw 3 cards.";
}


Impervious::Impervious()
    : Card("Impervious", "Gain 30 Block.", 2, CardType::Skill,
           true, false, false, false), blockAmount(30)
{
}
void Impervious::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addBlock(CombatCalculator::calculateBlock(user, blockAmount));

}
void Impervious::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 40;
    name = "Impervious+";
    description = "Gain 40 Block.";
}


ShrugItOff::ShrugItOff()
    : Card("Shrug It Off", "Draw 1 card. Gain 8 Block.", 1, CardType::Skill,
           false, false, false, false),
    blockAmount(8)
{
}
void ShrugItOff::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    // TODO: Player::drawCards(int) — Deck/CombatManager (Anahita)
    //user->drawCards(1);
    user->addBlock(CombatCalculator::calculateBlock(user, blockAmount));
}
void ShrugItOff::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 11;
    name = "Shrug It Off+";
    description = "Draw 1 card. Gain 11 Block.";
}


TrueGrit::TrueGrit()
    : Card("True Grit", "Gain 8 Block. Exhaust a random card in your hand.",
           1, CardType::Skill, false, false, false, false), blockAmount(7)
{
}
void TrueGrit::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addBlock(CombatCalculator::calculateBlock(user, blockAmount));

    // TODO: Deck/CombatManager - (َAnahita)
    // if (isUpgraded)
    //     user->exhaustChosenCardInHand();
    // else
    //     user->exhaustRandomCardInHand();
}
void TrueGrit::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 9;
    name = "True Grit+";
    description = "Gain 9 Block. Exhaust a card in your hand.";
}


Rage::Rage()
    : Card("Rage", "Whenever you play an Attack this turn, gain 3 Block.",
           0, CardType::Skill, false, false, false, false), blockPerAttack(3)
{
    //فقط یک Effect از نوع Rage با duration=1 میسازد (یعنی همین نوبت).
    // خودِ کارت هیچ منطقی برای "هر بار Attack بازی شد" ندارد - این باید توسط
    // CombatManager، هر بار یک کارت Attack بازی می‌شود، چک شود: اگر
    // user->getEffect(Effect::Type::Rage) موجود بود، addBlock(amount) صدا زده شود.
    // در پایان نوبت Effect خودش با onTurnEnd/decreaseDuration از بین میرود
}
void Rage::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)
    Q_UNUSED(enemies)

    if (!user)
        return;

    user->addEffect(Effect::Type::Rage, Effect::Category::Buff,
                    blockPerAttack, 1);
}
void Rage::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockPerAttack = 5;
    name = "Rage+";
    description = "Whenever you play an Attack this turn, gain 5 Block.";
}