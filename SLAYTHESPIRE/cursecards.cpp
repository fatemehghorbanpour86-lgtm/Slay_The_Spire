#include "cursecards.h"
#include "player.h"

//======================================================
// J.A.X.
//======================================================

JAX::JAX()
    : Card("J.A.X.","Lose 3 HP. Gain 2 Strength.",0,CardType::Curse)
{
}

void JAX::play(Player* user, QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(enemies)
    Q_UNUSED(target)

    if(user == nullptr)
    {
        return;
    }

    user->loseHP(3);

    user->addEffect( Effect::Type::Strength,Effect::Category::Buff, 2);
}

void JAX::upgrade()
{
    // Curse cards cannot be upgraded.
}


//======================================================
// CurseOfTheBell
//======================================================

CurseOfTheBell::CurseOfTheBell()
    : Card("Curse of the Bell", "Unplayable.",0, CardType::Curse,
           false,false,false,true)
{
}

void CurseOfTheBell::play(Player* user, QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

    // This card cannot be played.
    // TODO (Deck System):
    // This card cannot be permanently removed from the player's deck.
}


void CurseOfTheBell::upgrade()
{
    // Curse cards cannot be upgraded.
}
