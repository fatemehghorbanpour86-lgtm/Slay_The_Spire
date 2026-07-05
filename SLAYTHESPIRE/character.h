#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>
#include <QVector>
#include <QObject>
#include "effect.h"


class Character : public QObject
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
    void loseBlock(int amount);
    void clearBlock();
    void startTurnBlockReset();
    bool hasBlock() const;

    virtual void takeDamage(int damage);
    virtual void heal(int amount);

    void onTurnStartEffects();
    void onTurnEndEffects();
    void removeExpiredEffects();
    void removeEffect(Effect::Type type);
    bool hasEffect(Effect::Type type) const;
    const QVector<Effect*>& getEffects() const;
    void addEffect(Effect::Type type, Effect::Category category, int amount, int duration = -1);
    Effect* getEffect(Effect::Type type) const;

    void loseHP(int amount);

    bool isDead() const;
};

#endif
