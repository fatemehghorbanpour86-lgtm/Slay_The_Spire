#include "attackcards.h"


Strike::Strike() : Card("Strike", "Deal 6 damage.", 1, CardType::Attack)
{
    this->damageAmount = 6;
}


void Strike::play(Player* user, Enemy* target)
{
    Q_UNUSED(user)

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
