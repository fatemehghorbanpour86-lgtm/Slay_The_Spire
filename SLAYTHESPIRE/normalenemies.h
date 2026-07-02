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


class Cultist : public Enemy
{
private:

    enum Move
    {
        Incantation,
        Stab
    };

public:

    Cultist();

    void chooseIntent() override;

    void executeMove(Player* player) override;
};

class Thief : public Enemy
{
private:

    enum Move
    {
        Mug,
        SmokeBomb,
        Flee
    };

public:

    enum Type
    {
        Looter,
        Mugger
    };

private:

    Type thiefType;

    int stolenGold;

public:

    explicit Thief(Type type);

    Type getType() const;

    int getStolenGold() const;

    void chooseIntent() override;

    void executeMove(Player *player) override;
};

class BlueSlaver : public Enemy
{
private:

    enum Move
    {
        None,
        Stab,
        Rake
    };

    Move lastMove;

    int repeatCount;

public:

    BlueSlaver();

    void chooseIntent() override;

    void executeMove(Player* player) override;
};

#endif // NORMALENEMIES_H