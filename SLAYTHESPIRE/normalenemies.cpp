#include "normalenemies.h"

#include "player.h"
#include <QRandomGenerator>

JawWorm::JawWorm()
    : Enemy("Jaw Worm", QRandomGenerator::global()->bounded(42,46))
{
    //واسه ترن اول همیشه همینه
    setIntent(Intent::Attack);
    setCurrentMove(static_cast<int>(Move::Chomp));
    setIntentDamage(11);
    setIntentHits(1);
}

void JawWorm::chooseIntent()
{
    if (getTurnCount() == 0)
    {
        setIntent(Intent::Attack);
        setCurrentMove(static_cast<int>(Move::Chomp));
        setIntentDamage(11);
        setIntentHits(1);
        return;
    }

    int roll = QRandomGenerator::global()->bounded(1,100);

    if (roll <= 25)
    {
        setIntent(Intent::Attack);
        setCurrentMove(static_cast<int>(Move::Chomp));
        setIntentDamage(11);
        setIntentHits(1);
    }
    else if (roll <= 55)
    {
        setIntent(Intent::Attack);
        setCurrentMove(static_cast<int>(Move::Thrash));
        setIntentDamage(7);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::DefendBuff);
        setCurrentMove(static_cast<int>(Move::Bellow));
        setIntentDamage(0);
        setIntentHits(0);
    }
}
void JawWorm::executeMove(Player* player)
{
    switch (static_cast<Move>(getCurrentMove()))
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
    : Enemy(QRandomGenerator::global()->bounded(0,1) == 0 ? "Red Louse" : "Green Louse",
            QRandomGenerator::global()->bounded(10,15)),
    firstHitTaken(false)
{
    Louse::chooseIntent();
}
void Louse::chooseIntent()
{
    int roll = QRandomGenerator::global()->bounded(1,100);

    if (roll <= 75)
    {
        int randomizedBiteDamage = QRandomGenerator::global()->bounded(5, 7);

        setIntent(Intent::Attack);
        setCurrentMove(static_cast<int>(Move::Bite));
        setIntentDamage(randomizedBiteDamage);
        setIntentHits(1);
    }
    else
    {
        setIntent(Intent::Buff);
        setCurrentMove(static_cast<int>(Move::Grow));
        setIntentDamage(0);
        setIntentHits(0);
    }
}
void Louse::executeMove(Player* player)
{
    switch (static_cast<Move>(getCurrentMove()))
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

        int blockAmount = QRandomGenerator::global()->bounded(3, 7);

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


