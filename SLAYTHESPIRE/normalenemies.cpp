#include "normalenemies.h"

#include "player.h"
#include <QRandomGenerator>

JawWorm::JawWorm()
    : Enemy("Jaw Worm", QRandomGenerator::global()->bounded(42,47))
{
}

void JawWorm::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    if (getTurnCount() == 0)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Chomp);
        setIntentDamage(11);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
        return;
    }

    int roll = QRandomGenerator::global()->bounded(1,101);

    if (roll <= 25)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Chomp);
        setIntentDamage(11);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else if (roll <= 55)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Thrash);
        setIntentDamage(7);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::DefendBuff);
        setCurrentMove(Bellow);
        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
    }
}
void JawWorm::executeMove(Player* player)
{
    switch (getCurrentMove())
    {
    case Move::Chomp:
        performChomp(player);
        break;

    case Move::Thrash:
        performThrash(player);
        break;

    case Move::Bellow:
        performBellow();
        break;

    default:
        break;
    }
}
void JawWorm::performChomp(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 11);

}
void JawWorm::performThrash(Player* player)
{
    if (player)
    {
        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 7);

    }
    this->addBlock(5);
}
void JawWorm::performBellow()
{
    this->addEffect(Effect::Type::Strength, Effect::Category::Buff, 3, -1);
    this->addBlock(6);
}



Louse::Louse()
    : Enemy(QRandomGenerator::global()->bounded(0,2) == 0 ? "Red Louse" : "Green Louse",
            QRandomGenerator::global()->bounded(10, 16)),
    firstHitTaken(false)
{
}
void Louse::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    int roll = QRandomGenerator::global()->bounded(1,101);

    if (roll <= 75)
    {
        int randomizedBiteDamage = QRandomGenerator::global()->bounded(5, 8);

        setIntent(Intent::Attack);
        setCurrentMove(Bite);
        setIntentDamage(randomizedBiteDamage);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Buff);
        setCurrentMove(Grow);
        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
    }
}
void Louse::executeMove(Player* player)
{
    switch (getCurrentMove())
    {
    case Move::Bite:
        performBite(player);
        break;

    case Move::Grow:
        performGrow();
        break;

    default:
        break;
    }
}
void Louse::takeDamage(int damage)
{
    Enemy::takeDamage(damage);

    if (!firstHitTaken)
    {
        firstHitTaken = true;

        int blockAmount = QRandomGenerator::global()->bounded(3, 8);

        this->addBlock(blockAmount);
    }
}
void Louse::performBite(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, getIntentDamage());

}
void Louse::performGrow()
{
    this->addEffect(Effect::Type::Strength, Effect::Category::Buff, 3, -1);
}



SmallSlime::SmallSlime()
    : Enemy("Small Slime", QRandomGenerator::global()->bounded(8, 13))
{
}
void SmallSlime::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    int roll = QRandomGenerator::global()->bounded(1, 101);

    if (roll <= 50)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Tackle);

        setIntentDamage(3);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Debuff);
        setCurrentMove(Lick);

        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
    }
}
void SmallSlime::executeMove(Player* player)
{
    switch (getCurrentMove())
    {
    case Move::Tackle:
        performTackle(player);
        break;

    case Move::Lick:
        performLick(player);
        break;

    default:
        break;
    }
}
void SmallSlime::performTackle(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 3);

}
void SmallSlime::performLick(Player* player)
{
    if (!player) return;
    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff, 1, 1);
}



MediumSlime::MediumSlime()
    : Enemy("Medium Slime", QRandomGenerator::global()->bounded(28, 33))
{
}
void MediumSlime::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    int roll = QRandomGenerator::global()->bounded(1, 101);

    if (roll <= 30)
    {
        setIntent(Intent::AttackDebuff);
        setCurrentMove(CorrosiveSpit);

        setIntentDamage(7);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else if (roll <= 70)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Tackle);

        setIntentDamage(10);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Debuff);
        setCurrentMove(Lick);

        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
    }
}
void MediumSlime::executeMove(Player* player)
{
    switch (getCurrentMove())
    {
    case Move::CorrosiveSpit:
        performCorrosiveSpit(player);
        break;

    case Move::Tackle:
        performTackle(player);
        break;

    case Move::Lick:
        performLick(player);
        break;

    default:
        break;
    }
}
void MediumSlime::performCorrosiveSpit(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 7);


    // 2. Add SLIMED status card to the player's Discard Pile using the standard interface
    // player->addCardToDiscard(new SlimedCard());
}
void MediumSlime::performTackle(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 10);

}
void MediumSlime::performLick(Player* player)
{
    if (!player) return;
    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff, 1, 1);
}




LargeSlime::LargeSlime()
    : Enemy("Large Slime", QRandomGenerator::global()->bounded(68, 73)),
    m_splitReady(false),
    m_splitExecuted(false),
    m_splitRequested(false)
{
}
void LargeSlime::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    if (!m_splitExecuted && (getCurrentHealth() <= (getMaxHealth() / 2)))
    {
        m_splitReady = true;
        setIntent(Intent::Unknown);
        setCurrentMove(Split);

        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
        return;
    }

    int roll = QRandomGenerator::global()->bounded(1, 101);

    if (roll <= 30)
    {
        setIntent(Intent::AttackDebuff);
        setCurrentMove(CorrosiveSpit);

        setIntentDamage(7);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else if (roll <= 70)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Tackle);

        setIntentDamage(10);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Debuff);
        setCurrentMove(Lick);

        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(0);
    }
}
void LargeSlime::executeMove(Player* player)
{
    switch (getCurrentMove())
    {
    case Move::CorrosiveSpit:
        performCorrosiveSpit(player);
        break;

    case Move::Tackle:
        performTackle(player);
        break;

    case Move::Lick:
        performLick(player);
        break;

    case Move::Split:
        performSplit();
        break;

    default:
        break;
    }
}
void LargeSlime::performCorrosiveSpit(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 7);


    // Add SLIMED status card to the player's Discard Pile using the integrated Deck System
    // player->addCardToDiscard(new SlimedCard());
}
void LargeSlime::performTackle(Player* player)
{
    if (!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 10);

}
void LargeSlime::performLick(Player* player)
{
    if (!player) return;
    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff, 1, 1);
}
void LargeSlime::performSplit()
{
    m_splitReady = false;
    m_splitExecuted = true;
    m_splitRequested = true; // Flag checked by CombatManager immediately after execution
    // کمبت منیجر این اتریبیوت رو چک میکنه و در صورت ترو بودنش هندل میکنه که به دوتا مدیوم با سطح سلامتی فعلی تبدیل بشن
}
bool LargeSlime::isSplitRequested() const
{
    return m_splitRequested;
}

//======================================================
//  Cultist
//======================================================


Cultist::Cultist()
    : Enemy("Cultist", QRandomGenerator::global()->bounded(48,55))
{

}


void Cultist::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    if(getTurnCount() == 0)
    {
        setCurrentMove(Incantation);

        setIntent(Intent::Buff);

        setIntentDamage(0);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.

        setIntentHits(1);

        return;
    }

    setCurrentMove(Stab);

    setIntent(Intent::Attack);

    setIntentDamage(6);

    setIntentHits(1);
}


void Cultist::executeMove(Player* player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {

    case Incantation:

        break;

    case Stab:

        addEffect(Effect::Type::Strength, Effect::Category::Buff, 3,-1);

        // TODO CombatCalculator (Ana)
        // CombatCalculator::dealDamage(this, player, 6);

        break;
    }
}


//======================================================
//  Thief
//======================================================

Thief::Thief(Type type)
    : Enemy(type == Looter ?"Looter" : "Mugger",
            type == Looter ?QRandomGenerator::global()->bounded(44,49) : QRandomGenerator::global()->bounded(52,57))
{
    thiefType = type;

    stolenGold = 0;
}


Thief::Type Thief::getType() const
{
    return thiefType;
}

int Thief::getStolenGold() const
{
    return stolenGold;
}

void Thief::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    if(getTurnCount() < 2)
    {
        setCurrentMove(Mug);

        setIntent(Intent::Attack);

        if(thiefType == Looter)
        {
            setIntentDamage(10);
        }
        else
        {
            setIntentDamage(14);
        }

        setIntentHits(1);

        return;
    }

    // Turn 3
    if(getTurnCount() == 2)
    {
        setCurrentMove(SmokeBomb);

        setIntent(Intent::Defend);

        setIntentDamage(0);

        setIntentHits(1);

        return;
    }

    // Turn 4
    setCurrentMove(Flee);

    setIntent(Intent::Escape);

    setIntentDamage(0);
    // TODO CombatCalculator
    // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.


    setIntentHits(1);
}


void Thief::executeMove(Player *player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {

    case Mug:

        if(thiefType == Looter)
        {
            // TODO CombatCalculator(Ana)
            // CombatCalculator::dealDamage(this, player, 10);
        }
        else
        {
            // TODO CombatCalculator(Ana)
            // CombatCalculator::dealDamage(this, player, 14);
        }


        player->spendGold(15);
        stolenGold += 15;

        break;

    case SmokeBomb:

        addBlock(6);

        break;
    }

    // Turn 4 Escape
    if(getTurnCount() >= 3)
    {
        // TODO CombatManager
        // Remove this enemy from combat.
        // If killed before escaping, return stolenGold to the player.
    }
}


//======================================================
//  BlueSlaver
//======================================================

BlueSlaver::BlueSlaver()
    : Enemy("Blue Slaver", QRandomGenerator::global()->bounded(46,51))
{
    lastMove = None;

    repeatCount = 0;
}



void BlueSlaver::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    Move selectedMove;

    if(repeatCount >= 2)
    {
        if(lastMove == Stab)
        {
            selectedMove = Rake;
        }
        else
        {
            selectedMove = Stab;
        }
    }
    else
    {
        int random = QRandomGenerator::global()->bounded(100);

        if(random < 60)
        {
            selectedMove = Stab;
        }
        else
        {
            selectedMove = Rake;
        }
    }


    if(selectedMove == lastMove)
    {
        repeatCount++;
    }

    else
    {
        lastMove = selectedMove;

        repeatCount = 1;
    }


    switch(selectedMove)
    {

     case None:
        break;

     case Stab:

        setCurrentMove(Stab);

        setIntent(Intent::Attack);

        setIntentDamage(12);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.


        setIntentHits(1);

        break;

     case Rake:

        setCurrentMove(Rake);

        setIntent(Intent::AttackDebuff);

        setIntentDamage(7);
        // TODO CombatCalculator
        // UI should display the final calculated damage after all buffs/debuffs are applied., not the base damage.


        setIntentHits(1);

        break;
    }

}

void BlueSlaver::executeMove(Player* player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {

     case Stab:

        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 12);

        break;


     case Rake:

        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 7);

        player->addEffect(Effect::Type::Weak,
            Effect::Category::Debuff,1,1);

        break;
    }
}


