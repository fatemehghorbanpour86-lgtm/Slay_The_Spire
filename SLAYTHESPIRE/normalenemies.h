#ifndef NORMALENEMIES_H
#define NORMALENEMIES_H

#include "enemy.h"
#include <random>

class Player;

class JawWorm : public Enemy
{
public:
    enum class Move : int
    {
        Chomp = 1,
        Thrash = 2,
        Bellow = 3
    };

    JawWorm();
    virtual ~JawWorm() override = default;

    virtual void chooseIntent() override;
    virtual void executeMove(Player* player) override;

private:
    void performChomp(Player* player);
    void performThrash(Player* player);
    void performBellow();
};

class Louse : public Enemy
{
public:

    enum class Move : int
    {
        Bite = 1,
        Grow = 2
    };

    Louse();
    virtual ~Louse() override = default;

    virtual void chooseIntent() override;
    virtual void executeMove(Player* player) override;
    virtual void takeDamage(int damage) override;

private:

    void performBite(Player* player);
    void performGrow();

    bool firstHitTaken;
};

#endif // NORMALENEMIES_H
