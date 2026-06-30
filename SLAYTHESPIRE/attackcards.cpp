#include "attackcards.h"

// Strike

Strike::Strike() : Card("Strike", "Deal 6 damage.", 1, CardType::Attack)
{
    damageAmount = 6;
}


void Strike::play(Player* user , QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

    //target->takeDamage(damageAmount);
    //TODO anahita
    // Replace with CombatCalculator after combat system is implemented.

}

void Strike::upgrade()
{
    if (!isUpgraded)
    {
        isUpgraded = true;

        name = "Strike+";

        damageAmount = 9;

        description = "Deal 9 damage.";
    }
}

//Reaper

Reaper::Reaper() : Card("Reaper", "Deal 4 damage to ALL enemies. Heal HP equal to unblocked damage dealt.",2, CardType::Attack)
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

    //int totalHeal = 0;

    for(Enemy* enemy : enemies)
    {
        if(enemy == nullptr)
        {
            continue;
        }

        //enemy->takeDamage(damageAmount);
        // TODO :
        // Deal damage using CombatCalculator.
        // CombatCalculator should return the actual HP damage
        // after applying Strength, Weak, Vulnerable and Block.

        // Example:
        // int dealtDamage =
        // CombatCalculator::dealDamage(user, enemy, damageAmount);
        // totalHeal += dealtDamage;
    }

    // TODO:
    // Heal the player by the total unblocked damage dealt.
    // Example:
    // user->heal(totalHeal);
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
        "Deal 5 damage to ALL enemies. Heal HP equal to unblocked damage dealt.";
}

//  Bludgeon

Bludgeon::Bludgeon()
    : Card("Bludgeon", "Deal 32 damage.", 3, CardType::Attack)
{
    damageAmount = 32;
}

void Bludgeon::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)

    if(target == nullptr)
    {
        return;
    }

    //target->takeDamage(damageAmount);
    //TODO anahita
    // Replace with CombatCalculator after combat system is implemented.
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

//Feed
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

    //target->takeDamage(damageAmount);

    // TODO combat anahita:
    // If this attack kills the target,
    // increase the player's Max HP by 3.
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


