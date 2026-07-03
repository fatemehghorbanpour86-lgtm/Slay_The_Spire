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

class IceCream : public Relic
{
public:
    IceCream();

    void onTurnEnd(Player* player) override;
    void onTurnStart(Player* player) override;
};

class Shuriken : public Relic
{
public:
    Shuriken();

    void onCardPlayed(Player* player, Card* card) override;

private:
    static const int ATTACKS_REQUIRED = 3;
};

class Kunai : public Relic
{
public:
    Kunai();

    void onCardPlayed(Player* player, Card* card) override;

private:
    static const int ATTACKS_REQUIRED = 3;
};

#endif // NORMALRELICS_H
