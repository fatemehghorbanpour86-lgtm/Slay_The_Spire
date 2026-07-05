#include "campfire.h"
#include "player.h"
#include "card.h"

#include <cmath>

bool Campfire::canRest(Player* player) const
{
    if (player == nullptr)
        return false;

    return player->canRest();
}

bool Campfire::rest(Player* player)
{
    if (!canRest(player))
        return false;

    // Doc: heal 20% of Max HP, rounded UP, capped at Max HP.
    int healAmount = static_cast<int>(std::ceil(player->getMaxHealth() * 0.20));

    player->heal(healAmount); // Character::heal() already clamps to maxHealth.

    return true;
}

bool Campfire::canSmith(Player* player, Card* card) const
{
    if (player == nullptr || card == nullptr)
        return false;

    if (card->getIsUpgraded())
        return false;

    return true;
}

bool Campfire::smith(Player* player, Card* card)
{
    if (!canSmith(player, card))
        return false;

    card->upgrade();

    return true;
}

bool Campfire::canLift(Player* player) const
{
    if (player == nullptr)
        return false;

    return player->canLift();
}

bool Campfire::lift(Player* player)
{
    if (!canLift(player))
        return false;

    return player->lift();
}