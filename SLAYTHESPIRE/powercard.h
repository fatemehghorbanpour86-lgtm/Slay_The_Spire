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

    virtual Card* clone() const override;
};

class Metallicize : public Card
{
private:
    int blockAmount;

public:
    Metallicize();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class DemonForm : public Card
{
private:
    int strengthPerTurn;

public:
    DemonForm();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Brutality : public Card
{
public:
    Brutality();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Barricade : public Card
{
public:
    Barricade();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class FeelNoPain : public Card
{
private:
    int blockPerExhaust;

public:
    FeelNoPain();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class Berserk : public Card
{
private:
    int vulnerableAmount;

public:
    Berserk();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};

class DarkEmbrace : public Card
{
public:
    DarkEmbrace();
    void play(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) override;
    void upgrade() override;

    virtual Card* clone() const override;
};
#endif // POWERCARD_H
