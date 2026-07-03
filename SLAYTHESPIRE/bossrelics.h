#ifndef BOSSRELICS_H
#define BOSSRELICS_H

#include "relic.h"

class Player;
class Card;
class Enemy;

class CallingBell : public Relic
{
public:
    CallingBell();

    void onEquip(Player* player) override;
};

class MarkOfPain : public Relic
{
public:
    MarkOfPain();

    void onEquip(Player* player) override;
    void onCombatStart(Player* player) override;
};

#endif // BOSSRELICS_H
