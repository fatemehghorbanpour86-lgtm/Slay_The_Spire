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