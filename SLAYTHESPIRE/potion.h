#ifndef POTION_H
#define POTION_H

#include <QString>
#include <QVector>

class Player;
class Enemy;

class Potion
{

protected:

    QString name;
    QString description;

public:

    Potion(const QString& name, const QString& description);

    virtual ~Potion() = default;


    QString getName() const;
    QString getDescription() const;


    virtual void use(Player* user, Enemy* target = nullptr) = 0;


    virtual bool canUse(Player* user) const; //Default: true
};

class BlockPotion : public Potion
{
private:
     int blockAmount;

public:
    BlockPotion();

    void use(Player* user, Enemy* target = nullptr) override;
};

class FirePotion : public Potion
{
public:
    FirePotion();

    void use(Player* user, Enemy* target = nullptr) override;
};

class EnergyPotion : public Potion
{
public:
    EnergyPotion();

    void use(Player* user, Enemy* target = nullptr) override;
};

class SwiftPotion : public Potion
{
public:
    SwiftPotion();

    void use(Player* user, Enemy* target = nullptr) override;
};

class FairyInABottle : public Potion
{
public:
    FairyInABottle();

    void use(Player* user, Enemy* target = nullptr) override;

    bool canUse(Player* user) const override;
};

#endif // POTION_H
