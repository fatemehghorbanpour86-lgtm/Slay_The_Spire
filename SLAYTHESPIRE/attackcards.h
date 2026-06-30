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

    void play(Player* user , std ::vector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;
};

class Reaper : public Card
{
private:
    int damageAmount;

public:
    Reaper();

    void play(Player* user, std::vector<Enemy*>& enemies, Enemy* target = nullptr) override;

    void upgrade() override;
};

#endif // ATTACK_CARDS_H
