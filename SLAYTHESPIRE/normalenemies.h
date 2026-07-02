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

    static int generateRandomHP();

    std::mt19937 randomEngine;
};

#endif // NORMALENEMIES_H
