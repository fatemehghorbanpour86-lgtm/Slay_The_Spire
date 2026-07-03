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

#endif // NORMALRELICS_H
