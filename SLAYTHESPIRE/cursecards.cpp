#include "cursecards.h"


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
