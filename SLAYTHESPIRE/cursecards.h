#ifndef CURSECARDS_H
#define CURSECARDS_H

#include "card.h"

class Player;
class Enemy;


class JAX : public Card
{
public:
    JAX();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

class CurseOfTheBell : public Card
{
public:
    CurseOfTheBell();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

#endif // CURSECARDS_H
