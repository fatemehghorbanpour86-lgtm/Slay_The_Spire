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


    virtual void use(Player* user, QVector<Enemy*>& enemies, Enemy* target = nullptr) = 0;


    virtual bool canUse(Player* user) const; //Default: true
};

#endif // POTION_H
