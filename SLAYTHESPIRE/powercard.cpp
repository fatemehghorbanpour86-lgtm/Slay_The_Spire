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