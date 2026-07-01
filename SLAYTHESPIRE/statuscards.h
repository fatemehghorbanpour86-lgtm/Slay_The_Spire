#ifndef STATUSCARDS_H
#define STATUSCARDS_H

#include "card.h"

class Player;
class Enemy;

class Dazed : public Card
{
public:
    Dazed();

    void play(Player* user,QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

#endif // STATUSCARDS_H
