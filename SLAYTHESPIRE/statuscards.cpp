#include "statuscards.h"

//======================================================
// Dazed
//======================================================

Dazed::Dazed()
    : Card("Dazed","Unplayable. Ethereal.",0,CardType::Status,
           false,true,false,true)
{
}


void Dazed::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

    // This card cannot be played.

}


void Dazed::upgrade()
{
    // Status cards cannot be upgraded.
}