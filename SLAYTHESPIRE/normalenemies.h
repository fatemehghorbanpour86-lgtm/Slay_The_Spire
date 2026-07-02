#ifndef NORMALENEMIES_H
#define NORMALENEMIES_H

#include "enemy.h"

class Player;

class JawWorm : public Enemy
{
public:
    enum Move
    {
        Chomp,
        Thrash,
        Bellow
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

    enum Move
    {
        Bite,
        Grow
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

class SmallSlime : public Enemy
{
public:
    enum Move
    {
        Tackle,
        Lick
    };

    SmallSlime();
    virtual ~SmallSlime() override = default;

    virtual void chooseIntent() override;
    virtual void executeMove(Player* player) override;

private:
    void performTackle(Player* player);
    void performLick(Player* player);
};

#endif // NORMALENEMIES_H
