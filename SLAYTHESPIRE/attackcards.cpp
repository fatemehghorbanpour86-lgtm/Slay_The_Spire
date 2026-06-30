#include "attackcards.h"


Strike::Strike() : Card("Strike", "Deal 6 damage.", 1, CardType::Attack)
{
    this->damageAmount = 6;
}


void Strike::play(Player* user , std ::vector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)

    if (target != nullptr)
    {
       // Apply damage through CombatCalculator
    }
}

void Strike::upgrade()
{
    if (!isUpgraded)
    {
        isUpgraded = true;
        this->damageAmount = 9;
        description = "Deal 9 damage.";
    }
}


Reaper::Reaper() : Card("Reaper", "Deal 4 damage to ALL enemies. Heal HP equal to unblocked damage dealt.",2, CardType::Attack)
{
    this->damageAmount = 4;
}

void Reaper::play(Player* user, std::vector<Enemy*>& enemies, Enemy* target)
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

    damageAmount += 1;

    description =
        "Deal 5 damage to ALL enemies. Heal HP equal to unblocked damage dealt.";
}
