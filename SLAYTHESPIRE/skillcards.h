#ifndef SKILLCARDS_H
#define SKILLCARDS_H

#include "card.h"

class Defend : public Card
{
private:
    int blockAmount;

public:
    Defend();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target) override;
    void upgrade() override;
};

class Exhume : public Card
{
public:
    Exhume();
    void play(Player* user , QVector<Enemy*>& enemies, Enemy* target) override;
    void upgrade() override;
};

#endif // SKILLCARDS_H
