#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

class Player;

enum class Intent
{
    Attack,
    Defend,
    Buff,
    Debuff,

    AttackDefend,
    AttackBuff,
    AttackDebuff,
    DefendBuff,

    Escape,
    Unknown
};

class Enemy : public Character
{
     Q_OBJECT

signals:
    void died(Enemy* self);

protected:

    Intent currentIntent;

    int currentMove;
    // Stores the selected move.
    // Each derived enemy defines its own move IDs using an enum.

    int intentDamage;
    // Damage shown in the enemy intent.

    int intentHits;
    // Number of hits shown in the enemy intent.

    int turnCount;
    // Number of turns this enemy has taken.

protected:

    void setIntent(Intent intent);
    // Used by derived enemies to update their next action.

    void setCurrentMove(int move);

    void setIntentDamage(int damage);
    // Updates the displayed damage of the current intent.

    void setIntentHits(int hits);
    // Updates the displayed hit count of the current intent.

    void increaseTurnCount();
    // Called by CombatManager after this enemy finishes its turn.

public:

    void takeDamage(int amount) override;

    Enemy(const QString& name, int maxHealth);

    virtual ~Enemy() = default;

    Intent getIntent() const;

    int getCurrentMove() const;

    int getIntentDamage() const;

    int getIntentHits() const;

    int getTurnCount() const;

    virtual void chooseIntent(Player* player) = 0;
    // Chooses the next intent.
    // Each enemy implements its own Logic.

    virtual void executeMove(Player* player) = 0;
    // Executes the selected move.
    // Damage should be calculated through CombatCalculator.
    // Other effects (Block, Heal, Buffs, Debuffs, Card generation) should be applied directly here.
    // Combat Deck is ToDo so just comment Card generation to apply later.
    // Example = //combatDeck->addToDiscard( new Burn()).
};

#endif // ENEMY_H