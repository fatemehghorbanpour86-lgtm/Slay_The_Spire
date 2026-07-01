#ifndef CARD_H
#define CARD_H

#include <QString>
#include <QVector>

class Player;
class Enemy; // future class

enum class CardType
{
    Attack,
    Skill,
    Power,
    Status,
    Curse
};

class Card
{
protected:
    QString name;
    QString description;
    int energyCost;
    CardType type;

    bool isExhaust;
    // Checked by CombatManager after the card is played.

    bool isEthereal;
    // Checked by CombatManager at end of turn.

    bool isRetain;
    // Checked by CombatManager when ending the turn.

    bool isInnate;
    // Checked by CombatManager at the start of each Combat.

    bool isUnplayable;
    bool isUpgraded;

    void setEnergyCost(int cost);
    // Used for upgrading.

public:

    Card(const QString& name, const QString& description, int energyCost, CardType type,
         bool exhaust = false, bool ethereal = false, bool retain = false, bool unplayable = false);

    virtual ~Card() = default;

    virtual void play(Player* user , QVector<Enemy*>& enemies, Enemy* target = nullptr) = 0;
    // Executes the card effect.
    // for each card type is diffrent.


    virtual void upgrade() = 0;
    // Each card type defines its own upgrade.

    QString getName() const;
    QString getDescription() const;
    int getEnergyCost() const;
    CardType getType() const;

    bool doesExhaust() const;
    bool doesEthereal() const;
    bool doesRetain() const;
    bool doesInnate() const;
    bool canPlay() const;
    bool getIsUpgraded() const;
};

#endif