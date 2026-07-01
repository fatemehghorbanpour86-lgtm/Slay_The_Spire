#include "statuscards.h"

//======================================================
// Daze
//======================================================

Daze::Daze()
    : Card("Dazed","Unplayable. Ethereal.",0,CardType::Status,
           false,true,false,true)
{
}


void Daze::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

    // This card cannot be played.

}


void Daze::upgrade()
{
    // Status cards cannot be upgraded.
}


//======================================================
// Slime
//======================================================

Slime::Slime()
    : Card("Slimed","Exhaust.",1,CardType::Status,true)
{
}


void Slime::play(Player* user, QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

}


void Slime::upgrade()
{
    // Status cards cannot be upgraded.
}


//======================================================
// Wound
//======================================================

Wound::Wound()
    : Card("Wound","Unplayable.",0,CardType::Status,
           false, false, false, true)
{
}


void Wound::play(Player* user,QVector<Enemy*>& enemies, Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

}


void Wound::upgrade()
{
    // Status cards cannot be upgraded.
}
