#include "normalenemies.h"

#include "player.h"
#include <chrono>

int JawWorm::generateRandomHP()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(42, 46);
    return dis(gen);
}
JawWorm::JawWorm()
    : Enemy("Jaw Worm", generateRandomHP())
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    randomEngine.seed(static_cast<unsigned long>(seed));

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

    std::uniform_int_distribution<int> dist(1, 100);
    int roll = dist(randomEngine);

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


int Louse::generateRandomHP()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(10, 15);
    return dis(gen);
}
Louse::LouseType Louse::chooseRandomType()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1);
    return dis(gen) == 0 ? LouseType::Red : LouseType::Green;
}
Louse::Louse(LouseType randomType, int randomHP)
    : Enemy(randomType == LouseType::Red ? "Red Louse" : "Green Louse", randomHP),
    type(randomType),
    firstHitTaken(false)
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    randomEngine.seed(static_cast<unsigned long>(seed));
    Louse::chooseIntent();
}
Louse::Louse()
    : Louse(chooseRandomType(), generateRandomHP())
{
}
void Louse::chooseIntent()
{
    std::uniform_int_distribution<int> dist(1, 100);
    int roll = dist(randomEngine);

    if (roll <= 75)
    {
        std::uniform_int_distribution<int> damageDist(5, 7);
        int randomizedBiteDamage = damageDist(randomEngine);

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

        std::uniform_int_distribution<int> blockDist(3, 7);
        int blockAmount = blockDist(randomEngine);

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


