#ifndef POWERCARD_H
#define POWERCARD_H

#include "card.h"

class Inflame : public Card
{
private:
    int strengthAmount;

public:
    Inflame();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;
};

class Metallicize : public Card
{
private:
    int blockAmount;

public:
    Metallicize();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;
};

#endif // POWERCARD_H
