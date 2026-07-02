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

class MediumSlime : public Enemy
{
public:
    enum Move
    {
        CorrosiveSpit,
        Tackle,
        Lick
    };

    MediumSlime();
    virtual ~MediumSlime() override = default;

    virtual void chooseIntent() override;
    virtual void executeMove(Player* player) override;

private:
    void performCorrosiveSpit(Player* player);
    void performTackle(Player* player);
    void performLick(Player* player);
};

class LargeSlime : public Enemy
{
public:
    enum Move
    {
        CorrosiveSpit,
        Tackle,
        Lick,
        Split
    };

    LargeSlime();
    virtual ~LargeSlime() override = default;

    virtual void chooseIntent() override;
    virtual void executeMove(Player* player) override;

    bool isSplitRequested() const;

private:
    void performCorrosiveSpit(Player* player);
    void performTackle(Player* player);
    void performLick(Player* player);
    void performSplit();

    bool m_splitReady;      // True if HP <= 50% and locked into Split intent for the next turn
    bool m_splitExecuted;   // True once the Split move has run to prevent multiple activations
    bool m_splitRequested;  // Signal flagged to notify CombatManager to perform array substitution
};

#endif // NORMALENEMIES_H
