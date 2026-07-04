#ifndef EVENTRELICS_H
#define EVENTRELICS_H

#include "relic.h"

class Player;

class WarpedTongs : public Relic
{
public:
    WarpedTongs();

    void onCombatStart(Player* player) override;
};

class MutagenicStrength : public Relic
{
public:
    MutagenicStrength();

    void onCombatStart(Player* player) override;
    void onTurnEnd(Player* player) override;

private:
    bool strengthActive;
};

#endif // EVENTRELICS_H
