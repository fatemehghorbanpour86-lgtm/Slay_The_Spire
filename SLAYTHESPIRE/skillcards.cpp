#include "skillcards.h"
#include "effect.h"
#include "player.h"
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

    user->addBlock(blockAmount);
}
void Defend::upgrade()
{
    if (isUpgraded)
        return;

    isUpgraded = true;
    blockAmount = 8;
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
    description = "Costs 0. Put a card from your exhaust pile into your hand.";
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
    description = "Double your Strength.";
}