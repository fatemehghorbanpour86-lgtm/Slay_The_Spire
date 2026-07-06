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

Card* JAX::clone() const
{
    return new JAX(*this);
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
}


void CurseOfTheBell::upgrade()
{
    // Curse cards cannot be upgraded.
}

bool CurseOfTheBell::isRemovable() const
{
    return false;
}

Card* CurseOfTheBell::clone() const
{
    return new CurseOfTheBell(*this);
}

//======================================================
// Regret
//======================================================

Regret::Regret()
    : Card("Regret",
           "Unplayable. At the end of your turn, lose HP equal to the number of cards in your hand.",
           0,CardType::Curse,false,false,false,true)
{
}


void Regret::play(Player* user, QVector<Enemy*>& enemies,Enemy* target)
{
    Q_UNUSED(user)
    Q_UNUSED(enemies)
    Q_UNUSED(target)
}

void Regret::upgrade()
{
    // Curse cards cannot be upgraded.
}

Card* Regret::clone() const
{
    return new Regret(*this);
}
