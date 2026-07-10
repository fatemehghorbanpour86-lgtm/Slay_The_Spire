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

class CultistHeadpiece : public Relic
{
public:
    CultistHeadpiece();

    void onCombatStart(Player* player) override;
};

class GremlinVisage : public Relic
{
public:
    GremlinVisage();

    void onCombatStart(Player* player) override;
};

class FaceOfCleric : public Relic
{
public:
    FaceOfCleric();

    void onCombatEnd(Player* player) override;
};

class GoldenIdolRelic : public Relic
{
public:
    GoldenIdolRelic();
    void onEnemyDeath(Player* player, Enemy* enemy) override;
};

#endif // EVENTRELICS_H
