#ifndef COMBATCALCULATOR_H
#define COMBATCALCULATOR_H


class Character;
class Player;
class Enemy;

class CombatCalculator
{
public:

    // Returns the final damage after applying all combat modifiers.
    static int calculateDamage(Character* attacker,Character* defender,int baseDamage);

    // Deals damage and returns the actual HP damage dealt
    // after Block absorption.
    static int dealDamage(Character* attacker, Character* defender, int baseDamage);

    // Calculates the damage shown in enemy intent.
    static int calculateIntentDamage(Enemy* attacker, Player* defender,  int baseDamage);

    // Returns the final Block amount after modifiers.
    static int calculateBlock(Character* target, int baseBlock);

    // Grants Block after applying modifiers.
    static void grantBlock(Character* target, int baseBlock);

    // Returns true if the target would die.
    // Do not use in card or enemy, should only be used for UI;
    static bool willBeFatal(Character* attacker, Character* defender, int baseDamage);

    // Safe wrapper around Character::heal().
    static void healCharacter(Character* target, int amount);
};


#endif // COMBATCALCULATOR_H
