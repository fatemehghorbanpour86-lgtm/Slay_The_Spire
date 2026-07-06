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

Card* Daze::clone() const
{
    return new Daze(*this);
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

Card* Slime::clone() const
{
    return new Slime(*this);
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

Card* Wound::clone() const
{
    return new Wound(*this);
}


//======================================================
// Burn
//======================================================

Burn::Burn()
    : Card("Burn", "Unplayable. At the end of your turn, take 2 damage.",0, CardType::Status,
           false, false, false, true),damageAmount(2)
{
}


void Burn::play(Player* user,QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)

}



void Burn::upgrade()
{
    if(isUpgraded)
    {
        return;
    }

    isUpgraded = true;

    damageAmount = 4;

    name = "Burn+";

    description = "Unplayable. At the end of your turn, take 4 damage.";
}

int Burn::getDamageAmount() const
{
    return damageAmount;
}

Card* Burn::clone() const
{
    return new Burn(*this);
}

