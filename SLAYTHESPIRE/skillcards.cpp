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