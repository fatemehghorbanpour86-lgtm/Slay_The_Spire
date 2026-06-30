#ifndef ATTACK_CARDS_H
#define ATTACK_CARDS_H

#include "card.h"

class Player;
class Enemy;

class Strike : public Card
{
  private:
    int damageAmount;

public:

    Strike();

    void play(Player* user , QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;
};

class Reaper : public Card
{
private:
    int damageAmount;

public:
    Reaper();

    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;

    void upgrade() override;
};

class Bludgeon : public Card
{
private:
    int damageAmount;

public:
    Bludgeon();

    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;

    void upgrade() override;
};

class Feed : public Card
{
private:
    int damageAmount;

public:
    Feed();

    void play(Player* user,QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};

class Immolate : public Card
{
private:
    int damageAmount;

public:
    Immolate();

    void play(Player* user,QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};


class TwinStrike : public Card
{
private:
    int damageAmount;

public:
    TwinStrike();

    void play(Player* user, QVector<Enemy*>& enemies,Enemy* target = nullptr) override;

    void upgrade() override;
};


#endif // ATTACK_CARDS_H
