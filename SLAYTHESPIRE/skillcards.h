#ifndef SKILLCARDS_H
#define SKILLCARDS_H

#include "card.h"

class Defend : public Card
{
private:
    int blockAmount;

public:
    Defend();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Exhume : public Card
{
public:
    Exhume();
    void play(Player* user , QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class LimitBreak : public Card
{
public:
    LimitBreak();
    void play(Player* user , QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Offering : public Card
{
private:
    int drawcards;

public:
    Offering();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Impervious : public Card
{
private:
    int blockAmount;

public:
    Impervious();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class ShrugItOff : public Card
{
private:
    int blockAmount;

public:
    ShrugItOff();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class TrueGrit : public Card
{
private:
    int blockAmount;

public:
    TrueGrit();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Rage : public Card
{
private:
    int blockPerAttack;

public:
    Rage();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};
#endif // SKILLCARDS_H
