#ifndef CAMPFIRE_H
#define CAMPFIRE_H

class Player;
class Card;

class Campfire
{
public:
    bool rest(Player* player);
    bool smith(Player* player, Card* card);
    bool lift(Player* player);

    bool canRest(Player* player) const;
    bool canSmith(Player* player, Card* card) const;
    bool canLift(Player* player) const;
};

#endif // CAMPFIRE_H
