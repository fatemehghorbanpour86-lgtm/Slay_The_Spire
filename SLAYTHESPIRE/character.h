#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>
#include <QVector>
#include "effect.h"


class Character
{
protected:

    QString name;
    int maxHealth;
    int currentHealth;
    int block;
    QVector<Effect*> activeEffects;

protected:

    void setBlock(int value);
    void removeBlock(int amount);
    int absorbBlock(int damage);

public:
    Character();
    Character(const QString& name, int maxHealth);
    virtual ~Character();

    QString getName() const;
    int getCurrentHealth() const;
    int getMaxHealth() const;
    int getBlock() const;


    void addBlock(int amount);
    void clearBlock();

    virtual void takeDamage(int damage);
    virtual void heal(int amount);

    void addEffect(Effect::Type type, Effect::Category category, int amount, int duration = -1);
    Effect* getEffect(Effect::Type type) const;

    void loseHP(int amount);

    bool isDead() const;
};

#endif