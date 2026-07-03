#ifndef NORMALRELICS_H
#define NORMALRELICS_H

#include "relic.h"

class Player;
class Card;

//STARTER RELIC******************************************************
class BurningBlood : public Relic
{
public:
    BurningBlood();

    void onCombatEnd(Player* player) override;
};


//NORMAL RELICS**************************************************
class Girya : public Relic
{
public:
    Girya();

    bool canLift() const;
    void lift(Player* player);

private:
    static const int MAX_LIFTS = 3;
};

#endif // NORMALRELICS_H
