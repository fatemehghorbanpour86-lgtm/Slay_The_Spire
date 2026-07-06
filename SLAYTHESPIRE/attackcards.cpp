#include "attackcards.h"
#include "player.h"
#include "enemy.h"
#include "combatcalculator.h"
#include "statuscards.h"

//======================================================
//  Strike
//======================================================

Strike::Strike() : Card("Strike", "Deal 6 damage.", 1, CardType::Attack)
{
    damageAmount = 6;
}


void Strike::play(Player* user , QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

   CombatCalculator::dealDamage(user, target, damageAmount);

}

void Strike::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Strike+";

    damageAmount = 9;

    description = "Deal 9 damage.";
}

Card* Strike::clone() const
{
    return new Strike(*this);
}

//======================================================
//  Reaper
//======================================================

Reaper::Reaper() : Card("Reaper", "Deal 4 damage to ALL enemies. Heal HP equal to unblocked damage dealt. Exhaust.",2, CardType::Attack, true)
{
    damageAmount = 4;
}

void Reaper::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target)

    if(user == nullptr)
    {
        return;
    }

    int totalHeal = 0;

    for(Enemy* enemy : enemies)
    {
        if(enemy == nullptr)
        {
            continue;
        }

        totalHeal += CombatCalculator::dealDamage(user,enemy, damageAmount);
    }

    CombatCalculator::healCharacter( user,totalHeal);
}


void Reaper::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Reaper+";

    damageAmount += 1;

    description =
        "Deal 5 damage to ALL enemies. Heal HP equal to unblocked damage dealt. Exhaust";
}

Card* Reaper::clone() const
{
    return new Reaper(*this);
}

//======================================================
//  Bludgeon
//======================================================

Bludgeon::Bludgeon()
    : Card("Bludgeon", "Deal 32 damage.", 3, CardType::Attack)
{
    damageAmount = 32;
}

void Bludgeon::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

   CombatCalculator::dealDamage(user, target, damageAmount);
}

void Bludgeon::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Bludgeon+";

    damageAmount += 10;

    description = "Deal 42 damage.";
}

Card* Bludgeon::clone() const
{
    return new Bludgeon(*this);
}

//======================================================
//  Feed
//======================================================

Feed::Feed()
    : Card("Feed", "Deal 10 damage. If Fatal, raise your Max HP by 3.",1,CardType::Attack)
{
    damageAmount = 10;
}


void Feed::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies);

    if(user == nullptr || target == nullptr)
    {
        return;
    }

    CombatCalculator::dealDamage( user,target, damageAmount);

    if(target->isDead())
    {
        user->increaseMaxHealth(3);
    }

}

void Feed::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Feed+";

    damageAmount += 2;

    description = "Deal 12 damage. If Fatal, raise your Max HP by 4.";
}

Card* Feed::clone() const
{
    return new Feed(*this);
}

//======================================================
//  Immolate
//======================================================

Immolate::Immolate()
    : Card("Immolate", "Deal 21 damage to ALL enemies. Add a Burn into your Discard Pile.", 2,CardType::Attack)
{
    damageAmount = 21;
}

void Immolate::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(target);

    if(user == nullptr)
    {
        return;
    }

    for(Enemy* enemy : enemies)
    {
        if(enemy == nullptr)
        {
            continue;
        }

        CombatCalculator::dealDamage(user, enemy, damageAmount);
    }

    user->addCardToDiscardPile(new Burn());
}

void Immolate::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Immolate+";

    damageAmount = 28;

    description = "Deal 28 damage to ALL enemies. Add a Burn into your Discard Pile.";
}

Card* Immolate::clone() const
{
    return new Immolate(*this);
}

//======================================================
//  TwinStrike
//======================================================

TwinStrike::TwinStrike()
    : Card("Twin Strike","Deal 5 damage twice.",1,CardType::Attack)
{
    damageAmount = 5;
}


void TwinStrike::play(Player* user, QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

    CombatCalculator::dealDamage(user, target, damageAmount);

    CombatCalculator::dealDamage(user, target, damageAmount);
}

void TwinStrike::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Twin Strike+";

    damageAmount = 7;

    description = "Deal 7 damage twice.";
}

Card* TwinStrike::clone() const
{
    return new TwinStrike(*this);
}

//======================================================
//  Hemokinesis
//======================================================

Hemokinesis::Hemokinesis()
    : Card("Hemokinesis", "Lose 2 HP. Deal 15 damage.",1,CardType::Attack)
{
    damageAmount = 15;
    selfDamage = 2;
}


void Hemokinesis::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies);

    if(user == nullptr || target == nullptr)
    {
        return;
    }

    user->loseHP(selfDamage);

    CombatCalculator::dealDamage(user, target, damageAmount);
}


void Hemokinesis::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Hemokinesis+";

    damageAmount = 20;

    description = "Lose 2 HP. Deal 20 damage.";
}

Card* Hemokinesis::clone() const
{
    return new Hemokinesis(*this);
}

//======================================================
//  Carnage
//======================================================

Carnage::Carnage()
    : Card("Carnage","Deal 20 damage. Ethereal.", 2, CardType::Attack, false, true)
{
    damageAmount = 20;
}


void Carnage::play(Player* user, QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

   CombatCalculator::dealDamage(user, target, damageAmount);
}

void Carnage::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    name = "Carnage+";

    damageAmount = 28;

    description = "Deal 28 damage. Ethereal.";
}

Card* Carnage::clone() const
{
    return new Carnage(*this);
}