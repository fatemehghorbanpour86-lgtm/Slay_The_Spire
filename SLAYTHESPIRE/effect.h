#ifndef EFFECT_H
#define EFFECT_H

#include <QString>

class Character;

class Effect
{
public:

    enum class Category
    {
        Buff,
        Debuff
    };

    enum class Type
    {
        Strength,
        Dexterity,
        Metallicize,

        Weak,
        Vulnerable,
        Frail,

        Rage,
        DemonForm,
        Brutality,
        Barricade
    };

    Effect(Type type, Category category, int amount, int duration = -1);

    QString getName() const;
    Type getType() const;
    Category getCategory() const;

    int getAmount() const;
    int getDuration() const;
    bool isExpired() const;
    bool isBuff() const;
    bool isDebuff() const;
    void stack(int value);
    void decreaseDuration();
    void onTurnStart(Character *target);
    void onTurnEnd(Character *target);

private:

    Type type;
    Category category;

    int amount;
    int duration;
};

#endif
