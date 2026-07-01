#ifndef STATUSCARDS_H
#define STATUSCARDS_H

#include "card.h"

class Player;
class Enemy;

class Daze : public Card
{
public:
    Daze();

    void play(Player* user,QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};


class Slime : public Card
{
public:
    Slime();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

class Wound : public Card
{
public:
    Wound();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

#endif // STATUSCARDS_H
