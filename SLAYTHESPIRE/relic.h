#ifndef RELIC_H
#define RELIC_H

#include <QString>

class Player;
class Card;
class Enemy;

class Relic
{
public:
    enum class Tier
    {
        Starter,
        Normal,
        Boss,
        Event
    };


protected:
    QString name;
    QString description;
    Tier tier;
    int counter;

public:
    Relic(const QString& name, const QString& description, Tier tier);
    virtual ~Relic() = default;

    QString getName() const;
    QString getDescription() const;
    Tier getTier() const;

    int getCounter() const;
    void setCounter(int value);


    virtual void onCombatStart(Player* player);
    // Called once when a combat starts.
    // Used by relics that trigger at the beginning of combat.

    virtual void onCombatEnd(Player* player);
    // Called once after combat ends.
    // Used by relics with end-of-combat effects.

    virtual void onTurnStart(Player* player);
    // Called at the beginning of each player turn.
    // Used by relics with turn-based effects.

    virtual void onTurnEnd(Player* player);
    // Called at the end of each player turn.
    // Used by relics that trigger after the player's turn.

    virtual void onCardPlayed(Player* player, Card* card);
    // Called whenever the player plays a card.
    // Used by relics that react to card usage.

    virtual void onEnemyDeath(Player* player, Enemy* enemy);
    // Called whenever an enemy dies.
    // Used by relics that react to enemy deaths.


    virtual int modifyDamageTaken(int damage);
    // Allows the relic to modify incoming damage
    // before it is applied.

    virtual int modifyDamageDealt(int damage);
    // Allows the relic to modify outgoing damage
    // before it is dealt.


    virtual bool canRest() const;
    // Returns whether the player is allowed to Rest at Campfires.

    virtual bool canUsePotions() const;
    // Returns whether the player is allowed to use Potions.

    virtual void onChestOpened(Player* player);
    // Called when the player opens a Chest Used by relics with chest-related effects.
};

#endif // RELIC_H
