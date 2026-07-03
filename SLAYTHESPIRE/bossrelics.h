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

class VelvetChoker : public Relic
{
public:
    VelvetChoker();

    void onEquip(Player* player) override;
    void onTurnStart(Player* player) override;
    void onCardPlayed(Player* player, Card* card) override;
    bool canPlayCard(Player* player) const override;

private:
    static const int MAX_CARDS_PER_TURN = 6;
};

#endif // BOSSRELICS_H
