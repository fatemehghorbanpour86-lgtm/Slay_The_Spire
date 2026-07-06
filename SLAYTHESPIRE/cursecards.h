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

    virtual Card* clone() const override;
};

class CurseOfTheBell : public Card
{
public:
    CurseOfTheBell();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;

    bool isRemovable() const override;

    virtual Card* clone() const override;

};


class Regret : public Card
{
public:
    Regret();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;

    virtual Card* clone() const override;
};

#endif // CURSECARDS_H
