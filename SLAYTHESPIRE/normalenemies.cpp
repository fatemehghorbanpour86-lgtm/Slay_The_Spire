#include "normalenemies.h"

#include "player.h"
#include <QRandomGenerator>

JawWorm::JawWorm()
    : Enemy("Jaw Worm", QRandomGenerator::global()->bounded(42,47))
{
    //واسه ترن اول همیشه همینه
    setIntent(Intent::Attack);
    setCurrentMove(Chomp);
    setIntentDamage(11);
    setIntentHits(1);
}

void JawWorm::chooseIntent()
{
    if (getTurnCount() == 0)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Chomp);
        setIntentDamage(11);
        setIntentHits(1);
        return;
    }

    int roll = QRandomGenerator::global()->bounded(1,101);

    if (roll <= 25)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Chomp);
        setIntentDamage(11);
        setIntentHits(1);
    }
    else if (roll <= 55)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Thrash);
        setIntentDamage(7);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::DefendBuff);
        setCurrentMove(Bellow);
        setIntentDamage(0);
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

    int baseDamage = 11;
    player->takeDamage(baseDamage);
}
void JawWorm::performThrash(Player* player)
{
    if (player)
    {
        int baseDamage = 7;
        player->takeDamage(baseDamage);
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
            QRandomGenerator::global()->bounded(10,16)),
    firstHitTaken(false)
{
    chooseIntent();
}
void Louse::chooseIntent()
{
    int roll = QRandomGenerator::global()->bounded(1,101);

    if (roll <= 75)
    {
        int randomizedBiteDamage = QRandomGenerator::global()->bounded(5, 8);

        setIntent(Intent::Attack);
        setCurrentMove(Bite);
        setIntentDamage(randomizedBiteDamage);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Buff);
        setCurrentMove(Grow);
        setIntentDamage(0);
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
    player->takeDamage(getIntentDamage());
}
void Louse::performGrow()
{
    this->addEffect(Effect::Type::Strength, Effect::Category::Buff, 3, -1);
}



SmallSlime::SmallSlime()
    : Enemy("Small Slime", QRandomGenerator::global()->bounded(8, 13))
{
    chooseIntent();
}
void SmallSlime::chooseIntent()
{
    int roll = QRandomGenerator::global()->bounded(1, 101);

    if (roll <= 50)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Tackle);
        setIntentDamage(3);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Debuff);
        setCurrentMove(Lick);
        setIntentDamage(0);
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

    int baseDamage = 3;

    player->takeDamage(baseDamage);
}
void SmallSlime::performLick(Player* player)
{
    if (!player) return;
    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff, 1, 1);
}



MediumSlime::MediumSlime()
    : Enemy("Medium Slime", QRandomGenerator::global()->bounded(28, 33))
{
   chooseIntent();
}
void MediumSlime::chooseIntent()
{
    int roll = QRandomGenerator::global()->bounded(1, 101);

    if (roll <= 30)
    {
        setIntent(Intent::AttackDebuff);
        setCurrentMove(CorrosiveSpit);
        setIntentDamage(7);
        setIntentHits(1);
    }
    else if (roll <= 70)
    {
        setIntent(Intent::Attack);
        setCurrentMove(Tackle);
        setIntentDamage(10);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Debuff);
        setCurrentMove(Lick);
        setIntentDamage(0);
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

    int baseDamage = 7;
    player->takeDamage(baseDamage);

    // 2. Add SLIMED status card to the player's Discard Pile using the standard interface
    // player->addCardToDiscard(new SlimedCard());
}
void MediumSlime::performTackle(Player* player)
{
    if (!player) return;

    int baseDamage = 10;
    player->takeDamage(baseDamage);
}
void MediumSlime::performLick(Player* player)
{
    if (!player) return;
    player->addEffect(Effect::Type::Weak, Effect::Category::Debuff, 1, 1);
}