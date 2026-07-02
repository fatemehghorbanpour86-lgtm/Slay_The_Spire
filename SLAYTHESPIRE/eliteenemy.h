#ifndef ELITEENEMY_H
#define ELITEENEMY_H

#include "enemy.h"

class GremlinNob : public Enemy
{
private:

    enum Move
    {
        Bellow,
        Rush,
        SkullBash
    };

    bool enrageActive;

public:

    GremlinNob();

    bool isEnraged() const;

    void chooseIntent(Player* player) override;

    void executeMove(Player* player) override;
};

class Sentry : public Enemy
{
private:

    enum Move
    {
        Beam,
        Bolt
    };

    bool startsWithBeam;

    // In CombatManager
    //Enemy* left = new Sentry(false);
    //Enemy* middle = new Sentry(true);
    //Enemy* right = new Sentry(false);


public:

    explicit Sentry(bool startsWithBeam);

    void chooseIntent(Player* player) override;

    void executeMove(Player* player) override;
};

class BookOfStabbing : public Enemy
{
private:
    enum Move
    {
        MultiStab,
        SingleStab
    };
    int multiStabCount;
    // Number of times Multi-Stab has been used so far.
    // N = multiStabCount + 2 hits on the NEXT Multi-Stab use.
    void performMultiStab(Player *player);
    void performSingleStab(Player *player);

public:
    BookOfStabbing();
    void chooseIntent(Player* player) override;
    void executeMove(Player* player) override;
};

#endif // ELITEENEMY_H
